#ifndef VK_MANDELBROT_VULKANDEFAULTOFFSCREENRENDERPASS_HPP
#define VK_MANDELBROT_VULKANDEFAULTOFFSCREENRENDERPASS_HPP

#include <vulkan/vulkan.h>

#include "interface/AVulkanOffscreenRenderPass.hpp"

class VulkanDefaultOffscreenRenderPass final
  : public AVulkanOffscreenRenderPass<VulkanDefaultOffscreenRenderPass>
{
  public:
    VulkanDefaultOffscreenRenderPass() = default;
    ~VulkanDefaultOffscreenRenderPass() override = default;
    VulkanDefaultOffscreenRenderPass(
      VulkanDefaultOffscreenRenderPass const &src) = default;
    VulkanDefaultOffscreenRenderPass &operator=(
      VulkanDefaultOffscreenRenderPass const &rhs) = default;
    VulkanDefaultOffscreenRenderPass(
      VulkanDefaultOffscreenRenderPass &&src) noexcept =
      default;
    VulkanDefaultOffscreenRenderPass &operator=(
      VulkanDefaultOffscreenRenderPass &&rhs) noexcept = default;

    void implInit(VkFormat colorImageFormat, int32_t imgW, int32_t imgH);
    void implResize(VkFormat colorImageFormat, int32_t imgW, int32_t imgH);
    void implClean();
    void implClear();
};

#endif // VK_MANDELBROT_VULKANDEFAULTOFFSCREENRENDERPASS_HPP
