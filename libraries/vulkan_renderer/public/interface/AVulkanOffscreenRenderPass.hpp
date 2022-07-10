#ifndef PARTICLE_SYSTEM_VULKAN_AVULKANOFFSCREENRENDERPASS_HPP
#define PARTICLE_SYSTEM_VULKAN_AVULKANOFFSCREENRENDERPASS_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"

template<class Child>
class AVulkanOffscreenRenderPass
{
  public:
    AVulkanOffscreenRenderPass() = default;
    virtual ~AVulkanOffscreenRenderPass() = default;
    AVulkanOffscreenRenderPass(AVulkanOffscreenRenderPass const &src) = default;
    AVulkanOffscreenRenderPass &operator=(
      AVulkanOffscreenRenderPass const &rhs) = default;
    AVulkanOffscreenRenderPass(AVulkanOffscreenRenderPass &&src) noexcept =
      default;
    AVulkanOffscreenRenderPass &operator=(
      AVulkanOffscreenRenderPass &&rhs) noexcept = default;
    void init(VulkanInstance const &vkInstance,
              VkFormat colorImageFormat,
              int32_t imgW,
              int32_t imgH);
    void resize(VkFormat colorImageFormat, int32_t imgW, int32_t imgH);
    void clean();
    void clear();

    VulkanTexture colorTex{};
    VulkanTexture depthTex{};
    VkRenderPass renderPass{};
    VkFramebuffer framebuffer{};

  protected:
    VulkanDevices _devices;
    VulkanQueues _queues;
    VulkanCommandPools _cmdPools;

    inline void defaultCreateColorResources(VkFormat colorImageFormat,
                                            int32_t imgW,
                                            int32_t imgH);
    inline void defaultCreateDepthResources(int32_t imgW, int32_t imgH);
    inline void defaultCreateRenderPass(VkFormat colorImageFormat,
                                        VkAttachmentLoadOp loadOp,
                                        VkImageLayout initialLayout);
    inline void defaultCreateFramebuffer(int32_t imgW, int32_t imgH);
};

template<class Child>
void
AVulkanOffscreenRenderPass<Child>::init(VulkanInstance const &vkInstance,
                                        VkFormat colorImageFormat,
                                        int32_t imgW,
                                        int32_t imgH)
{
    _devices = vkInstance.devices;
    _queues = vkInstance.queues;
    _cmdPools = vkInstance.cmdPools;
    static_cast<Child &>(*this).implInit(colorImageFormat, imgW, imgH);
}

template<class Child>
void
AVulkanOffscreenRenderPass<Child>::resize(VkFormat colorImageFormat,
                                          int32_t imgW,
                                          int32_t imgH)
{
    static_cast<Child &>(*this).implResize(colorImageFormat, imgW, imgH);
}

template<class Child>
void
AVulkanOffscreenRenderPass<Child>::clear()
{
    static_cast<Child &>(*this).implClear();
    clean();
    framebuffer = nullptr;
    colorTex = VulkanTexture{};
    depthTex = VulkanTexture{};
    renderPass = nullptr;
    _devices = VulkanDevices{};
    _queues = VulkanQueues{};
    _cmdPools = VulkanCommandPools{};
}

template<class Child>
void
AVulkanOffscreenRenderPass<Child>::clean()
{
    static_cast<Child &>(*this).implClean();
    colorTex.clear();
    depthTex.clear();
    vkDestroyFramebuffer(_devices.device, framebuffer, nullptr);
    vkDestroyRenderPass(_devices.device, renderPass, nullptr);
}

#endif // PARTICLE_SYSTEM_VULKAN_AVULKANOFFSCREENRENDERPASS_HPP
