#include "common/VulkanDefaultImageTexture.hpp"

#include "interface/AVulkanImageBufferPrivate.hpp"

void
VulkanDefaultImageTexture::implInit(VkFormat colorImageFormat,
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
VulkanDefaultImageTexture::implResize(VkFormat colorImageFormat,
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
VulkanDefaultImageTexture::implClean()
{
}

void
VulkanDefaultImageTexture::implClear()
{
}