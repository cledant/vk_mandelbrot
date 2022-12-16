#ifndef VK_MANDELBROT_VULKANMULTIPLEOFFSCREENRENDERPASS_HPP
#define VK_MANDELBROT_VULKANMULTIPLEOFFSCREENRENDERPASS_HPP

#include <vulkan/vulkan.h>

#include "interfaces/AVulkanOffscreenRenderPass.hpp"

class VulkanMultipleOffscreenRenderPass final
  : public AVulkanOffscreenRenderPass<VulkanMultipleOffscreenRenderPass>
{
  public:
    VulkanMultipleOffscreenRenderPass() = default;
    ~VulkanMultipleOffscreenRenderPass() override = default;
    VulkanMultipleOffscreenRenderPass(
      VulkanMultipleOffscreenRenderPass const &src) = default;
    VulkanMultipleOffscreenRenderPass &operator=(
      VulkanMultipleOffscreenRenderPass const &rhs) = default;
    VulkanMultipleOffscreenRenderPass(
      VulkanMultipleOffscreenRenderPass &&src) noexcept = default;
    VulkanMultipleOffscreenRenderPass &operator=(
      VulkanMultipleOffscreenRenderPass &&rhs) noexcept = default;

    void implInit(VkFormat colorImageFormat,
                  VkFormat depthImageFormat,
                  VkImageView colorTexImgView,
                  VkImageView depthTexImgView,
                  int32_t imgW,
                  int32_t imgH);
    void implResize(VkFormat colorImageFormat,
                    VkFormat depthImageFormat,
                    VkImageView colorTexImgView,
                    VkImageView depthTexImgView,
                    int32_t imgW,
                    int32_t imgH);
    void implClean();
    void implClear();
};

#endif // VK_MANDELBROT_VULKANMULTIPLEOFFSCREENRENDERPASS_HPP
