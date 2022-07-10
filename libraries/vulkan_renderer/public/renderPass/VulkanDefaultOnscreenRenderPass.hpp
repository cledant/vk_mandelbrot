#ifndef VK_MANDELBROT_VULKANDEFAULTONSCREENRENDERPASS_HPP
#define VK_MANDELBROT_VULKANDEFAULTONSCREENRENDERPASS_HPP

#include <vulkan/vulkan.h>

#include "interface/AVulkanRenderPass.hpp"

class VulkanDefaultOnscreenRenderPass final
  : public AVulkanRenderPass<VulkanDefaultOnscreenRenderPass>
{
  public:
    VulkanDefaultOnscreenRenderPass() = default;
    ~VulkanDefaultOnscreenRenderPass() override = default;
    VulkanDefaultOnscreenRenderPass(
      VulkanDefaultOnscreenRenderPass const &src) = default;
    VulkanDefaultOnscreenRenderPass &operator=(
      VulkanDefaultOnscreenRenderPass const &rhs) =
      default;
    VulkanDefaultOnscreenRenderPass(
      VulkanDefaultOnscreenRenderPass &&src) noexcept = default;
    VulkanDefaultOnscreenRenderPass &operator=(
      VulkanDefaultOnscreenRenderPass &&rhs) noexcept = default;

    void implInit(VulkanInstance const &vkInstance,
                  VulkanSwapChain const &swapChain);
    void implResize(VulkanSwapChain const &swapChain);
    void implClean();
    void implClear();
};

#endif // VK_MANDELBROT_VULKANDEFAULTONSCREENRENDERPASS_HPP
