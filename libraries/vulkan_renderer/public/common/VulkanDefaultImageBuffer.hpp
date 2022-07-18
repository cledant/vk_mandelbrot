#ifndef VK_MANDELBROT_VULKANDEFAULTIMAGEBUFFER_HPP
#define VK_MANDELBROT_VULKANDEFAULTIMAGEBUFFER_HPP

#include <vulkan/vulkan.h>

#include "interface/AVulkanImageBuffer.hpp"

class VulkanDefaultImageBuffer final
  : public AVulkanImageBuffer<VulkanDefaultImageBuffer>
{
  public:
    VulkanDefaultImageBuffer() = default;
    ~VulkanDefaultImageBuffer() override = default;
    VulkanDefaultImageBuffer(VulkanDefaultImageBuffer const &src) = default;
    VulkanDefaultImageBuffer &operator=(VulkanDefaultImageBuffer const &rhs) =
      default;
    VulkanDefaultImageBuffer(VulkanDefaultImageBuffer &&src) noexcept = default;
    VulkanDefaultImageBuffer &operator=(
      VulkanDefaultImageBuffer &&rhs) noexcept = default;

    void implInit(VkFormat colorImageFormat,
                  VkFormat depthImageFormat,
                  int32_t imgW,
                  int32_t imgH);
    void implResize(VkFormat colorImageFormat,
                    VkFormat depthImageFormat,
                    int32_t imgW,
                    int32_t imgH);
    void implClean();
    void implClear();
};

#endif // VK_MANDELBROT_VULKANDEFAULTIMAGEBUFFER_HPP
