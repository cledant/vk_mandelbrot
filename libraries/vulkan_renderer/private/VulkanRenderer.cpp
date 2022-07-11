#include "VulkanRenderer.hpp"

#include <cstdint>
#include <stdexcept>
#include <cassert>
#include <cstring>

#include "VulkanDebug.hpp"
#include "utils/VulkanCommandBuffer.hpp"
#include "utils/VulkanMemory.hpp"
#include "utils/VulkanTextureUtils.hpp"

void
VulkanRenderer::createInstance(std::string &&appName,
                               std::string &&engineName,
                               uint32_t appVersion,
                               uint32_t engineVersion,
                               std::vector<char const *> &&requiredExtensions)
{
    if constexpr (ENABLE_VALIDATION_LAYER) {
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    _appName = std::move(appName);
    _appVersion = appVersion;
    _engineName = std::move(engineName);
    _engineVersion = engineVersion;
    _vkInstance.instance =
      VulkanInstance::createInstance(_appName,
                                     _engineName,
                                     _appVersion,
                                     _engineVersion,
                                     std::move(requiredExtensions));
}

VkInstance
VulkanRenderer::getVkInstance() const
{
    return (_vkInstance.instance);
}

void
VulkanRenderer::init(VkSurfaceKHR surface,
                     VulkanInstanceOptions const &options,
                     uint32_t winW,
                     uint32_t winH)
{
    assert(surface);

    _vkInstance.init(surface, options);
    _swapChain.init(_vkInstance, winW, winH);
    _sync.init(_vkInstance, _swapChain.swapChainImageViews.size());
    _toScreenRenderPass.init(_vkInstance, _swapChain);
    loadTextureFromFile(_vkInstance.devices,
                        _vkInstance.cmdPools,
                        _vkInstance.queues,
                        "resources/textures/chibiRenko.jpg",
                        _dbgTexture);
    _toScreen.init(_vkInstance,
                   _swapChain,
                   _toScreenRenderPass,
                   { _dbgTexture.textureSampler,
                     _dbgTexture.textureImgView,
                     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL });
    allocateCommandBuffers(_vkInstance.devices.device,
                           _vkInstance.cmdPools.renderCommandPool,
                           _renderCommandBuffers,
                           _swapChain.swapChainImageViews.size());
}

void
VulkanRenderer::resize(uint32_t winW, uint32_t winH)
{
    vkDeviceWaitIdle(_vkInstance.devices.device);
    if (winW <= 0 || winH <= 0) {
        return;
    }

    _swapChain.resize(winW, winH);
    _sync.resize(_swapChain.currentSwapChainNbImg);
    _toScreenRenderPass.resize(_swapChain);
    _toScreen.resize(_swapChain,
                     _toScreenRenderPass,
                     { _dbgTexture.textureSampler,
                       _dbgTexture.textureImgView,
                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL });
    allocateCommandBuffers(_vkInstance.devices.device,
                           _vkInstance.cmdPools.renderCommandPool,
                           _renderCommandBuffers,
                           _swapChain.swapChainImageViews.size());
}

void
VulkanRenderer::clear()
{
    vkDeviceWaitIdle(_vkInstance.devices.device);
    _toScreen.clear();
    _toScreenRenderPass.clear();
    _dbgTexture.clear();
    _sync.clear();
    _swapChain.clear();
    _vkInstance.clear();
}

// Info related
std::string const &
VulkanRenderer::getAppName() const
{
    return (_appName);
}

uint32_t
VulkanRenderer::getAppVersion() const
{
    return (_appVersion);
}

std::string const &
VulkanRenderer::getEngineName() const
{
    return (_engineName);
}

uint32_t
VulkanRenderer::getEngineVersion() const
{
    return (_engineVersion);
}

// Render Related
void
VulkanRenderer::draw()
{
    vkWaitForFences(_vkInstance.devices.device,
                    1,
                    &_sync.inflightFence[_sync.currentFrame],
                    VK_TRUE,
                    UINT64_MAX);

    uint32_t img_index;
    auto result =
      vkAcquireNextImageKHR(_vkInstance.devices.device,
                            _swapChain.swapChain,
                            UINT64_MAX,
                            _sync.imageAvailableSem[_sync.currentFrame],
                            VK_NULL_HANDLE,
                            &img_index);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        return;
    }

    if (_sync.imgsInflightFence[img_index] != VK_NULL_HANDLE) {
        vkWaitForFences(_vkInstance.devices.device,
                        1,
                        &_sync.imgsInflightFence[img_index],
                        VK_TRUE,
                        UINT64_MAX);
    }
    _sync.imgsInflightFence[img_index] =
      _sync.inflightFence[_sync.currentFrame];

    recordRenderCmd(img_index, clearColor);
    emitDrawCmds(img_index);

    VkSwapchainKHR swap_chains[] = { _swapChain.swapChain };
    VkSemaphore present_wait_sems[] = {
        _sync.allRenderFinishedSem[_sync.currentFrame],
    };
    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = present_wait_sems;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swap_chains;
    present_info.pImageIndices = &img_index;
    present_info.pResults = nullptr;
    vkQueuePresentKHR(_vkInstance.queues.presentQueue, &present_info);
    _sync.currentFrame =
      (_sync.currentFrame + 1) % VulkanSync::MAX_FRAME_INFLIGHT;
}

