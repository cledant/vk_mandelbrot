#ifndef VK_MANDELBROT_AVULKANIMAGETEXTURE_HPP
#define VK_MANDELBROT_AVULKANIMAGETEXTURE_HPP

#include <vector>
#include <stdexcept>

#include <vulkan/vulkan.h>

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"
#include "utils/VulkanTransitionLayout.hpp"

template<class Child>
class AVulkanImageTexture
{
  public:
    AVulkanImageTexture() = default;
    virtual ~AVulkanImageTexture() = default;
    AVulkanImageTexture(AVulkanImageTexture const &src) = default;
    AVulkanImageTexture &operator=(AVulkanImageTexture const &rhs) = default;
    AVulkanImageTexture(AVulkanImageTexture &&src) noexcept = default;
    AVulkanImageTexture &operator=(AVulkanImageTexture &&rhs) noexcept =
      default;
    void init(VulkanInstance const &vkInstance,
              VkFormat colorImageFormat,
              VkFormat depthFormat,
              int32_t imgW,
              int32_t imgH);
    void resize(VkFormat colorImageFormat,
                VkFormat depthFormat,
                int32_t imgW,
                int32_t imgH);
    void clean();
    void clear();

    void transitionColorImageLayout(VkCommandBuffer cmdBuffer,
                                    VkImageLayout oldLayout,
                                    VkImageLayout newLayout);
    void transitionDepthImageLayout(VkCommandBuffer cmdBuffer,
                                    VkImageLayout oldLayout,
                                    VkImageLayout newLayout);
    void transitionDepthColorImageLayout(VkCommandBuffer cmdBuffer,
                                         VkImageLayout oldLayout,
                                         VkImageLayout newLayout);

    void copyColorImageContent(AVulkanImageTexture const &srcTex,
                               VkCommandBuffer cmdBuffer);
    void copyDepthImageContent(AVulkanImageTexture const &srcTex,
                               VkCommandBuffer cmdBuffer);
    void copyColorDepthImageContent(AVulkanImageTexture const &srcTex,
                                    VkCommandBuffer cmdBuffer);

    VulkanTexture colorTex{};
    VulkanTexture depthTex{};
    VkDescriptorImageInfo descriptorImage{};

  protected:
    VulkanDevices _devices;
    VulkanQueues _queues;
    VulkanCommandPools _cmdPools;

    inline void defaultCreateColorResources(VkFormat colorImageFormat,
                                            int32_t imgW,
                                            int32_t imgH);
    inline void defaultCreateDepthResources(VkFormat depthFormat,
                                            int32_t imgW,
                                            int32_t imgH);
};

template<class Child>
void
AVulkanImageTexture<Child>::init(VulkanInstance const &vkInstance,
                                 VkFormat colorImageFormat,
                                 VkFormat depthFormat,
                                 int32_t imgW,
                                 int32_t imgH)
{
    _devices = vkInstance.devices;
    _queues = vkInstance.queues;
    _cmdPools = vkInstance.cmdPools;
    static_cast<Child &>(*this).implInit(
      colorImageFormat, depthFormat, imgW, imgH);
}

template<class Child>
void
AVulkanImageTexture<Child>::resize(VkFormat colorImageFormat,
                                   VkFormat depthFormat,
                                   int32_t imgW,
                                   int32_t imgH)
{
    static_cast<Child &>(*this).implResize(
      colorImageFormat, depthFormat, imgW, imgH);
}

template<class Child>
void
AVulkanImageTexture<Child>::clear()
{
    static_cast<Child &>(*this).implClear();
    clean();
    descriptorImage = {};
    colorTex = VulkanTexture{};
    depthTex = VulkanTexture{};
    _devices = VulkanDevices{};
    _queues = VulkanQueues{};
    _cmdPools = VulkanCommandPools{};
}

template<class Child>
void
AVulkanImageTexture<Child>::clean()
{
    static_cast<Child &>(*this).implClean();
    descriptorImage = {};
    colorTex.clear();
    depthTex.clear();
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

#endif // VK_MANDELBROT_AVULKANIMAGETEXTURE_HPP
