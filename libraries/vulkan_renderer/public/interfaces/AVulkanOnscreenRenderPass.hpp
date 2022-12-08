#ifndef VK_MANDELBROT_AVULKANONSCREENRENDERPASS_HPP
#define VK_MANDELBROT_AVULKANONSCREENRENDERPASS_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"

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
              VulkanSwapChain const &swapChain);
    void resize(VulkanSwapChain const &swapChain);
    void clean();
    void clear();

    std::vector<VkFramebuffer> framebuffers;
    VkFormat depthFormat{};
    std::vector<VulkanTexture> depthTexs;
    VkRenderPass renderPass{};

  protected:
    VulkanDevices _devices;
    VulkanQueues _queues;
    VulkanCommandPools _cmdPools;

    inline void defaultCreateRenderPass(VulkanSwapChain const &swapChain,
                                        VkAttachmentLoadOp loadOp,
                                        VkImageLayout colorInitialLayout,
                                        VkImageLayout colorFinalLayout);
    inline void defaultCreateDepthResources(VulkanSwapChain const &swapChain);
    inline void defaultCreateFramebuffers(VulkanSwapChain const &swapChain);
};

template<class Child>
void
AVulkanOnscreenRenderPass<Child>::init(VulkanInstance const &vkInstance,
                                       VulkanSwapChain const &swapChain)
{
    _devices = vkInstance.devices;
    _queues = vkInstance.queues;
    _cmdPools = vkInstance.cmdPools;
    static_cast<Child &>(*this).implInit(vkInstance, swapChain);
}

template<class Child>
void
AVulkanOnscreenRenderPass<Child>::resize(VulkanSwapChain const &swapChain)
{
    static_cast<Child &>(*this).implResize(swapChain);
}

template<class Child>
void
AVulkanOnscreenRenderPass<Child>::clear()
{
    static_cast<Child &>(*this).implClear();
    framebuffers.clear();
    depthTexs.clear();
    renderPass = nullptr;
    _devices = VulkanDevices{};
    _queues = VulkanQueues{};
    _cmdPools = VulkanCommandPools{};
}

template<class Child>
void
AVulkanOnscreenRenderPass<Child>::clean()
{
    static_cast<Child &>(*this).implClean();
    for (auto &it : depthTexs) {
        it.clear();
    }
    for (auto &it : framebuffers) {
        vkDestroyFramebuffer(_devices.device, it, nullptr);
    }
    vkDestroyRenderPass(_devices.device, renderPass, nullptr);
}

#endif // VK_MANDELBROT_AVULKANONSCREENRENDERPASS_HPP
