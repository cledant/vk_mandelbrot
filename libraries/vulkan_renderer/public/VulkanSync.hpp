#ifndef VK_MANDELBROT_VULKAN_VULKANSYNC_HPP
#define VK_MANDELBROT_VULKAN_VULKANSYNC_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "VulkanSwapChain.hpp"
#include "common/VulkanCommonStruct.hpp"

class VulkanSync final
{
  public:
    VulkanSync() = default;
    ~VulkanSync() = default;
    VulkanSync(VulkanSync const &src) = delete;
    VulkanSync &operator=(VulkanSync const &rhs) = delete;
    VulkanSync(VulkanSync &&src) = delete;
    VulkanSync &operator=(VulkanSync &&rhs) = delete;

    void init(VulkanInstance const &vkInstance, uint32_t nbFramebufferImgs);
    void resize(uint32_t nbFramebufferImgs);
    void clear();

    static constexpr size_t const MAX_FRAME_INFLIGHT = 2;

    size_t currentFrame{};
    std::vector<VkSemaphore> imageAvailableSem;
    std::vector<VkSemaphore> computeFinishedSem;
    std::vector<VkSemaphore> sceneFinishedSem;
    std::vector<VkSemaphore> allRenderFinishedSem;
    std::vector<VkFence> inflightFence;
    std::vector<VkFence> imgsInflightFence;

  private:
    VulkanDevices _devices;
};

#endif // VK_MANDELBROT_VULKAN_VULKANSYNC_HPP
