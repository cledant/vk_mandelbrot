#ifndef VK_MANDELBROT_VULKANUIOFFSCREENRENDERPASS_HPP
#define VK_MANDELBROT_VULKANUIOFFSCREENRENDERPASS_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "interfaces/AVulkanOffscreenRenderPass.hpp"

class VulkanUiOffscreenRenderPass final
  : public AVulkanOffscreenRenderPass<VulkanUiOffscreenRenderPass>
{
  public:
    VulkanUiOffscreenRenderPass() = default;
    ~VulkanUiOffscreenRenderPass() override = default;
    VulkanUiOffscreenRenderPass(VulkanUiOffscreenRenderPass const &src) =
      default;
    VulkanUiOffscreenRenderPass &operator=(
      VulkanUiOffscreenRenderPass const &rhs) = default;
    VulkanUiOffscreenRenderPass(VulkanUiOffscreenRenderPass &&src) noexcept =
      default;
    VulkanUiOffscreenRenderPass &operator=(
      VulkanUiOffscreenRenderPass &&rhs) noexcept = default;

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

#endif // VK_MANDELBROT_VULKANUIOFFSCREENRENDERPASS_HPP
