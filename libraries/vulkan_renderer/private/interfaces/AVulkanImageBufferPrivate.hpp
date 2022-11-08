#ifndef VK_MANDELBROT_AVULKANIMAGEBUFFERPRIVATE_HPP
#define VK_MANDELBROT_AVULKANIMAGEBUFFERPRIVATE_HPP

#include <vector>
#include <array>
#include <stdexcept>

#include <vulkan/vulkan.h>

#include "utils/VulkanImageUtils.hpp"
#include "utils/VulkanTextureUtils.hpp"
#include "utils/VulkanFrameBufferUtils.hpp"
#include "utils/VulkanTransitionLayout.hpp"

#include "interfaces/AVulkanImageTexture.hpp"

template<class Child>
void
AVulkanImageTexture<Child>::defaultCreateColorResources(
  VkFormat colorImageFormat,
  int32_t imgW,
  int32_t imgH)
{
    colorTex.createColorTexture(_devices, imgW, imgH, colorImageFormat);
}

template<class Child>
void
AVulkanImageTexture<Child>::defaultCreateDepthResources(VkFormat depthFormat,
                                                       int32_t imgW,
                                                       int32_t imgH)
{
    depthTex.createDepthTexture(
      _devices, _cmdPools, _queues, imgW, imgH, depthFormat);
}

template<class Child>
void
AVulkanImageTexture<Child>::transitionColorImageLayout(
  VkCommandBuffer cmdBuffer,
  VkImageLayout oldLayout,
  VkImageLayout newLayout)
{
    transitionImageLayout(cmdBuffer, colorTex, oldLayout, newLayout);
}

template<class Child>
void
AVulkanImageTexture<Child>::transitionDepthImageLayout(
  VkCommandBuffer cmdBuffer,
  VkImageLayout oldLayout,
  VkImageLayout newLayout)
{
    transitionImageLayout(cmdBuffer, depthTex, oldLayout, newLayout);
}

template<class Child>
void
AVulkanImageTexture<Child>::transitionDepthColorImageLayout(
  VkCommandBuffer cmdBuffer,
  VkImageLayout oldLayout,
  VkImageLayout newLayout)
{
    transitionImageLayout(cmdBuffer, colorTex, oldLayout, newLayout);
    transitionImageLayout(cmdBuffer, depthTex, oldLayout, newLayout);
}

template<class Child>
void
AVulkanImageTexture<Child>::copyColorImageContent(
  AVulkanImageTexture const &srcTex,
  VkCommandBuffer cmdBuffer)
{
    VkImageCopy imageRegion{};
    imageRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageRegion.srcSubresource.layerCount = 1;
    imageRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageRegion.dstSubresource.layerCount = 1;
    imageRegion.extent.width = srcTex.colorTex.width;
    imageRegion.extent.height = srcTex.colorTex.height;
    imageRegion.extent.depth = 1;

    vkCmdCopyImage(cmdBuffer,
                   srcTex.colorTex.textureImg,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   colorTex.textureImg,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1,
                   &imageRegion);
}

template<class Child>
void
AVulkanImageTexture<Child>::copyDepthImageContent(
  AVulkanImageTexture const &srcTex,
  VkCommandBuffer cmdBuffer)
{
    VkImageCopy imageRegion{};
    imageRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    imageRegion.srcSubresource.layerCount = 1;
    imageRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    imageRegion.dstSubresource.layerCount = 1;
    imageRegion.extent.width = srcTex.depthTex.width;
    imageRegion.extent.height = srcTex.depthTex.height;
    imageRegion.extent.depth = 1;

    vkCmdCopyImage(cmdBuffer,
                   srcTex.depthTex.textureImg,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   depthTex.textureImg,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1,
                   &imageRegion);
}

template<class Child>
void
AVulkanImageTexture<Child>::copyColorDepthImageContent(
  AVulkanImageTexture const &srcTex,
  VkCommandBuffer cmdBuffer)
{
    copyColorImageContent(srcTex, cmdBuffer);
    copyDepthImageContent(srcTex, cmdBuffer);
}

#endif // VK_MANDELBROT_AVULKANIMAGEBUFFERPRIVATE_HPP
