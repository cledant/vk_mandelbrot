#include "renderPasses/VulkanMultipleOffscreenRenderPass.hpp"

#include "interfaces/AVulkanOffscreenRenderPassPrivate.hpp"

void
VulkanMultipleOffscreenRenderPass::implInit(VkFormat colorImageFormat,
                                            VkFormat depthImageFormat,
                                            VkImageView colorTexImgView,
                                            VkImageView depthTexImgView,
                                            int32_t imgW,
                                            int32_t imgH)
{
    defaultCreateRenderPass(colorImageFormat,
                            VK_ATTACHMENT_LOAD_OP_LOAD,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                            depthImageFormat,
                            VK_ATTACHMENT_LOAD_OP_LOAD,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    defaultCreateFramebuffer(colorTexImgView, depthTexImgView, imgW, imgH);
}

void
VulkanMultipleOffscreenRenderPass::implResize(VkFormat colorImageFormat,
                                              VkFormat depthImageFormat,
                                              VkImageView colorTexImgView,
                                              VkImageView depthTexImgView,
                                              int32_t imgW,
                                              int32_t imgH)
{
    clean();

    defaultCreateRenderPass(colorImageFormat,
                            VK_ATTACHMENT_LOAD_OP_LOAD,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                            depthImageFormat,
                            VK_ATTACHMENT_LOAD_OP_LOAD,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    defaultCreateFramebuffer(colorTexImgView, depthTexImgView, imgW, imgH);
}

void
VulkanMultipleOffscreenRenderPass::implClean()
{
}

void
VulkanMultipleOffscreenRenderPass::implClear()
{
}