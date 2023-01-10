#include "renderPasses/VulkanMultipleOffscreenRenderPass.hpp"

void
VulkanMultipleOffscreenRenderPass::implInit(VkFormat colorImageFormat,
                                            VkFormat depthImageFormat,
                                            VkImageView colorTexImgView,
                                            VkImageView depthTexImgView)
{
    defaultCreateRenderPass(colorImageFormat,
                            VK_ATTACHMENT_LOAD_OP_LOAD,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                            depthImageFormat,
                            VK_ATTACHMENT_LOAD_OP_LOAD,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    defaultCreateFramebuffer(colorTexImgView, depthTexImgView);
}

void
VulkanMultipleOffscreenRenderPass::implResize(VkFormat colorImageFormat,
                                              VkFormat depthImageFormat,
                                              VkImageView colorTexImgView,
                                              VkImageView depthTexImgView)
{
    clean();

    defaultCreateRenderPass(colorImageFormat,
                            VK_ATTACHMENT_LOAD_OP_LOAD,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                            depthImageFormat,
                            VK_ATTACHMENT_LOAD_OP_LOAD,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    defaultCreateFramebuffer(colorTexImgView, depthTexImgView);
}

void
VulkanMultipleOffscreenRenderPass::implClean()
{
}

void
VulkanMultipleOffscreenRenderPass::implClear()
{
}