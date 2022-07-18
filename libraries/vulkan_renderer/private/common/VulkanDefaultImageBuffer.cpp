#include "common/VulkanDefaultImageBuffer.hpp"

#include "interface/AVulkanImageBufferPrivate.hpp"

void
VulkanDefaultImageBuffer::implInit(VkFormat colorImageFormat,
                                   VkFormat depthImageFormat,
                                   int32_t imgW,
                                   int32_t imgH)
{
    defaultCreateColorResources(colorImageFormat, imgW, imgH);
    defaultCreateDepthResources(depthImageFormat, imgW, imgH);
    descriptorImage.sampler = colorTex.textureSampler;
    descriptorImage.imageView = colorTex.textureImgView;
    descriptorImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

void
VulkanDefaultImageBuffer::implResize(VkFormat colorImageFormat,
                                     VkFormat depthImageFormat,
                                     int32_t imgW,
                                     int32_t imgH)
{
    clean();

    defaultCreateColorResources(colorImageFormat, imgW, imgH);
    defaultCreateDepthResources(depthImageFormat, imgW, imgH);
    descriptorImage.sampler = colorTex.textureSampler;
    descriptorImage.imageView = colorTex.textureImgView;
    descriptorImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

void
VulkanDefaultImageBuffer::implClean()
{
}

void
VulkanDefaultImageBuffer::implClear()
{
}