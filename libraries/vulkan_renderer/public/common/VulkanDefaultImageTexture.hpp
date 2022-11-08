#ifndef VK_MANDELBROT_VULKANDEFAULTIMAGETEXTURE_HPP
#define VK_MANDELBROT_VULKANDEFAULTIMAGETEXTURE_HPP

#include <vulkan/vulkan.h>

#include "interfaces/AVulkanImageTexture.hpp"

class VulkanDefaultImageTexture final
  : public AVulkanImageTexture<VulkanDefaultImageTexture>
{
  public:
    VulkanDefaultImageTexture() = default;
    ~VulkanDefaultImageTexture() override = default;
    VulkanDefaultImageTexture(VulkanDefaultImageTexture const &src) = default;
    VulkanDefaultImageTexture &operator=(VulkanDefaultImageTexture const &rhs) =
      default;
    VulkanDefaultImageTexture(VulkanDefaultImageTexture &&src) noexcept =
      default;
    VulkanDefaultImageTexture &operator=(
      VulkanDefaultImageTexture &&rhs) noexcept = default;

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

#endif // VK_MANDELBROT_VULKANDEFAULTIMAGETEXTURE_HPP
