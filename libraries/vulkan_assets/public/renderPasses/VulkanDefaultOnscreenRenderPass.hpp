#ifndef VK_MANDELBROT_VULKANDEFAULTONSCREENRENDERPASS_HPP
#define VK_MANDELBROT_VULKANDEFAULTONSCREENRENDERPASS_HPP

#include <vulkan/vulkan.h>

#include "interfaces/AVulkanOnscreenRenderPass.hpp"

class VulkanDefaultOnscreenRenderPass final
  : public AVulkanOnscreenRenderPass<VulkanDefaultOnscreenRenderPass>
{
  public:
    VulkanDefaultOnscreenRenderPass() = default;
    ~VulkanDefaultOnscreenRenderPass() override = default;
    VulkanDefaultOnscreenRenderPass(
      VulkanDefaultOnscreenRenderPass const &src) = default;
    VulkanDefaultOnscreenRenderPass &operator=(
      VulkanDefaultOnscreenRenderPass const &rhs) = default;
    VulkanDefaultOnscreenRenderPass(
      VulkanDefaultOnscreenRenderPass &&src) noexcept = default;
    VulkanDefaultOnscreenRenderPass &operator=(
      VulkanDefaultOnscreenRenderPass &&rhs) noexcept = default;

    void implInit(VulkanInstance const &vkInstance,
                  std::vector<VulkanTexture> const &swapChainImageViews,
                  VkFormat swapChainImageFormat);
    void implResize(std::vector<VulkanTexture> const &swapChainImageViews,
                    VkFormat swapChainImageFormat);
    void implClean();
    void implClear();
};

#endif // VK_MANDELBROT_VULKANDEFAULTONSCREENRENDERPASS_HPP
