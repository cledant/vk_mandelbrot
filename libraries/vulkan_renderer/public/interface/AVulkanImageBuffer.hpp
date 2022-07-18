#ifndef VK_MANDELBROT_AVULKANIMAGEBUFFER_HPP
#define VK_MANDELBROT_AVULKANIMAGEBUFFER_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"

template<class Child>
class AVulkanImageBuffer
{
  public:
    AVulkanImageBuffer() = default;
    virtual ~AVulkanImageBuffer() = default;
    AVulkanImageBuffer(AVulkanImageBuffer const &src) = default;
    AVulkanImageBuffer &operator=(AVulkanImageBuffer const &rhs) = default;
    AVulkanImageBuffer(AVulkanImageBuffer &&src) noexcept = default;
    AVulkanImageBuffer &operator=(AVulkanImageBuffer &&rhs) noexcept = default;
    void init(VulkanInstance const &vkInstance,
              VkFormat colorImageFormat,
              VkFormat depthFormat,
              int32_t imgW,
              int32_t imgH);
    void resize(VkFormat colorImageFormat,
                VkFormat depthFormat,
                int32_t imgW,
                int32_t imgH);
    void clean();
    void clear();

    VulkanTexture colorTex{};
    VulkanTexture depthTex{};
    VkDescriptorImageInfo descriptorImage{};

  protected:
    VulkanDevices _devices;
    VulkanQueues _queues;
    VulkanCommandPools _cmdPools;

    inline void defaultCreateColorResources(VkFormat colorImageFormat,
                                            int32_t imgW,
                                            int32_t imgH);
    inline void defaultCreateDepthResources(VkFormat depthFormat,
                                            int32_t imgW,
                                            int32_t imgH);
};

template<class Child>
void
AVulkanImageBuffer<Child>::init(VulkanInstance const &vkInstance,
                                VkFormat colorImageFormat,
                                VkFormat depthFormat,
                                int32_t imgW,
                                int32_t imgH)
{
    _devices = vkInstance.devices;
    _queues = vkInstance.queues;
    _cmdPools = vkInstance.cmdPools;
    static_cast<Child &>(*this).implInit(
      colorImageFormat, depthFormat, imgW, imgH);
}

template<class Child>
void
AVulkanImageBuffer<Child>::resize(VkFormat colorImageFormat,
                                  VkFormat depthFormat,
                                  int32_t imgW,
                                  int32_t imgH)
{
    static_cast<Child &>(*this).implResize(
      colorImageFormat, depthFormat, imgW, imgH);
}

template<class Child>
void
AVulkanImageBuffer<Child>::clear()
{
    static_cast<Child &>(*this).implClear();
    clean();
    descriptorImage = {};
    colorTex = VulkanTexture{};
    depthTex = VulkanTexture{};
    _devices = VulkanDevices{};
    _queues = VulkanQueues{};
    _cmdPools = VulkanCommandPools{};
}

template<class Child>
void
AVulkanImageBuffer<Child>::clean()
{
    static_cast<Child &>(*this).implClean();
    descriptorImage = {};
    colorTex.clear();
    depthTex.clear();
}

#endif // VK_MANDELBROT_AVULKANIMAGEBUFFER_HPP
