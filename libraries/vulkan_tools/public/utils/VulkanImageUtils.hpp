#ifndef VK_MANDELBROT_VULKANIMAGE_HPP
#define VK_MANDELBROT_VULKANIMAGE_HPP

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

#include "structs/VulkanCommonStruct.hpp"

void createImage(VkDevice device,
                 VulkanTexture &texture,
                 VkImageTiling tiling,
                 VkImageUsageFlags usage);
void allocateImage(VulkanDevices const &devices,
                   VulkanTexture &texture,
                   VkMemoryPropertyFlags properties);
void copyBufferToImageSingleCmd(VulkanDevices const &devices,
                                VulkanCommandPools const &cmdPools,
                                VulkanQueues const &queues,
                                VulkanBuffer const &stagingBuffer,
                                VulkanTexture const &texture);
void copyImageToBuffer(VkCommandBuffer cmdBuffer,
                       VulkanBuffer const &stagingBuffer,
                       VulkanTexture const &texture);
void copyImageToImage(VkCommandBuffer cmdBuffer,
                      VkImage srcImg,
                      VkImage dstImg,
                      int32_t imgWidth,
                      int32_t imgHeight,
                      VkImageAspectFlags imgAspect);
void createImageView(VulkanDevices const &devices,
                     VulkanTexture &texture,
                     VkImageAspectFlags aspect_flags);
VkFormat findSupportedFormat(VkPhysicalDevice physical_device,
                             std::vector<VkFormat> const &candidates,
                             VkImageTiling tiling,
                             VkFormatFeatureFlags features);
bool hasStencilComponent(VkFormat format);

#endif // VK_MANDELBROT_VULKANIMAGE_HPP
