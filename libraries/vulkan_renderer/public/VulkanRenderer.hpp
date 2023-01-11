#ifndef VK_MANDELBROT_VULKANRENDERER_HPP
#define VK_MANDELBROT_VULKANRENDERER_HPP

#include <vulkan/vulkan.h>

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanSync.hpp"
#include "imageTextures/VulkanDefaultImageTexture.hpp"

class VulkanRenderer final
{
  public:
    VulkanRenderer() = default;
    ~VulkanRenderer() = default;
    VulkanRenderer(VulkanRenderer const &src) = delete;
    VulkanRenderer &operator=(VulkanRenderer const &rhs) = delete;
    VulkanRenderer(VulkanRenderer &&src) = delete;
    VulkanRenderer &operator=(VulkanRenderer &&rhs) = delete;

    // Default Values
    static constexpr VkClearColorValue const DEFAULT_CLEAR_COLOR = {
        { 0.0f, 0.0f, 0.0f, 1.0f }
    };
    static constexpr VkClearDepthStencilValue const
      DEFAULT_CLEAR_DEPTH_STENCIL = { 1.0f, 0 };

    // Public values
    bool saveNextFrame{};
    VulkanSwapChain swapChain;

    void init(VulkanInstance const &vkInstance,
              uint32_t winW,
              uint32_t winH,
              bool vsync);
    void resize(uint32_t winW, uint32_t winH, bool vsync);
    void clear();

    // Render related
    void acquireImage(VkCommandBuffer &cmdBuffer, uint32_t &imgIndex);
    void presentImage(uint32_t imgIndex,
                      VulkanDefaultImageTexture &framebuffer,
                      VulkanTextureStaging &capturedFrame);

  private:
    // Vulkan related
    VulkanDevices _devices;
    VulkanQueues _queues;
    VulkanCommandPools _cmdPools;

    VulkanSync _sync;
    std::vector<VkCommandBuffer> _renderCommandBuffers;

    inline void emitDrawCmds(VkCommandBuffer cmdBuffer);
    inline void copyFrameToHostMemory(
      VulkanDefaultImageTexture const &framebuffer,
      VulkanTextureStaging &capturedFrame);
};

#endif // VK_MANDELBROT_VULKANRENDERER_HPP
