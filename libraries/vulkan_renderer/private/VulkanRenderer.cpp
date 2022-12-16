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

    // Vulkan related
    _vkInstance.init(surface, options);
    _swapChain.init(_vkInstance, winW, winH, options.vsync);
    _sync.init(_vkInstance);

    // Textures
    int32_t rendererW =
      _swapChain.swapChainExtent.width * options.rendererScale;
    int32_t rendererH =
      _swapChain.swapChainExtent.height * options.rendererScale;
    _imageMandelbrot.init(_vkInstance,
                          VK_FORMAT_R8G8B8A8_UNORM,
                          _vkInstance.depthFormat,
                          rendererW,
                          rendererH);
    _capturedFrame.allocate(
      _vkInstance.devices, rendererW, rendererH, 4, false);

    // Render Passes + pipelines
    _toScreenRenderPass.init(_vkInstance, _swapChain);
    _toScreen.init(_vkInstance,
                   _swapChain,
                   _toScreenRenderPass.renderPass,
                   _imageMandelbrot.descriptorImage);

    _mandelbrotFirstRenderPass.init(_vkInstance,
                                    VK_FORMAT_R8G8B8A8_UNORM,
                                    _vkInstance.depthFormat,
                                    _imageMandelbrot.colorTex.textureImgView,
                                    _imageMandelbrot.depthTex.textureImgView,
                                    rendererW,
                                    rendererH);
    _mandelbrotFirst.init(
      _vkInstance, _mandelbrotFirstRenderPass.renderPass, rendererW, rendererH);
    /*    _mandelbrotMultipleRenderPass.init(_vkInstance,
                                           VK_FORMAT_R8G8B8A8_UNORM,
                                           _vkInstance.depthFormat,
                                           _imageMandelbrot.colorTex.textureImgView,
                                           _imageMandelbrot.depthTex.textureImgView,
                                           rendererW,
                                           rendererH);
        _mandelbrotMultiple.init(
          _vkInstance, _mandelbrotMultipleRenderPass, rendererW, rendererH);*/

    _uiRenderPass.init(_vkInstance, _swapChain);
    _ui.init(
      _vkInstance, _uiRenderPass.renderPass, _swapChain.currentSwapChainNbImg);

    allocateCommandBuffers(_vkInstance.devices.device,
                           _vkInstance.cmdPools.renderCommandPool,
                           _renderCommandBuffers,
                           VulkanSync::MAX_FRAME_INFLIGHT);
}

void
VulkanRenderer::resize(uint32_t winW,
                       uint32_t winH,
                       float rendererScale,
                       bool vsync)
{
    vkDeviceWaitIdle(_vkInstance.devices.device);
    if (winW <= 0 || winH <= 0 || rendererScale <= 0.0f) {
        return;
    }

    // Vulkan
    for (auto &it : _renderCommandBuffers) {
        vkResetCommandBuffer(it, 0);
    }
    _swapChain.resize(winW, winH, vsync);

    // Textures
    int32_t rendererW = _swapChain.swapChainExtent.width * rendererScale;
    int32_t rendererH = _swapChain.swapChainExtent.height * rendererScale;
    _imageMandelbrot.resize(
      VK_FORMAT_R8G8B8A8_UNORM, _vkInstance.depthFormat, rendererW, rendererH);
    _capturedFrame.clear();
    _capturedFrame.allocate(
      _vkInstance.devices, rendererW, rendererH, 4, false);

    // Render passes + pipelines
    _toScreenRenderPass.resize(_swapChain);
    _toScreen.resize(_swapChain,
                     _toScreenRenderPass.renderPass,
                     _imageMandelbrot.descriptorImage);

    _mandelbrotFirstRenderPass.resize(VK_FORMAT_R8G8B8A8_UNORM,
                                      _vkInstance.depthFormat,
                                      _imageMandelbrot.colorTex.textureImgView,
                                      _imageMandelbrot.depthTex.textureImgView,
                                      rendererW,
                                      rendererH);
    _mandelbrotFirst.resize(
      _mandelbrotFirstRenderPass.renderPass, rendererW, rendererH);

    _uiRenderPass.resize(_swapChain);
    _ui.resize(_uiRenderPass.renderPass, _swapChain.currentSwapChainNbImg);
}

