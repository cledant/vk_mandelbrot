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
    static_cast<void>(depthImageFormat);
    static_cast<void>(colorTexImgView);
    static_cast<void>(depthTexImgView);
    static_cast<void>(imgW);
    static_cast<void>(imgH);

    createUiRenderPass(
      colorImageFormat, VK_ATTACHMENT_LOAD_OP_LOAD, VK_IMAGE_LAYOUT_UNDEFINED);
}

void
VulkanUiOffscreenRenderPass::implResize(VkFormat colorImageFormat,
                                        VkFormat depthImageFormat,
                                        VkImageView colorTexImgView,
                                        VkImageView depthTexImgView,
                                        int32_t imgW,
                                        int32_t imgH)
{
    static_cast<void>(depthImageFormat);
    static_cast<void>(colorTexImgView);
    static_cast<void>(depthTexImgView);
    static_cast<void>(imgW);
    static_cast<void>(imgH);

    clean();

    createUiRenderPass(
      colorImageFormat, VK_ATTACHMENT_LOAD_OP_LOAD, VK_IMAGE_LAYOUT_UNDEFINED);
}

void
VulkanUiOffscreenRenderPass::implClean()
{
}

void
VulkanUiOffscreenRenderPass::implClear()
{
}

void
VulkanUiOffscreenRenderPass::createUiRenderPass(VkFormat colorImageFormat,
                                                VkAttachmentLoadOp loadOp,
                                                VkImageLayout initialLayout)
{
    // Color
    VkAttachmentDescription color_attachment{};
    color_attachment.format = colorImageFormat;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = loadOp;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = initialLayout;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.pDepthStencilAttachment = nullptr;

    VkSubpassDependency sub_dep{};
    sub_dep.srcSubpass = VK_SUBPASS_EXTERNAL;
    sub_dep.dstSubpass = 0;
    sub_dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    sub_dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    sub_dep.srcAccessMask = 0;
    sub_dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 1> attachments = {
        color_attachment,
    };
    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = attachments.size();
    render_pass_info.pAttachments = attachments.data();
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &sub_dep;

    if (vkCreateRenderPass(
          _devices.device, &render_pass_info, nullptr, &renderPass) !=
        VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanUiOffscreenRenderPass: failed to create render pass");
    }
}