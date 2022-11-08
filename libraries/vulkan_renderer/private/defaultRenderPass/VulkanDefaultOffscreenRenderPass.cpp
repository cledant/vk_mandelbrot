#include "defaultRenderPass/VulkanDefaultOffscreenRenderPass.hpp"

#include "interfaces/AVulkanOffscreenRenderPassPrivate.hpp"

void
VulkanDefaultOffscreenRenderPass::implInit(VkFormat colorImageFormat,
                                           VkFormat depthImageFormat,
                                           VkImageView colorTexImgView,
                                           VkImageView depthTexImgView,
                                           int32_t imgW,
                                           int32_t imgH)
{
    defaultCreateRenderPass(colorImageFormat,
                            depthImageFormat,
                            VK_ATTACHMENT_LOAD_OP_CLEAR,
                            VK_IMAGE_LAYOUT_UNDEFINED);
    defaultCreateFramebuffer(colorTexImgView, depthTexImgView, imgW, imgH);
}

void
VulkanDefaultOffscreenRenderPass::implResize(VkFormat colorImageFormat,
                                             VkFormat depthImageFormat,
                                             VkImageView colorTexImgView,
                                             VkImageView depthTexImgView,
                                             int32_t imgW,
                                             int32_t imgH)
{
    clean();

    defaultCreateRenderPass(colorImageFormat,
                            depthImageFormat,
                            VK_ATTACHMENT_LOAD_OP_CLEAR,
                            VK_IMAGE_LAYOUT_UNDEFINED);
    defaultCreateFramebuffer(colorTexImgView, depthTexImgView, imgW, imgH);
}

void
VulkanDefaultOffscreenRenderPass::implClean()
{
}

void
VulkanDefaultOffscreenRenderPass::implClear()
{
}