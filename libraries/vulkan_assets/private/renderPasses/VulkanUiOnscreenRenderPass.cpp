#include "renderPasses/VulkanUiOnscreenRenderPass.hpp"

void
VulkanUiOnscreenRenderPass::implInit(
  VulkanInstance const &vkInstance,
  std::vector<VulkanTexture> const &swapChainImageViews,
  VkFormat swapChainImageFormat)
{
    static_cast<void>(vkInstance);
    defaultCreateRenderPass(swapChainImageFormat,
                            VK_ATTACHMENT_LOAD_OP_LOAD,
                            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                            VK_ATTACHMENT_LOAD_OP_LOAD,
                            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    defaultCreateDepthResources();
    defaultCreateFramebuffers(swapChainImageViews);
}

void
VulkanUiOnscreenRenderPass::implResize(
  std::vector<VulkanTexture> const &swapChainImageViews,
  VkFormat swapChainImageFormat)
{
    clean();
    defaultCreateRenderPass(swapChainImageFormat,
                            VK_ATTACHMENT_LOAD_OP_LOAD,
                            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                            VK_ATTACHMENT_LOAD_OP_LOAD,
                            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    defaultCreateDepthResources();
    defaultCreateFramebuffers(swapChainImageViews);
}

void
VulkanUiOnscreenRenderPass::implClean()
{
}

void
VulkanUiOnscreenRenderPass::implClear()
{
    clean();
}