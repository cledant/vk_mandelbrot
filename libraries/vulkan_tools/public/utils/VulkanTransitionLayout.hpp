#ifndef VK_MANDELBROT_VULKANTRANSITIONLAYOUT_HPP
#define VK_MANDELBROT_VULKANTRANSITIONLAYOUT_HPP

#include <vulkan/vulkan.h>

#include "structs/VulkanCommonStruct.hpp"

void transitionImageLayoutSingleCmd(VulkanDevices const &devices,
                                    VulkanCommandPools const &cmdPools,
                                    VulkanQueues const &queues,
                                    VulkanTexture const &texture,
                                    VkImageLayout oldLayout,
                                    VkImageLayout newLayout);
void transitionImageLayout(VkCommandBuffer cmdBuffer,
                           VulkanTexture const &texture,
                           VkImageLayout oldLayout,
                           VkImageLayout newLayout);

#endif // VK_MANDELBROT_VULKANTRANSITIONLAYOUT_HPP
