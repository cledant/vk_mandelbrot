#include "renderPasses/VulkanDefaultOnscreenRenderPass.hpp"

void
VulkanDefaultOnscreenRenderPass::implInit(
  VulkanInstance const &vkInstance,
  std::vector<VulkanTexture> const &swapChainImageViews,
  VkFormat swapChainImageFormat,
  int32_t swapChainImgW,
  int32_t swapChainImgH)
{
    static_cast<void>(vkInstance);
    defaultCreateRenderPass(swapChainImageFormat,
                            VK_ATTACHMENT_LOAD_OP_CLEAR,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                            VK_ATTACHMENT_LOAD_OP_CLEAR,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    defaultCreateDepthResources(swapChainImgW, swapChainImgH);
    defaultCreateFramebuffers(
      swapChainImageViews, swapChainImgW, swapChainImgH);
}

void
VulkanDefaultOnscreenRenderPass::implResize(
  std::vector<VulkanTexture> const &swapChainImageViews,
  VkFormat swapChainImageFormat,
  int32_t swapChainImgW,
  int32_t swapChainImgH)
{
    clean();
    defaultCreateRenderPass(swapChainImageFormat,
                            VK_ATTACHMENT_LOAD_OP_CLEAR,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                            VK_ATTACHMENT_LOAD_OP_CLEAR,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    defaultCreateDepthResources(swapChainImgW, swapChainImgH);
    defaultCreateFramebuffers(
      swapChainImageViews, swapChainImgW, swapChainImgH);
}

void
VulkanDefaultOnscreenRenderPass::implClean()
{
}

void
VulkanDefaultOnscreenRenderPass::implClear()
{
    clean();
}