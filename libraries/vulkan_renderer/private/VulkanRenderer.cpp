#include "VulkanRenderer.hpp"

#include <cstdint>
#include <stdexcept>

#include "utils/VulkanCommandBuffer.hpp"
#include "utils/VulkanMemory.hpp"
#include "utils/VulkanImageUtils.hpp"

void
VulkanRenderer::init(VulkanInstance const &vkInstance,
                     uint32_t winW,
                     uint32_t winH,
                     bool vsync)
{
    _devices = vkInstance.devices;
    _queues = vkInstance.queues;
    _cmdPools = vkInstance.cmdPools;

    swapChain.init(vkInstance, winW, winH, vsync);
    _sync.init(vkInstance);

    allocateCommandBuffers(_devices.device,
                           _cmdPools.renderCommandPool,
                           _renderCommandBuffers,
                           VulkanSync::MAX_FRAME_INFLIGHT);
}

void
VulkanRenderer::resize(uint32_t winW, uint32_t winH, bool vsync)
{
    vkDeviceWaitIdle(_devices.device);
    if (winW <= 0 || winH <= 0) {
        return;
    }

    for (auto &it : _renderCommandBuffers) {
        vkResetCommandBuffer(it, 0);
    }
    swapChain.resize(winW, winH, vsync);
}

void
VulkanRenderer::clear()
{
    vkDeviceWaitIdle(_devices.device);
    _sync.clear();
    swapChain.clear();
    _devices = {};
    _cmdPools = {};
}

void
VulkanRenderer::acquireImage(VkCommandBuffer &cmdBuffer, uint32_t &imgIndex)
{
    {
        auto waitResult =
          vkWaitForFences(_devices.device,
                          1,
                          &_sync.inflightFence[_sync.currentFrame],
                          VK_TRUE,
                          UINT64_MAX);
        if (waitResult == VK_ERROR_DEVICE_LOST) {
            throw std::runtime_error("VulkanRenderer: Device Lost");
        }
    }

    {
        auto acquireResult =
          vkAcquireNextImageKHR(_devices.device,
                                swapChain.swapChain,
                                UINT64_MAX,
                                _sync.imageAvailableSem[_sync.currentFrame],
                                VK_NULL_HANDLE,
                                &imgIndex);
        if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR) {
            return;
        }
    }

    vkResetFences(_devices.device, 1, &_sync.inflightFence[_sync.currentFrame]);
    cmdBuffer = _renderCommandBuffers[_sync.currentFrame];
}

void
VulkanRenderer::presentImage(uint32_t imgIndex,
                             VulkanDefaultImageTexture &framebuffer,
                             VulkanTextureStaging &capturedFrame)
{
    emitDrawCmds(_renderCommandBuffers[_sync.currentFrame]);

    VkSwapchainKHR swapChains[] = { swapChain.swapChain };
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
    vkQueuePresentKHR(_queues.presentQueue, &presentInfo);
    if (saveNextFrame) {
        copyFrameToHostMemory(framebuffer, capturedFrame);
        saveNextFrame = false;
    }
    _sync.currentFrame =
      (_sync.currentFrame + 1) % VulkanSync::MAX_FRAME_INFLIGHT;
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
    auto result = vkQueueSubmit(_queues.graphicQueue,
                                1,
                                &submitInfo,
                                _sync.inflightFence[_sync.currentFrame]);
    if (result != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanRenderer: Failed to submit render draw command buffer");
    }
}

void
VulkanRenderer::copyFrameToHostMemory(
  VulkanDefaultImageTexture const &framebuffer,
  VulkanTextureStaging &capturedFrame)
{
    vkWaitForFences(_devices.device,
                    1,
                    &_sync.inflightFence[_sync.currentFrame],
                    VK_TRUE,
                    UINT64_MAX);
    framebuffer.colorTex.loadTextureOnHost(_cmdPools, _queues, capturedFrame);
}
