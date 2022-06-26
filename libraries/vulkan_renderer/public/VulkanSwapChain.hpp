#ifndef VK_MANDELBROT_VULKAN_VULKANSWAPCHAIN_HPP
#define VK_MANDELBROT_VULKAN_VULKANSWAPCHAIN_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "VulkanInstance.hpp"

class VulkanSwapChain final
{
  public:
    VulkanSwapChain() = default;
    ~VulkanSwapChain() = default;
    VulkanSwapChain(VulkanSwapChain const &src) = delete;
    VulkanSwapChain &operator=(VulkanSwapChain const &rhs) = delete;
    VulkanSwapChain(VulkanSwapChain &&src) = delete;
    VulkanSwapChain &operator=(VulkanSwapChain &&rhs) = delete;

    void init(VulkanInstance const &vkInstance, uint32_t fbW, uint32_t fbH);
    void resize(uint32_t fbW, uint32_t fbH);
    void clean();
    void clear();

    uint32_t oldSwapChainNbImg{};
    uint32_t currentSwapChainNbImg{};
    VkSwapchainKHR swapChain{};
    VkFormat swapChainImageFormat{};
    VkExtent2D swapChainExtent{};
    std::vector<VkImage> swapChainImages;
    std::vector<VulkanTexture> swapChainImageViews;

  private:
    VulkanDevices _devices{};
    VkSurfaceKHR _surface{};

    inline void createSwapChain(uint32_t framebufferW, uint32_t framebufferH);
    inline void createImageView();
};

#endif // VK_MANDELBROT_VULKAN_VULKANSWAPCHAIN_HPP
