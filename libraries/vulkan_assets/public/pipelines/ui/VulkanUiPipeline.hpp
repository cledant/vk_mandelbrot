#ifndef VK_MANDELBROT_VULKANUI_HPP
#define VK_MANDELBROT_VULKANUI_HPP

#include "VulkanInstance.hpp"

class VulkanUiPipeline final
{
  public:
    VulkanUiPipeline() = default;
    ~VulkanUiPipeline() = default;
    VulkanUiPipeline(VulkanUiPipeline const &src) = delete;
    VulkanUiPipeline &operator=(VulkanUiPipeline const &rhs) = delete;
    VulkanUiPipeline(VulkanUiPipeline &&src) = delete;
    VulkanUiPipeline &operator=(VulkanUiPipeline &&rhs) = delete;

    void init(VulkanInstance const &vkInstance,
              VkRenderPass renderPass,
              uint32_t nbImgSwapChain);
    void resize(VkRenderPass renderPass, uint32_t nbImgSwapChain);
    void clear();

    void generateCommands(VkCommandBuffer cmdBuffer);

  private:
    // Vulkan Related
    VkInstance _instance{};
    VulkanDevices _devices;
    VulkanQueues _queues;
    VulkanCommandPools _cmdPools;

    // Global
    VkDescriptorPool _descriptorPool{};

    inline void initImgui(VkRenderPass renderPass, uint32_t nbImgSwapChain);
    inline void loadFonts() const;
};

#endif // VK_MANDELBROT_VULKANUI_HPP