void
VulkanRenderer::emitDrawCmds(uint32_t imgIndex)
{
    // Send scene rendering
    VkSemaphore finish_scene_sig_sems[] = {
        _sync.allRenderFinishedSem[_sync.currentFrame],
    };
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pWaitSemaphores = nullptr;
    submit_info.pWaitDstStageMask = nullptr;
    submit_info.waitSemaphoreCount = 0;
    submit_info.pSignalSemaphores = finish_scene_sig_sems;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pCommandBuffers = &_renderCommandBuffers[imgIndex];
    submit_info.commandBufferCount = 1;
    vkResetFences(
      _vkInstance.devices.device, 1, &_sync.inflightFence[_sync.currentFrame]);
    if (vkQueueSubmit(_vkInstance.queues.graphicQueue,
                      1,
                      &submit_info,
                      _sync.inflightFence[_sync.currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanRenderer: Failed to submit render draw command buffer");
    }
}

void
VulkanRenderer::recordRenderCmd(uint32_t imgIndex,
                                VkClearColorValue const &cmdClearColor)
{
    VkCommandBufferBeginInfo cb_begin_info{};
    cb_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cb_begin_info.flags = 0;
    cb_begin_info.pInheritanceInfo = nullptr;
    if (vkBeginCommandBuffer(_renderCommandBuffers[imgIndex], &cb_begin_info) !=
        VK_SUCCESS) {
        throw std::runtime_error("VulkanRenderer: Failed to begin "
                                 "recording render command buffer");
    }

    // Begin onscreen renderpass
    {
        std::array<VkClearValue, 2> clear_vals{};
        clear_vals[0].color = cmdClearColor;
        clear_vals[1].depthStencil = DEFAULT_CLEAR_DEPTH_STENCIL;
        VkRenderPassBeginInfo rp_begin_info{};
        rp_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rp_begin_info.renderPass = _toScreenRenderPass.renderPass;
        rp_begin_info.framebuffer = _toScreenRenderPass.framebuffers[imgIndex];
        rp_begin_info.renderArea.offset = { 0, 0 };
        rp_begin_info.renderArea.extent = _swapChain.swapChainExtent;
        rp_begin_info.clearValueCount = clear_vals.size();
        rp_begin_info.pClearValues = clear_vals.data();
        vkCmdBeginRenderPass(_renderCommandBuffers[imgIndex],
                             &rp_begin_info,
                             VK_SUBPASS_CONTENTS_INLINE);
        _toScreen.generateCommands(_renderCommandBuffers[imgIndex], imgIndex);
        vkCmdEndRenderPass(_renderCommandBuffers[imgIndex]);
    }

    if (vkEndCommandBuffer(_renderCommandBuffers[imgIndex]) != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanRenderer: Failed to record render command Buffer");
    }
}
