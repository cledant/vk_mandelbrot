#include "ui/VulkanUiOffscreenRenderPass.hpp"

#include <array>
#include <stdexcept>

#include "interface/AVulkanOffscreenRenderPassPrivate.hpp"

void
VulkanUiOffscreenRenderPass::implInit(VkFormat colorImageFormat,
                                      VkFormat depthImageFormat,
                                      VkImageView colorTexImgView,
                                      VkImageView depthTexImgView,
                                      int32_t imgW,
                                      int32_t imgH)
{
    static_cast<void>(colorTexImgView);
    static_cast<void>(depthTexImgView);
    static_cast<void>(imgW);
    static_cast<void>(imgH);

    defaultCreateRenderPass(colorImageFormat,
                            depthImageFormat,
                            VK_ATTACHMENT_LOAD_OP_LOAD,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    defaultCreateFramebuffer(colorTexImgView, depthTexImgView, imgW, imgH);
}

void
VulkanUiOffscreenRenderPass::implResize(VkFormat colorImageFormat,
                                        VkFormat depthImageFormat,
                                        VkImageView colorTexImgView,
                                        VkImageView depthTexImgView,
                                        int32_t imgW,
                                        int32_t imgH)
{
    static_cast<void>(colorTexImgView);
    static_cast<void>(depthTexImgView);
    static_cast<void>(imgW);
    static_cast<void>(imgH);

    clean();

    defaultCreateRenderPass(colorImageFormat,
                            depthImageFormat,
                            VK_ATTACHMENT_LOAD_OP_LOAD,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    defaultCreateFramebuffer(colorTexImgView, depthTexImgView, imgW, imgH);
}

void
VulkanUiOffscreenRenderPass::implClean()
{
}

void
VulkanUiOffscreenRenderPass::implClear()
{
}