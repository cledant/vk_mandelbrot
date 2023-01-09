#ifndef VK_MANDELBROT_AVULKANIMAGETEXTURE_HPP
#define VK_MANDELBROT_AVULKANIMAGETEXTURE_HPP

#include <vector>
#include <array>
#include <stdexcept>

#include <vulkan/vulkan.h>

#include "utils/VulkanImageUtils.hpp"
#include "utils/VulkanTextureUtils.hpp"
#include "utils/VulkanFrameBufferUtils.hpp"
#include "utils/VulkanTransitionLayout.hpp"

#include "interfaces/AVulkanImageTexture.hpp"

#include "VulkanInstance.hpp"

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

    void copyColorTextureContent(AVulkanImageTexture const &srcTex,
                                 VkCommandBuffer cmdBuffer,
                                 VkImageLayout srcInitialLayout,
                                 VkImageLayout srcFinalLayout,
                                 VkImageLayout dslInitialLayout,
                                 VkImageLayout dstFinalLayout) const;
    void copyDepthTextureContent(AVulkanImageTexture const &srcTex,
                                 VkCommandBuffer cmdBuffer,
                                 VkImageLayout srcInitialLayout,
                                 VkImageLayout srcFinalLayout,
                                 VkImageLayout dslInitialLayout,
                                 VkImageLayout dstFinalLayout) const;
    void copyColorDepthTexturesContent(AVulkanImageTexture const &srcTex,
                                       VkCommandBuffer cmdBuffer,
                                       VkImageLayout srcInitialLayout,
                                       VkImageLayout srcFinalLayout,
                                       VkImageLayout dslInitialLayout,
                                       VkImageLayout dstFinalLayout) const;

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
AVulkanImageTexture<Child>::copyColorTextureContent(
  AVulkanImageTexture const &srcTex,
  VkCommandBuffer cmdBuffer,
  VkImageLayout srcInitialLayout,
  VkImageLayout srcFinalLayout,
  VkImageLayout dslInitialLayout,
  VkImageLayout dstFinalLayout) const
{
    transitionImageLayout(cmdBuffer,
                          srcTex.colorTex,
                          srcInitialLayout,
                          VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    transitionImageLayout(cmdBuffer,
                          colorTex,
                          dslInitialLayout,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyImageToImage(cmdBuffer,
                     srcTex.colorTex.textureImg,
                     colorTex.textureImg,
                     srcTex.colorTex.width,
                     srcTex.colorTex.height,
                     VK_IMAGE_ASPECT_COLOR_BIT);
    transitionImageLayout(cmdBuffer,
                          srcTex.colorTex,
                          VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                          srcFinalLayout);
    transitionImageLayout(cmdBuffer,
                          colorTex,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          dstFinalLayout);
}

template<class Child>
void
AVulkanImageTexture<Child>::copyDepthTextureContent(
  AVulkanImageTexture const &srcTex,
  VkCommandBuffer cmdBuffer,
  VkImageLayout srcInitialLayout,
  VkImageLayout srcFinalLayout,
  VkImageLayout dslInitialLayout,
  VkImageLayout dstFinalLayout) const
{
    transitionImageLayout(cmdBuffer,
                          srcTex.depthTex,
                          srcInitialLayout,
                          VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    transitionImageLayout(cmdBuffer,
                          depthTex,
                          dslInitialLayout,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyImageToImage(cmdBuffer,
                     srcTex.depthTex.textureImg,
                     depthTex.textureImg,
                     srcTex.depthTex.width,
                     srcTex.depthTex.height,
                     VK_IMAGE_ASPECT_DEPTH_BIT);
    transitionImageLayout(cmdBuffer,
                          srcTex.depthTex,
                          VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                          srcFinalLayout);
    transitionImageLayout(cmdBuffer,
                          depthTex,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          dstFinalLayout);
}

template<class Child>
void
AVulkanImageTexture<Child>::copyColorDepthTexturesContent(
  AVulkanImageTexture const &srcTex,
  VkCommandBuffer cmdBuffer,
  VkImageLayout srcInitialLayout,
  VkImageLayout srcFinalLayout,
  VkImageLayout dslInitialLayout,
  VkImageLayout dstFinalLayout) const
{
    copyColorTextureContent(srcTex,
                            cmdBuffer,
                            srcInitialLayout,
                            srcFinalLayout,
                            dslInitialLayout,
                            dstFinalLayout);
    copyDepthTextureContent(srcTex,
                            cmdBuffer,
                            srcInitialLayout,
                            srcFinalLayout,
                            dslInitialLayout,
                            dstFinalLayout);
}

#endif // VK_MANDELBROT_AVULKANIMAGETEXTURE_HPP
