#ifndef VK_MANDELBROT_AVULKANOFFSCREENRENDERPASSPRIVATE_HPP
#define VK_MANDELBROT_AVULKANOFFSCREENRENDERPASSPRIVATE_HPP

#include <vector>
#include <array>
#include <stdexcept>

#include <vulkan/vulkan.h>

#include "utils/VulkanImageUtils.hpp"
#include "utils/VulkanTextureUtils.hpp"
#include "utils/VulkanSwapChainUtils.hpp"
#include "utils/VulkanFrameBufferUtils.hpp"

#include "interfaces/AVulkanOffscreenRenderPass.hpp"

template<class Child>
void
AVulkanOffscreenRenderPass<Child>::defaultCreateRenderPass(
  VkFormat colorImageFormat,
  VkFormat depthImageFormat,
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

    // Depth
    VkAttachmentDescription depth_attachment{};
    depth_attachment.format = depthImageFormat;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = loadOp;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = initialLayout;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentReference depth_attachment_ref{};
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.pDepthStencilAttachment = &depth_attachment_ref;

    std::array<VkSubpassDependency, 1> sub_dep{};
    sub_dep[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    sub_dep[0].dstSubpass = 0;
    sub_dep[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                              VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    sub_dep[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                              VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    sub_dep[0].srcAccessMask = 0;
    sub_dep[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                               VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = { color_attachment,
                                                           depth_attachment };
    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = attachments.size();
    render_pass_info.pAttachments = attachments.data();
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = sub_dep.size();
    render_pass_info.pDependencies = sub_dep.data();

    if (vkCreateRenderPass(
          _devices.device, &render_pass_info, nullptr, &renderPass) !=
        VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanDefaultOffscreenRenderPass: failed to create render pass");
    }
}

template<class Child>
void
AVulkanOffscreenRenderPass<Child>::defaultCreateFramebuffer(
  VkImageView colorTexImgView,
  VkImageView depthTexImgView,
  int32_t imgW,
  int32_t imgH)
{
    std::array<VkImageView, 2> imgViews{ colorTexImgView, depthTexImgView };

    framebuffer = createFrameBuffer(
      _devices.device,
      renderPass,
      imgViews,
      VkExtent2D{ static_cast<uint32_t>(imgW), static_cast<uint32_t>(imgH) });
}

#endif // VK_MANDELBROT_AVULKANOFFSCREENRENDERPASSPRIVATE_HPP
