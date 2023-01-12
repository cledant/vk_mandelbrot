#ifndef VK_MANDELBROT_AVULKANONSCREENRENDERPASS_HPP
#define VK_MANDELBROT_AVULKANONSCREENRENDERPASS_HPP

#include <vector>
#include <array>
#include <stdexcept>

#include <vulkan/vulkan.h>

#include "utils/VulkanImageUtils.hpp"
#include "utils/VulkanSwapChainUtils.hpp"
#include "utils/VulkanFrameBufferUtils.hpp"

#include "interfaces/AVulkanOnscreenRenderPass.hpp"

#include "VulkanInstance.hpp"

template<class Child>
class AVulkanOnscreenRenderPass
{
  public:
    AVulkanOnscreenRenderPass() = default;
    virtual ~AVulkanOnscreenRenderPass() = default;
    AVulkanOnscreenRenderPass(AVulkanOnscreenRenderPass const &src) = default;
    AVulkanOnscreenRenderPass &operator=(AVulkanOnscreenRenderPass const &rhs) =
      default;
    AVulkanOnscreenRenderPass(AVulkanOnscreenRenderPass &&src) noexcept =
      default;
    AVulkanOnscreenRenderPass &operator=(
      AVulkanOnscreenRenderPass &&rhs) noexcept = default;
    void init(VulkanInstance const &vkInstance,
              std::vector<VulkanTexture> const &swapChainImageViews,
              VkFormat swapChainImageFormat,
              int32_t swapChainImgW,
              int32_t swapChainImgH);
    void resize(std::vector<VulkanTexture> const &swapChainImageViews,
                VkFormat swapChainImageFormat,
                int32_t swapChainImgW,
                int32_t swapChainImgH);
    void clean();
    void clear();

    std::vector<VkFramebuffer> framebuffers;
    VkFormat depthFormat{};
    VulkanTexture depthTex;
    VkRenderPass renderPass{};
    VkExtent2D renderPassExtent{};

  protected:
    VulkanDevices _devices;
    VulkanQueues _queues;
    VulkanCommandPools _cmdPools;

    inline void defaultCreateRenderPass(VkFormat swapChainImageFormat,
                                        VkAttachmentLoadOp colorLoadOp,
                                        VkImageLayout colorInitialLayout,
                                        VkImageLayout colorFinalLayout,
                                        VkAttachmentLoadOp depthLoadOp,
                                        VkImageLayout depthInitialLayout,
                                        VkImageLayout depthFinalLayout);
    inline void defaultCreateDepthResources();
    inline void defaultCreateFramebuffers(
      std::vector<VulkanTexture> const &swapChainImageViews);
};

template<class Child>
void
AVulkanOnscreenRenderPass<Child>::init(
  VulkanInstance const &vkInstance,
  std::vector<VulkanTexture> const &swapChainImageViews,
  VkFormat swapChainImageFormat,
  int32_t swapChainImgW,
  int32_t swapChainImgH)
{
    _devices = vkInstance.devices;
    _queues = vkInstance.queues;
    _cmdPools = vkInstance.cmdPools;
    depthFormat = vkInstance.depthFormat;
    renderPassExtent = { static_cast<uint32_t>(swapChainImgW),
                         static_cast<uint32_t>(swapChainImgH) };
    static_cast<Child &>(*this).implInit(
      vkInstance, swapChainImageViews, swapChainImageFormat);
}

template<class Child>
void
AVulkanOnscreenRenderPass<Child>::resize(
  std::vector<VulkanTexture> const &swapChainImageViews,
  VkFormat swapChainImageFormat,
  int32_t swapChainImgW,
  int32_t swapChainImgH)
{
    clean();

    renderPassExtent = { static_cast<uint32_t>(swapChainImgW),
                         static_cast<uint32_t>(swapChainImgH) };
    static_cast<Child &>(*this).implResize(swapChainImageViews,
                                           swapChainImageFormat);
}

template<class Child>
void
AVulkanOnscreenRenderPass<Child>::clear()
{
    static_cast<Child &>(*this).implClear();
    clean();
    framebuffers.clear();
    depthTex = VulkanTexture{};
    renderPass = nullptr;
    _devices = VulkanDevices{};
    _queues = VulkanQueues{};
    _cmdPools = VulkanCommandPools{};
    depthFormat = {};
    renderPassExtent = {};
}

template<class Child>
void
AVulkanOnscreenRenderPass<Child>::clean()
{
    static_cast<Child &>(*this).implClean();
    depthTex.clear();
    for (auto &it : framebuffers) {
        vkDestroyFramebuffer(_devices.device, it, nullptr);
    }
    vkDestroyRenderPass(_devices.device, renderPass, nullptr);
}

template<class Child>
void
AVulkanOnscreenRenderPass<Child>::defaultCreateRenderPass(
  VkFormat swapChainImageFormat,
  VkAttachmentLoadOp colorLoadOp,
  VkImageLayout colorInitialLayout,
  VkImageLayout colorFinalLayout,
  VkAttachmentLoadOp depthLoadOp,
  VkImageLayout depthInitialLayout,
  VkImageLayout depthFinalLayout)
{
    // Color
    VkAttachmentDescription color_attachment{};
    color_attachment.format = swapChainImageFormat;
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
AVulkanOnscreenRenderPass<Child>::defaultCreateDepthResources()
{
    depthTex.createDepthTexture(_devices,
                                _cmdPools,
                                _queues,
                                renderPassExtent.width,
                                renderPassExtent.height,
                                depthFormat);
}

template<class Child>
void
AVulkanOnscreenRenderPass<Child>::defaultCreateFramebuffers(
  std::vector<VulkanTexture> const &swapChainImageViews)
{
    framebuffers.resize(swapChainImageViews.size());

    size_t i = 0;
    for (auto const &it : swapChainImageViews) {
        std::array<VkImageView, 2> sciv{ it.textureImgView,
                                         depthTex.textureImgView };

        framebuffers[i] = createFrameBuffer(
          _devices.device, renderPass, sciv, renderPassExtent);
        ++i;
    }
}

#endif // VK_MANDELBROT_AVULKANONSCREENRENDERPASS_HPP
