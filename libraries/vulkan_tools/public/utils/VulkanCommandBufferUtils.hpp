#ifndef VK_MANDELBROT_VULKANCOMMANDBUFFERUTILS_HPP
#define VK_MANDELBROT_VULKANCOMMANDBUFFERUTILS_HPP

#include <vector>

#include <vulkan/vulkan.h>

VkCommandBuffer beginSingleTimeCommands(VkDevice device,
                                        VkCommandPool command_pool);
void endSingleTimeCommands(VkDevice device,
                           VkCommandPool command_pool,
                           VkCommandBuffer command_buffer,
                           VkQueue gfx_queue);
VkCommandPool createCommandPool(VkDevice device,
                                uint32_t queueIndex,
                                VkCommandPoolCreateFlags flags);
void allocateCommandBuffers(VkDevice device,
                            VkCommandPool cmdPool,
                            std::vector<VkCommandBuffer> &cmdBuffers,
                            uint32_t nbCmdBuffers);

#endif // VK_MANDELBROT_VULKANCOMMANDBUFFERUTILS_HPP
