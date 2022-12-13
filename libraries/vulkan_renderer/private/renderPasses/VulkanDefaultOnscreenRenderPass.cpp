#include "renderPasses/VulkanDefaultOnscreenRenderPass.hpp"

#include "interfaces/AVulkanOnscreenRenderPassPrivate.hpp"

void
VulkanDefaultOnscreenRenderPass::implInit(VulkanInstance const &vkInstance,
                                          VulkanSwapChain const &swapChain)
{
    static_cast<void>(vkInstance);
    defaultCreateRenderPass(swapChain,
                            VK_ATTACHMENT_LOAD_OP_CLEAR,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                            VK_ATTACHMENT_LOAD_OP_CLEAR,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    defaultCreateDepthResources(swapChain);
    defaultCreateFramebuffers(swapChain);
}

void
VulkanDefaultOnscreenRenderPass::implResize(VulkanSwapChain const &swapChain)
{
    clean();
    defaultCreateRenderPass(swapChain,
                            VK_ATTACHMENT_LOAD_OP_CLEAR,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                            VK_ATTACHMENT_LOAD_OP_CLEAR,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    defaultCreateDepthResources(swapChain);
    defaultCreateFramebuffers(swapChain);
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