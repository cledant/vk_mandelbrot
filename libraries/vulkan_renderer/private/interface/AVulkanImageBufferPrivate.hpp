#ifndef VK_MANDELBROT_VULKAN_AVULKANIMAGEBUFFERPRIVATE_HPP
#define VK_MANDELBROT_VULKAN_AVULKANIMAGEBUFFERPRIVATE_HPP

#include <vector>
#include <array>
#include <stdexcept>

#include <vulkan/vulkan.h>

#include "utils/VulkanImageUtils.hpp"
#include "utils/VulkanTextureUtils.hpp"
#include "utils/VulkanFrameBufferUtils.hpp"

#include "interface/AVulkanImageBuffer.hpp"

template<class Child>
void
AVulkanImageBuffer<Child>::defaultCreateColorResources(
  VkFormat colorImageFormat,
  int32_t imgW,
  int32_t imgH)
{
    colorTex.createColorTexture(_devices, imgW, imgH, colorImageFormat);
}

template<class Child>
void
AVulkanImageBuffer<Child>::defaultCreateDepthResources(VkFormat depthFormat,
                                                       int32_t imgW,
                                                       int32_t imgH)
{
    depthTex.createDepthTexture(
      _devices, _cmdPools, _queues, imgW, imgH, depthFormat);
}

#endif // VK_MANDELBROT_VULKAN_AVULKANIMAGEBUFFERPRIVATE_HPP