void
VulkanRenderer::clear()
{
    vkDeviceWaitIdle(_vkInstance.devices.device);
    _ui.clear();
    _uiRenderPass.clear();
    _mandelbrotFirst.clear();
    _mandelbrotFirstRenderPass.clear();
    _toScreen.clear();
    _toScreenRenderPass.clear();
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
    {
        auto waitResult =
          vkWaitForFences(_vkInstance.devices.device,
                          1,
                          &_sync.inflightFence[_sync.currentFrame],
                          VK_TRUE,
                          UINT64_MAX);
        if (waitResult == VK_ERROR_DEVICE_LOST) {
            throw std::runtime_error("VulkanRenderer: Device Lost");
        }
    }

    uint32_t imgIndex;
    {
        auto acquireResult =
          vkAcquireNextImageKHR(_vkInstance.devices.device,
                                _swapChain.swapChain,
                                UINT64_MAX,
                                _sync.imageAvailableSem[_sync.currentFrame],
                                VK_NULL_HANDLE,
                                &imgIndex);
        if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR) {
            return;
        }
    }

    vkResetFences(
      _vkInstance.devices.device, 1, &_sync.inflightFence[_sync.currentFrame]);
    recordRenderCmd(
      _renderCommandBuffers[_sync.currentFrame], imgIndex, clearColor);
    emitDrawCmds(_renderCommandBuffers[_sync.currentFrame]);

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
        copyFrameToHostMemory();
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
VulkanRenderer::recordRenderCmd(VkCommandBuffer cmdBuffer,
                                uint32_t imgIndex,
                                VkClearColorValue const &cmdClearColor)
{
    vkResetCommandBuffer(cmdBuffer, 0);

    VkCommandBufferBeginInfo cb_begin_info{};
    cb_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cb_begin_info.flags = 0;
    cb_begin_info.pInheritanceInfo = nullptr;
    if (vkBeginCommandBuffer(cmdBuffer, &cb_begin_info) != VK_SUCCESS) {
        throw std::runtime_error("VulkanRenderer: Failed to begin "
                                 "recording render command buffer");
    }

    if (!mandelbrotComputeDone) {
        // Update push constant values
        mandelbrotConstants.fbW = _imageMandelbrot.colorTex.width;
        mandelbrotConstants.fbH = _imageMandelbrot.colorTex.height;
        recordMandelbrotRenderCmd(cmdBuffer, cmdClearColor);
        mandelbrotComputeDone = true;
    }
    recordToScreenRenderCmd(cmdBuffer, imgIndex, cmdClearColor);
    recordUiRenderCmd(cmdBuffer, imgIndex, cmdClearColor);

    if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanRenderer: Failed to record render command Buffer");
    }
}

void
VulkanRenderer::emitDrawCmds(VkCommandBuffer cmdBuffer)
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

    submitInfo.pCommandBuffers = &cmdBuffer;
    submitInfo.commandBufferCount = 1;
    auto result = vkQueueSubmit(_vkInstance.queues.graphicQueue,
                                1,
                                &submitInfo,
                                _sync.inflightFence[_sync.currentFrame]);
    if (result != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanRenderer: Failed to submit render draw command buffer");
    }
}

// Sub-functions for recordRenderCmd
void
VulkanRenderer::recordMandelbrotRenderCmd(
  VkCommandBuffer cmdBuffer,
  VkClearColorValue const &cmdClearColor)
{
    // Begin Mandelbrot renderpass
    std::array<VkClearValue, 2> clear_vals{};
    clear_vals[0].color = cmdClearColor;
    clear_vals[1].depthStencil = DEFAULT_CLEAR_DEPTH_STENCIL;
    VkRenderPassBeginInfo rp_begin_info{};
    rp_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin_info.renderPass = _mandelbrotFirstRenderPass.renderPass;
    rp_begin_info.framebuffer = _mandelbrotFirstRenderPass.framebuffer;
    rp_begin_info.renderArea.offset = { 0, 0 };
    rp_begin_info.renderArea.extent = {
        static_cast<uint32_t>(_imageMandelbrot.colorTex.width),
        static_cast<uint32_t>(_imageMandelbrot.colorTex.height),
    };
    rp_begin_info.clearValueCount = clear_vals.size();
    rp_begin_info.pClearValues = clear_vals.data();

    vkCmdBeginRenderPass(cmdBuffer, &rp_begin_info, VK_SUBPASS_CONTENTS_INLINE);
    _mandelbrotFirst.generateCommands(cmdBuffer, mandelbrotConstants);
    vkCmdEndRenderPass(cmdBuffer);
}

void
VulkanRenderer::recordUiRenderCmd(VkCommandBuffer cmdBuffer,
                                  uint32_t imgIndex,
                                  VkClearColorValue const &cmdClearColor)
{
    // Begin Ui renderpass
    std::array<VkClearValue, 2> clear_vals{};
    clear_vals[0].color = cmdClearColor;
    clear_vals[1].depthStencil = DEFAULT_CLEAR_DEPTH_STENCIL;
    VkRenderPassBeginInfo rp_begin_info{};
    rp_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin_info.renderPass = _uiRenderPass.renderPass;
    rp_begin_info.framebuffer = _uiRenderPass.framebuffers[imgIndex];
    rp_begin_info.renderArea.offset = { 0, 0 };
    rp_begin_info.renderArea.extent = _swapChain.swapChainExtent;
    rp_begin_info.clearValueCount = clear_vals.size();
    rp_begin_info.pClearValues = clear_vals.data();

    vkCmdBeginRenderPass(cmdBuffer, &rp_begin_info, VK_SUBPASS_CONTENTS_INLINE);
    _ui.generateCommands(cmdBuffer);
    vkCmdEndRenderPass(cmdBuffer);
}

void
VulkanRenderer::recordToScreenRenderCmd(VkCommandBuffer cmdBuffer,
                                        uint32_t imgIndex,
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

    vkCmdBeginRenderPass(cmdBuffer, &rp_begin_info, VK_SUBPASS_CONTENTS_INLINE);
    _toScreen.generateCommands(cmdBuffer, imgIndex);
    vkCmdEndRenderPass(cmdBuffer);
}

void
VulkanRenderer::copyFrameToHostMemory()
{
    vkWaitForFences(_vkInstance.devices.device,
                    1,
                    &_sync.inflightFence[_sync.currentFrame],
                    VK_TRUE,
                    UINT64_MAX);
    _imageMandelbrot.colorTex.loadTextureOnCPU(
      _vkInstance.cmdPools, _vkInstance.queues, _capturedFrame);
}
