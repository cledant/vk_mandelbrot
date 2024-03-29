#ifndef VK_MANDELBROT_VULKANSYNC_HPP
#define VK_MANDELBROT_VULKANSYNC_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"
#include "structs/VulkanBasicStructs.hpp"

class VulkanSync final
{
  public:
    VulkanSync() = default;
    ~VulkanSync() = default;
    VulkanSync(VulkanSync const &src) = delete;
    VulkanSync &operator=(VulkanSync const &rhs) = delete;
    VulkanSync(VulkanSync &&src) = delete;
    VulkanSync &operator=(VulkanSync &&rhs) = delete;

    void init(VulkanInstance const &vkInstance);
    void clear();

    static constexpr size_t const MAX_FRAME_INFLIGHT = 2;

    size_t currentFrame{};
    std::vector<VkSemaphore> imageAvailableSem;
    std::vector<VkSemaphore> allRenderFinishedSem;
    std::vector<VkFence> inflightFence;

  private:
    VulkanDevices _devices;
};

#endif // VK_MANDELBROT_VULKANSYNC_HPP
