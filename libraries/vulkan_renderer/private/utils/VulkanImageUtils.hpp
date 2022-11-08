#ifndef VK_MANDELBROT_VULKANIMAGE_HPP
#define VK_MANDELBROT_VULKANIMAGE_HPP

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

#include "common/VulkanCommonStruct.hpp"

void createImage(VkDevice device,
                 VulkanTexture &texture,
                 VkImageTiling tiling,
                 VkImageUsageFlags usage);
void allocateImage(VulkanDevices const &devices,
                   VulkanTexture &texture,
                   VkMemoryPropertyFlags properties);
void copyBufferToImage(VulkanDevices const &devices,
                       VulkanCommandPools const &cmdPools,
                       VulkanQueues const &queues,
                       VulkanBuffer const &stagingBuffer,
                       VulkanTexture const &texture);
void createImageView(VulkanDevices const &devices,
                     VulkanTexture &texture,
                     VkImageAspectFlags aspect_flags);
VkFormat findSupportedFormat(VkPhysicalDevice physical_device,
                             std::vector<VkFormat> const &candidates,
                             VkImageTiling tiling,
                             VkFormatFeatureFlags features);
bool hasStencilComponent(VkFormat format);

#endif // VK_MANDELBROT_VULKANIMAGE_HPP
