#ifndef VK_MANDELBROT_VULKANUIONSCREENRENDERPASS_HPP
#define VK_MANDELBROT_VULKANUIONSCREENRENDERPASS_HPP

#include <vulkan/vulkan.h>

#include "interfaces/AVulkanOnscreenRenderPass.hpp"

class VulkanUiOnscreenRenderPass final
  : public AVulkanOnscreenRenderPass<VulkanUiOnscreenRenderPass>
{
  public:
    VulkanUiOnscreenRenderPass() = default;
    ~VulkanUiOnscreenRenderPass() override = default;
    VulkanUiOnscreenRenderPass(VulkanUiOnscreenRenderPass const &src) = default;
    VulkanUiOnscreenRenderPass &operator=(
      VulkanUiOnscreenRenderPass const &rhs) = default;
    VulkanUiOnscreenRenderPass(VulkanUiOnscreenRenderPass &&src) noexcept =
      default;
    VulkanUiOnscreenRenderPass &operator=(
      VulkanUiOnscreenRenderPass &&rhs) noexcept = default;

    void implInit(VulkanInstance const &vkInstance,
                  std::vector<VulkanTexture> const &swapChainImageViews,
                  VkFormat swapChainImageFormat,
                  int32_t swapChainImgW,
                  int32_t swapChainImgH);
    void implResize(std::vector<VulkanTexture> const &swapChainImageViews,
                    VkFormat swapChainImageFormat,
                    int32_t swapChainImgW,
                    int32_t swapChainImgH);
    void implClean();
    void implClear();
};

#endif // VK_MANDELBROT_VULKANUIONSCREENRENDERPASS_HPP
