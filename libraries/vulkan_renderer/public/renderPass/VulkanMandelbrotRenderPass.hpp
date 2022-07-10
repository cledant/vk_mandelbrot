#ifndef VK_MANDELBROT_VULKANMANDELBROTRENDERPASS_HPP
#define VK_MANDELBROT_VULKANMANDELBROTRENDERPASS_HPP

#include <vulkan/vulkan.h>

#include "interface/AVulkanOffscreenRenderPass.hpp"

class VulkanMandelbrotRenderPass final
  : public AVulkanOffscreenRenderPass<VulkanMandelbrotRenderPass>
{
  public:
    VulkanMandelbrotRenderPass() = default;
    ~VulkanMandelbrotRenderPass() override = default;
    VulkanMandelbrotRenderPass(VulkanMandelbrotRenderPass const &src) = default;
    VulkanMandelbrotRenderPass &operator=(
      VulkanMandelbrotRenderPass const &rhs) = default;
    VulkanMandelbrotRenderPass(VulkanMandelbrotRenderPass &&src) noexcept =
      default;
    VulkanMandelbrotRenderPass &operator=(
      VulkanMandelbrotRenderPass &&rhs) noexcept = default;

    void implInit(VkFormat colorImageFormat, int32_t imgW, int32_t imgH);
    void implResize(VkFormat colorImageFormat, int32_t imgW, int32_t imgH);
    void implClean();
    void implClear();
};

#endif // VK_MANDELBROT_VULKANMANDELBROTRENDERPASS_HPP
