#ifndef VK_MANDELBROT_VULKANTRANSITIONLAYOUTUTILS_HPP
#define VK_MANDELBROT_VULKANTRANSITIONLAYOUTUTILS_HPP

#include <vulkan/vulkan.h>

#include "structs/VulkanBasicStructs.hpp"
#include "structs/VulkanTexture.hpp"

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

#endif // VK_MANDELBROT_VULKANTRANSITIONLAYOUTUTILS_HPP
