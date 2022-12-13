#ifndef VK_MANDELBROT_AVULKANRENDERPASSPRIVATE_HPP
#define VK_MANDELBROT_AVULKANRENDERPASSPRIVATE_HPP

#include <vector>
#include <array>
#include <stdexcept>

#include <vulkan/vulkan.h>

#include "utils/VulkanImageUtils.hpp"
#include "utils/VulkanSwapChainUtils.hpp"
#include "utils/VulkanFrameBufferUtils.hpp"

#include "interfaces/AVulkanOnscreenRenderPass.hpp"

template<class Child>
void
AVulkanOnscreenRenderPass<Child>::defaultCreateRenderPass(
  VulkanSwapChain const &swapChain,
  VkAttachmentLoadOp colorLoadOp,
  VkImageLayout colorInitialLayout,
  VkImageLayout colorFinalLayout,
  VkAttachmentLoadOp depthLoadOp,
  VkImageLayout depthInitialLayout,
  VkImageLayout depthFinalLayout)
{
    // Color
    VkAttachmentDescription color_attachment{};
    color_attachment.format = swapChain.swapChainImageFormat;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = colorLoadOp;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = colorInitialLayout;
    color_attachment.finalLayout = colorFinalLayout;

    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Depth
    VkAttachmentDescription depth_attachment{};
    depth_attachment.format = depthFormat;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = depthLoadOp;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = depthInitialLayout;
    depth_attachment.finalLayout = depthFinalLayout;

    VkAttachmentReference depth_attachment_ref{};
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.pDepthStencilAttachment = &depth_attachment_ref;

    VkSubpassDependency sub_dep{};
    sub_dep.srcSubpass = VK_SUBPASS_EXTERNAL;
    sub_dep.dstSubpass = 0;
    sub_dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                           VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    sub_dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                           VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    sub_dep.srcAccessMask = 0;
    sub_dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = { color_attachment,
                                                           depth_attachment };
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
          "VulkanDefaultRenderPass: failed to create render pass");
    }
}

template<class Child>
void
AVulkanOnscreenRenderPass<Child>::defaultCreateDepthResources(
  VulkanSwapChain const &swapChain)
{
    depthTex.createDepthTexture(_devices,
                                _cmdPools,
                                _queues,
                                swapChain.swapChainExtent.width,
                                swapChain.swapChainExtent.height,
                                depthFormat);
}

template<class Child>
void
AVulkanOnscreenRenderPass<Child>::defaultCreateFramebuffers(
  VulkanSwapChain const &swapChain)
{
    framebuffers.resize(swapChain.swapChainImageViews.size());

    size_t i = 0;
    for (auto const &it : swapChain.swapChainImageViews) {
        std::array<VkImageView, 2> sciv{ it.textureImgView,
                                         depthTex.textureImgView };

        framebuffers[i] = createFrameBuffer(
          _devices.device, renderPass, sciv, swapChain.swapChainExtent);
        ++i;
    }
}

#endif // VK_MANDELBROT_AVULKANRENDERPASSPRIVATE_HPP
