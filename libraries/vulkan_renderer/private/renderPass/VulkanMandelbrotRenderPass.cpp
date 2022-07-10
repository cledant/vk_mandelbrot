#include "renderPass/VulkanMandelbrotRenderPass.hpp"

#include "interface/AVulkanOffscreenRenderPassPrivate.hpp"

void
VulkanMandelbrotRenderPass::implInit(VkFormat colorImageFormat,
                                     int32_t imgW,
                                     int32_t imgH)
{
    defaultCreateColorResources(colorImageFormat, imgW, imgH);
    defaultCreateDepthResources(imgW, imgH);
    defaultCreateRenderPass(colorTex.textureFormat,
                            VK_ATTACHMENT_LOAD_OP_CLEAR,
                            VK_IMAGE_LAYOUT_UNDEFINED);
    defaultCreateFramebuffer(imgW, imgH);
}

void
VulkanMandelbrotRenderPass::implResize(VkFormat colorImageFormat,
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
}

void
VulkanMandelbrotRenderPass::implClean()
{
}

void
VulkanMandelbrotRenderPass::implClear()
{
}