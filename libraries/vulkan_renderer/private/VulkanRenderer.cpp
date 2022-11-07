#include "VulkanRenderer.hpp"

#include <cstdint>
#include <stdexcept>
#include <cassert>

#include "VulkanDebug.hpp"
#include "utils/VulkanCommandBuffer.hpp"
#include "utils/VulkanMemory.hpp"
#include "utils/VulkanImageUtils.hpp"

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
    auto depthFormat =
      findSupportedFormat(_vkInstance.devices.physicalDevice,
                          { VK_FORMAT_D32_SFLOAT,
                            VK_FORMAT_D32_SFLOAT_S8_UINT,
                            VK_FORMAT_D24_UNORM_S8_UINT },
                          VK_IMAGE_TILING_OPTIMAL,
                          VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    _imageMandelbrot.init(_vkInstance,
                          VK_FORMAT_R8G8B8A8_UNORM,
                          depthFormat,
                          _swapChain.swapChainExtent.width,
                          _swapChain.swapChainExtent.height);
    _imageDisplayed.init(_vkInstance,
                         VK_FORMAT_R8G8B8A8_UNORM,
                         depthFormat,
                         _swapChain.swapChainExtent.width,
                         _swapChain.swapChainExtent.height);
    _toScreenRenderPass.init(_vkInstance, _swapChain);
    _toScreen.init(_vkInstance,
                   _swapChain,
                   _toScreenRenderPass,
                   _imageDisplayed.descriptorImage);
    _mandelbrotRenderPass.init(_vkInstance,
                               VK_FORMAT_R8G8B8A8_UNORM,
                               depthFormat,
                               _imageMandelbrot.colorTex.textureImgView,
                               _imageMandelbrot.depthTex.textureImgView,
                               _swapChain.swapChainExtent.width,
                               _swapChain.swapChainExtent.height);
    _mandelbrot.init(_vkInstance,
                     _mandelbrotRenderPass,
                     _swapChain.swapChainExtent.width,
                     _swapChain.swapChainExtent.height);
    _uiRenderPass.init(_vkInstance,
                       VK_FORMAT_R8G8B8A8_UNORM,
                       depthFormat,
                       _imageDisplayed.colorTex.textureImgView,
                       _imageDisplayed.depthTex.textureImgView,
                       _swapChain.swapChainExtent.width,
                       _swapChain.swapChainExtent.height);
    _ui.init(_vkInstance, _uiRenderPass, _swapChain.currentSwapChainNbImg);
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
    auto depthFormat =
      findSupportedFormat(_vkInstance.devices.physicalDevice,
                          { VK_FORMAT_D32_SFLOAT,
                            VK_FORMAT_D32_SFLOAT_S8_UINT,
                            VK_FORMAT_D24_UNORM_S8_UINT },
                          VK_IMAGE_TILING_OPTIMAL,
                          VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    _imageMandelbrot.resize(VK_FORMAT_R8G8B8A8_UNORM,
                            depthFormat,
                            _swapChain.swapChainExtent.width,
                            _swapChain.swapChainExtent.height);
    _imageDisplayed.resize(VK_FORMAT_R8G8B8A8_UNORM,
                           depthFormat,
                           _swapChain.swapChainExtent.width,
                           _swapChain.swapChainExtent.height);
    _toScreenRenderPass.resize(_swapChain);
    _toScreen.resize(
      _swapChain, _toScreenRenderPass, _imageDisplayed.descriptorImage);
    _mandelbrotRenderPass.resize(VK_FORMAT_R8G8B8A8_UNORM,
                                 depthFormat,
                                 _imageMandelbrot.colorTex.textureImgView,
                                 _imageMandelbrot.depthTex.textureImgView,
                                 _swapChain.swapChainExtent.width,
                                 _swapChain.swapChainExtent.height);
    _mandelbrot.resize(_mandelbrotRenderPass,
                       _swapChain.swapChainExtent.width,
                       _swapChain.swapChainExtent.height);
    _uiRenderPass.resize(VK_FORMAT_R8G8B8A8_UNORM,
                         depthFormat,
                         _imageDisplayed.colorTex.textureImgView,
                         _imageDisplayed.depthTex.textureImgView,
                         _swapChain.swapChainExtent.width,
                         _swapChain.swapChainExtent.height);
    _ui.resize(_uiRenderPass, _swapChain.currentSwapChainNbImg);
    allocateCommandBuffers(_vkInstance.devices.device,
                           _vkInstance.cmdPools.renderCommandPool,
                           _renderCommandBuffers,
                           _swapChain.swapChainImageViews.size());
}

void
VulkanRenderer::clear()
{
    vkDeviceWaitIdle(_vkInstance.devices.device);
    _ui.clear();
    _uiRenderPass.clear();
    _mandelbrot.clear();
    _mandelbrotRenderPass.clear();
    _toScreen.clear();
    _toScreenRenderPass.clear();
    _imageDisplayed.clear();
    _imageMandelbrot.clear();
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

    if (!mandelbrotComputeDone) {
        // Update push constant values
        mandelbrotConstants.fwW = _imageDisplayed.colorTex.width;
        mandelbrotConstants.fwH = _imageDisplayed.colorTex.height;
        recordMandelbrotRenderCmd(imgIndex, cmdClearColor);
        mandelbrotComputeDone = true;
    }
    recordUiRenderCmd(imgIndex, cmdClearColor);
    recordToScreenRenderCmd(imgIndex, cmdClearColor);

    if (vkEndCommandBuffer(_renderCommandBuffers[imgIndex]) != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanRenderer: Failed to record render command Buffer");
    }
}

void
VulkanRenderer::recordMandelbrotRenderCmd(
  uint32_t imgIndex,
  VkClearColorValue const &cmdClearColor)
{
    // Begin Mandelbrot renderpass
    std::array<VkClearValue, 2> clear_vals{};
    clear_vals[0].color = cmdClearColor;
    clear_vals[1].depthStencil = DEFAULT_CLEAR_DEPTH_STENCIL;
    VkRenderPassBeginInfo rp_begin_info{};
    rp_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin_info.renderPass = _mandelbrotRenderPass.renderPass;
    rp_begin_info.framebuffer = _mandelbrotRenderPass.framebuffer;
    rp_begin_info.renderArea.offset = { 0, 0 };
    rp_begin_info.renderArea.extent = {
        static_cast<uint32_t>(_imageMandelbrot.colorTex.width),
        static_cast<uint32_t>(_imageMandelbrot.colorTex.height),
    };
    rp_begin_info.clearValueCount = clear_vals.size();
    rp_begin_info.pClearValues = clear_vals.data();

    vkCmdBeginRenderPass(_renderCommandBuffers[imgIndex],
                         &rp_begin_info,
                         VK_SUBPASS_CONTENTS_INLINE);
    _mandelbrot.generateCommands(_renderCommandBuffers[imgIndex],
                                 mandelbrotConstants);
    vkCmdEndRenderPass(_renderCommandBuffers[imgIndex]);
}

void
VulkanRenderer::recordUiRenderCmd(uint32_t imgIndex,
                                  VkClearColorValue const &cmdClearColor)
{
    // TODO: refactor
    // Copy imageMandelbrot into imageDisplayed
    VkImageCopy imageCopyRegionColor{};
    imageCopyRegionColor.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageCopyRegionColor.srcSubresource.layerCount = 1;
    imageCopyRegionColor.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageCopyRegionColor.dstSubresource.layerCount = 1;
    imageCopyRegionColor.extent.width = _imageMandelbrot.colorTex.width;
    imageCopyRegionColor.extent.height = _imageMandelbrot.colorTex.height;
    imageCopyRegionColor.extent.depth = 1;

    VkImageCopy imageCopyRegionDepth{};
    imageCopyRegionDepth.srcSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    imageCopyRegionDepth.srcSubresource.layerCount = 1;
    imageCopyRegionDepth.dstSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    imageCopyRegionDepth.dstSubresource.layerCount = 1;
    imageCopyRegionDepth.extent.width = _imageMandelbrot.depthTex.width;
    imageCopyRegionDepth.extent.height = _imageMandelbrot.depthTex.height;
    imageCopyRegionDepth.extent.depth = 1;

    // Transition layouts for transfert
    transitionImageLayout(_renderCommandBuffers[imgIndex],
                          _imageMandelbrot.colorTex,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                          VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    transitionImageLayout(_renderCommandBuffers[imgIndex],
                          _imageMandelbrot.depthTex,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                          VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    transitionImageLayout(_renderCommandBuffers[imgIndex],
                          _imageDisplayed.colorTex,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    transitionImageLayout(_renderCommandBuffers[imgIndex],
                          _imageDisplayed.depthTex,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // Issue the copy commands
    vkCmdCopyImage(_renderCommandBuffers[imgIndex],
                   _imageMandelbrot.colorTex.textureImg,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   _imageDisplayed.colorTex.textureImg,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1,
                   &imageCopyRegionColor);
    vkCmdCopyImage(_renderCommandBuffers[imgIndex],
                   _imageMandelbrot.depthTex.textureImg,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   _imageDisplayed.depthTex.textureImg,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1,
                   &imageCopyRegionDepth);

    // Transition layouts
    transitionImageLayout(_renderCommandBuffers[imgIndex],
                          _imageMandelbrot.colorTex,
                          VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    transitionImageLayout(_renderCommandBuffers[imgIndex],
                          _imageMandelbrot.depthTex,
                          VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // Begin Ui renderpass
    std::array<VkClearValue, 2> clear_vals{};
    clear_vals[0].color = cmdClearColor;
    clear_vals[1].depthStencil = DEFAULT_CLEAR_DEPTH_STENCIL;
    VkRenderPassBeginInfo rp_begin_info{};
    rp_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin_info.renderPass = _uiRenderPass.renderPass;
    rp_begin_info.framebuffer = _uiRenderPass.framebuffer;
    rp_begin_info.renderArea.offset = { 0, 0 };
    rp_begin_info.renderArea.extent = {
        static_cast<uint32_t>(_imageDisplayed.colorTex.width),
        static_cast<uint32_t>(_imageDisplayed.colorTex.height),
    };
    rp_begin_info.clearValueCount = clear_vals.size();
    rp_begin_info.pClearValues = clear_vals.data();

    vkCmdBeginRenderPass(_renderCommandBuffers[imgIndex],
                         &rp_begin_info,
                         VK_SUBPASS_CONTENTS_INLINE);
    _ui.generateCommands(_renderCommandBuffers[imgIndex]);
    vkCmdEndRenderPass(_renderCommandBuffers[imgIndex]);
}

void
VulkanRenderer::recordToScreenRenderCmd(uint32_t imgIndex,
                                        VkClearColorValue const &cmdClearColor)
{
    // Begin onscreen renderpass
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
