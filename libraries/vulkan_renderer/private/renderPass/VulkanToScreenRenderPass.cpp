#include "renderPass/VulkanToScreenRenderPass.hpp"

#include "interface/AVulkanRenderPassPrivate.hpp"

void
VulkanToScreenRenderPass::implInit(VulkanInstance const &vkInstance,
                                   VulkanSwapChain const &swapChain)
{
    static_cast<void>(vkInstance);
    defaultCreateRenderPass(swapChain,
                            VK_ATTACHMENT_LOAD_OP_CLEAR,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    defaultCreateDepthResources(swapChain);
    defaultCreateFramebuffers(swapChain);
}

void
VulkanToScreenRenderPass::implResize(VulkanSwapChain const &swapChain)
{
    clean();
    defaultCreateRenderPass(swapChain,
                            VK_ATTACHMENT_LOAD_OP_CLEAR,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    defaultCreateDepthResources(swapChain);
    defaultCreateFramebuffers(swapChain);
}

void
VulkanToScreenRenderPass::implClean()
{
}

void
VulkanToScreenRenderPass::implClear()
{
    clean();
}