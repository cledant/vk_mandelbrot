#ifndef VK_MANDELBROT_VULKANTOSCREENRENDERPASS_HPP
#define VK_MANDELBROT_VULKANTOSCREENRENDERPASS_HPP

#include <vulkan/vulkan.h>

#include "interface/AVulkanRenderPass.hpp"

class VulkanToScreenRenderPass final
  : public AVulkanRenderPass<VulkanToScreenRenderPass>
{
  public:
    VulkanToScreenRenderPass() = default;
    ~VulkanToScreenRenderPass() override = default;
    VulkanToScreenRenderPass(VulkanToScreenRenderPass const &src) = default;
    VulkanToScreenRenderPass &operator=(VulkanToScreenRenderPass const &rhs) =
      default;
    VulkanToScreenRenderPass(VulkanToScreenRenderPass &&src) noexcept = default;
    VulkanToScreenRenderPass &operator=(
      VulkanToScreenRenderPass &&rhs) noexcept = default;

    void implInit(VulkanInstance const &vkInstance,
                  VulkanSwapChain const &swapChain);
    void implResize(VulkanSwapChain const &swapChain);
    void implClean();
    void implClear();
};

#endif // VK_MANDELBROT_VULKANTOSCREENRENDERPASS_HPP
