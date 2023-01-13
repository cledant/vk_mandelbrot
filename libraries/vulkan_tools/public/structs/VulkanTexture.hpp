#ifndef VK_MANDELBROT_VULKANTEXTURE_HPP
#define VK_MANDELBROT_VULKANTEXTURE_HPP

#include <vulkan/vulkan.h>

#include "structs/VulkanTextureStaging.hpp"

struct VulkanTexture final
{
    VkImage textureImg{};
    VkDeviceMemory textureImgMemory{};
    VkImageView textureImgView{};
    VkSampler textureSampler{};
    int32_t width{};
    int32_t height{};
    uint32_t mipLevel = 1;
    VkFormat textureFormat{};
    bool isCubemap{};

    void loadTextureOnDevice(VulkanDevices const &devices,
                             VulkanCommandPools const &cmdPools,
                             VulkanQueues const &queues,
                             VulkanTextureStaging const &stagingTexture,
                             VkFormat format);
    void loadTextureOnHost(VulkanCommandPools const &cmdPools,
                           VulkanQueues const &queues,
                           VulkanTextureStaging const &stagingTexture) const;
    void createColorTexture(VulkanDevices const &devices,
                            int32_t texW,
                            int32_t texH,
                            VkFormat depthFormat);
    void createDepthTexture(VulkanDevices const &devices,
                            VulkanCommandPools const &cmdPools,
                            VulkanQueues const &queues,
                            int32_t texW,
                            int32_t texH,
                            VkFormat depthFormat);
    void createSwapchainTexture(VulkanDevices const &devices,
                                VkImage swapchainImg,
                                VkFormat swapchainFormat,
                                VkExtent2D extent);
    void clear();
    void clearSwapchainTexture();

  private:
    VulkanDevices _devices;
};

#endif // VK_MANDELBROT_VULKANTEXTURE_HPP
