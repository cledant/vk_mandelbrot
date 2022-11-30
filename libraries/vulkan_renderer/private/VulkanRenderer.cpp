#include "VulkanRenderer.hpp"

#include <cstdint>
#include <stdexcept>
#include <cassert>
#include <cstring>

#include "VulkanDebug.hpp"
#include "utils/VulkanCommandBuffer.hpp"
#include "utils/VulkanMemory.hpp"
#include "utils/VulkanImageUtils.hpp"
#include "interfaces/AVulkanImageBufferPrivate.hpp"

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
    _swapChain.init(_vkInstance, winW, winH, options.vsync);
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
    _capturedFrame.allocate(_vkInstance.devices,
                            _swapChain.swapChainExtent.width,
                            _swapChain.swapChainExtent.height,
                            4,
                            false);
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
VulkanRenderer::resize(uint32_t winW, uint32_t winH, bool vsync)
{
    vkDeviceWaitIdle(_vkInstance.devices.device);
    if (winW <= 0 || winH <= 0) {
        return;
    }

    _swapChain.resize(winW, winH, vsync);
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
    _capturedFrame.clear();
    _capturedFrame.allocate(_vkInstance.devices,
                            _swapChain.swapChainExtent.width,
                            _swapChain.swapChainExtent.height,
                            4,
                            false);
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
    _capturedFrame.clear();
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

    uint32_t imgIndex;
    auto result =
      vkAcquireNextImageKHR(_vkInstance.devices.device,
                            _swapChain.swapChain,
                            UINT64_MAX,
                            _sync.imageAvailableSem[_sync.currentFrame],
                            VK_NULL_HANDLE,
                            &imgIndex);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        return;
    }

    if (_sync.imgsInflightFence[imgIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(_vkInstance.devices.device,
                        1,
                        &_sync.imgsInflightFence[imgIndex],
                        VK_TRUE,
                        UINT64_MAX);
    }
    _sync.imgsInflightFence[imgIndex] = _sync.inflightFence[_sync.currentFrame];

    recordRenderCmd(imgIndex, clearColor);
    emitDrawCmds(imgIndex);

    VkSwapchainKHR swapChains[] = { _swapChain.swapChain };
    VkSemaphore presentWaitSems[] = {
        _sync.allRenderFinishedSem[_sync.currentFrame],
    };
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = presentWaitSems;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imgIndex;
    presentInfo.pResults = nullptr;
    vkQueuePresentKHR(_vkInstance.queues.presentQueue, &presentInfo);
    if (saveNextFrame) {
        copyFrameToHostMemory(_sync.currentFrame);
        saveNextFrame = false;
    }
    _sync.currentFrame =
      (_sync.currentFrame + 1) % VulkanSync::MAX_FRAME_INFLIGHT;
}

// Screenshot related
VulkanScreenshot
VulkanRenderer::generateScreenshot() const
{
    VulkanScreenshot screenshot{};

    screenshot.width = _capturedFrame.width;
    screenshot.height = _capturedFrame.height;
    screenshot.nbChannel = _capturedFrame.nbChannel;
    screenshot.data.reset(
      new uint8_t[_capturedFrame.width * _capturedFrame.height *
                  _capturedFrame.nbChannel]);

    void *data;
    vkMapMemory(_vkInstance.devices.device,
                _capturedFrame.stagingBuffer.memory,
                0,
                _capturedFrame.stagingBuffer.size,
                0,
                &data);
    memcpy(screenshot.data.get(), data, _capturedFrame.stagingBuffer.size);
    vkUnmapMemory(_vkInstance.devices.device,
                  _capturedFrame.stagingBuffer.memory);
    return (screenshot);
}

// Cmd buffer related
void
VulkanRenderer::emitDrawCmds(uint32_t imgIndex)
{
    // Send scene rendering
    VkSemaphore waitSceneSigSems[] = {
        _sync.imageAvailableSem[_sync.currentFrame],
    };
    VkSemaphore finishSceneSigSems[] = {
        _sync.allRenderFinishedSem[_sync.currentFrame],
    };
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pWaitSemaphores = waitSceneSigSems;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = finishSceneSigSems;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pCommandBuffers = &_renderCommandBuffers[imgIndex];
    submitInfo.commandBufferCount = 1;
    vkResetFences(
      _vkInstance.devices.device, 1, &_sync.inflightFence[_sync.currentFrame]);
    if (vkQueueSubmit(_vkInstance.queues.graphicQueue,
                      1,
                      &submitInfo,
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
        mandelbrotConstants.fbW = _imageDisplayed.colorTex.width;
        mandelbrotConstants.fbH = _imageDisplayed.colorTex.height;
        recordMandelbrotRenderCmd(imgIndex, cmdClearColor);
        mandelbrotComputeDone = true;
    }
    _imageDisplayed.copyColorDepthTexturesContent(
      _imageMandelbrot,
      _renderCommandBuffers[imgIndex],
      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      VK_IMAGE_LAYOUT_UNDEFINED,
      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    recordUiRenderCmd(imgIndex, cmdClearColor);
    recordToScreenRenderCmd(imgIndex, cmdClearColor);

    if (vkEndCommandBuffer(_renderCommandBuffers[imgIndex]) != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanRenderer: Failed to record render command Buffer");
    }
}

// Sub-functions for recordRenderCmd
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

void
VulkanRenderer::copyFrameToHostMemory(size_t imgIndex)
{
    vkWaitForFences(_vkInstance.devices.device,
                    1,
                    &_sync.inflightFence[imgIndex],
                    VK_TRUE,
                    UINT64_MAX);
    _imageMandelbrot.colorTex.loadTextureOnCPU(
      _vkInstance.cmdPools, _vkInstance.queues, _capturedFrame);
}
