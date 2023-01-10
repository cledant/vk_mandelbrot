#ifndef VK_MANDELBROT_VULKANRENDERER_HPP
#define VK_MANDELBROT_VULKANRENDERER_HPP

#include <vulkan/vulkan.h>

#include "VulkanSwapChain.hpp"
#include "VulkanSync.hpp"

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
    VkClearColorValue clearColor = DEFAULT_CLEAR_COLOR;
    bool mandelbrotComputeDone{};
    bool saveNextFrame{};

    void init(VkSurfaceKHR surface,
              VulkanInstanceOptions const &options,
              uint32_t winW,
              uint32_t winH);
    void resize(uint32_t winW, uint32_t winH, float rendererScale, bool vsync);
    void clear();

    // Render related
    void draw();

    // Screenshot related
    [[nodiscard]] VulkanScreenshot generateScreenshot() const;

  private:
    static constexpr int32_t const CHUNK_WIDTH = 320;
    static constexpr int32_t const CHUNK_HEIGHT = 180;

    // Vulkan related
    VulkanSwapChain _swapChain;
    VulkanSync _sync;

    // Cmd Buffers
    std::vector<VkCommandBuffer> _renderCommandBuffers;

    // Cmd buffer related
    inline void recordRenderCmd(VkCommandBuffer cmdBuffer,
                                uint32_t imgIndex,
                                VkClearColorValue const &clearColor);
    inline void emitDrawCmds(VkCommandBuffer cmdBuffer);

    // Sub-functions for recordRenderCmd
    inline void recordMandelbrotFirstRenderCmd(
      VkCommandBuffer cmdBuffer,
      VkClearColorValue const &cmdClearColor);
    inline void recordMandelbrotMultipleRenderCmd(
      VkCommandBuffer cmdBuffer,
      VkClearColorValue const &cmdClearColor);
    inline void recordUiRenderCmd(VkCommandBuffer cmdBuffer,
                                  uint32_t imgIndex,
                                  VkClearColorValue const &cmdClearColor);
    inline void recordToScreenRenderCmd(VkCommandBuffer cmdBuffer,
                                        uint32_t imgIndex,
                                        VkClearColorValue const &cmdClearColor);

    // Screenshot related fct
    inline void copyFrameToHostMemory();
};

#endif // VK_MANDELBROT_VULKANRENDERER_HPP
