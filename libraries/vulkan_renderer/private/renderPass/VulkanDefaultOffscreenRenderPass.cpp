#include "renderPass/VulkanDefaultOffscreenRenderPass.hpp"

#include "interface/AVulkanOffscreenRenderPassPrivate.hpp"

void
VulkanDefaultOffscreenRenderPass::implInit(VkFormat colorImageFormat,
                                           int32_t imgW,
                                           int32_t imgH)
{
    defaultCreateColorResources(colorImageFormat, imgW, imgH);
    defaultCreateDepthResources(imgW, imgH);
    defaultCreateRenderPass(colorTex.textureFormat,
                            VK_ATTACHMENT_LOAD_OP_CLEAR,
                            VK_IMAGE_LAYOUT_UNDEFINED);
    defaultCreateFramebuffer(imgW, imgH);
    descriptorImage.sampler = colorTex.textureSampler;
    descriptorImage.imageView = colorTex.textureImgView;
    descriptorImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

void
VulkanDefaultOffscreenRenderPass::implResize(VkFormat colorImageFormat,
                                             int32_t imgW,
                                             int32_t imgH)
{
    clean();

    defaultCreateColorResources(colorImageFormat, imgW, imgH);
    defaultCreateDepthResources(imgW, imgH);
    defaultCreateRenderPass(colorTex.textureFormat,
                            VK_ATTACHMENT_LOAD_OP_CLEAR,
                            VK_IMAGE_LAYOUT_UNDEFINED);
    defaultCreateFramebuffer(imgW, imgH);
    descriptorImage.sampler = colorTex.textureSampler;
    descriptorImage.imageView = colorTex.textureImgView;
    descriptorImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

void
VulkanDefaultOffscreenRenderPass::implClean()
{
}

void
VulkanDefaultOffscreenRenderPass::implClear()
{
}