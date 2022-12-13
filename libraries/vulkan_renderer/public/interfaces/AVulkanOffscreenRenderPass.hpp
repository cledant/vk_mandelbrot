#ifndef VK_MANDELBROT_AVULKANOFFSCREENRENDERPASS_HPP
#define VK_MANDELBROT_AVULKANOFFSCREENRENDERPASS_HPP

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
              VkFormat depthImageFormat,
              VkImageView colorTexImgView,
              VkImageView depthTexImgView,
              int32_t imgW,
              int32_t imgH);
    void resize(VkFormat colorImageFormat,
                VkFormat depthImageFormat,
                VkImageView colorTexImgView,
                VkImageView depthTexImgView,
                int32_t imgW,
                int32_t imgH);
    void clean();
    void clear();

    VkRenderPass renderPass{};
    VkFramebuffer framebuffer{};

  protected:
    VulkanDevices _devices;

    inline void defaultCreateRenderPass(VkFormat colorImageFormat,
                                        VkAttachmentLoadOp colorLoadOp,
                                        VkImageLayout colorInitialLayout,
                                        VkFormat depthImageFormat,
                                        VkAttachmentLoadOp depthLoadOp,
                                        VkImageLayout depthInitialLayout);
    inline void defaultCreateFramebuffer(VkImageView colorTexImgView,
                                         VkImageView depthTexImgView,
                                         int32_t imgW,
                                         int32_t imgH);
};

template<class Child>
void
AVulkanOffscreenRenderPass<Child>::init(VulkanInstance const &vkInstance,
                                        VkFormat colorImageFormat,
                                        VkFormat depthImageFormat,
                                        VkImageView colorTexImgView,
                                        VkImageView depthTexImgView,
                                        int32_t imgW,
                                        int32_t imgH)
{
    _devices = vkInstance.devices;
    static_cast<Child &>(*this).implInit(colorImageFormat,
                                         depthImageFormat,
                                         colorTexImgView,
                                         depthTexImgView,
                                         imgW,
                                         imgH);
}

template<class Child>
void
AVulkanOffscreenRenderPass<Child>::resize(VkFormat colorImageFormat,
                                          VkFormat depthImageFormat,
                                          VkImageView colorTexImgView,
                                          VkImageView depthTexImgView,
                                          int32_t imgW,
                                          int32_t imgH)
{
    static_cast<Child &>(*this).implResize(colorImageFormat,
                                           depthImageFormat,
                                           colorTexImgView,
                                           depthTexImgView,
                                           imgW,
                                           imgH);
}

template<class Child>
void
AVulkanOffscreenRenderPass<Child>::clear()
{
    static_cast<Child &>(*this).implClear();
    clean();
    framebuffer = nullptr;
    renderPass = nullptr;
    _devices = VulkanDevices{};
}

template<class Child>
void
AVulkanOffscreenRenderPass<Child>::clean()
{
    static_cast<Child &>(*this).implClean();
    if (framebuffer) {
        vkDestroyFramebuffer(_devices.device, framebuffer, nullptr);
    }
    if (renderPass) {
        vkDestroyRenderPass(_devices.device, renderPass, nullptr);
    }
}

#endif // VK_MANDELBROT_AVULKANOFFSCREENRENDERPASS_HPP
