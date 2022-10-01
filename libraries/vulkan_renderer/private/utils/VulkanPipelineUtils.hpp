#ifndef VK_MANDELBROT_VULKAN_VULKANPIPELINEUTILS_HPP
#define VK_MANDELBROT_VULKAN_VULKANPIPELINEUTILS_HPP

#include "common/VulkanCommonStruct.hpp"

VkPipelineLayout createPipelineLayout(
  VulkanDevices const &devices,
  VkDescriptorSetLayout descriptorSetLayout);
VkPipelineLayout createPipelineLayout(VulkanDevices const &devices,
                                      VkDescriptorSetLayout descriptorSetLayout,
                                      VkPushConstantRange pushConstantRange);

#endif // VK_MANDELBROT_VULKAN_VULKANPIPELINEUTILS_HPP
