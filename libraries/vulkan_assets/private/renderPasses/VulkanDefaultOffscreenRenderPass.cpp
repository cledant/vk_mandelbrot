#include "renderPasses/VulkanDefaultOffscreenRenderPass.hpp"

void
VulkanDefaultOffscreenRenderPass::implInit(VkFormat colorImageFormat,
                                           VkFormat depthImageFormat,
                                           VkImageView colorTexImgView,
                                           VkImageView depthTexImgView)
{
    defaultCreateRenderPass(colorImageFormat,
                            VK_ATTACHMENT_LOAD_OP_CLEAR,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            depthImageFormat,
                            VK_ATTACHMENT_LOAD_OP_CLEAR,
                            VK_IMAGE_LAYOUT_UNDEFINED);
    defaultCreateFramebuffer(colorTexImgView, depthTexImgView);
}

void
VulkanDefaultOffscreenRenderPass::implResize(VkFormat colorImageFormat,
                                             VkFormat depthImageFormat,
                                             VkImageView colorTexImgView,
                                             VkImageView depthTexImgView)
{
    clean();

    defaultCreateRenderPass(colorImageFormat,
                            VK_ATTACHMENT_LOAD_OP_CLEAR,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            depthImageFormat,
                            VK_ATTACHMENT_LOAD_OP_CLEAR,
                            VK_IMAGE_LAYOUT_UNDEFINED);
    defaultCreateFramebuffer(colorTexImgView, depthTexImgView);
}

void
VulkanDefaultOffscreenRenderPass::implClean()
{
}

void
VulkanDefaultOffscreenRenderPass::implClear()
{
}