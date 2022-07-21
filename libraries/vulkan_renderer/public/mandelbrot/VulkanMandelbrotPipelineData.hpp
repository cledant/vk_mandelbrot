#ifndef VK_MANDELBROT_VULKANMANDELBROTPIPELINEDATA_HPP
#define VK_MANDELBROT_VULKANMANDELBROTPIPELINEDATA_HPP

#include <array>
#include <vector>

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"

#include "common/VulkanCommonStruct.hpp"

struct VulkanMandelbrotPipelineData
{
    VulkanBuffer data{};
    VkDeviceSize verticesSize{};
    VkDeviceSize indicesSize{};
    VkDeviceSize indicesOffset{};
    VkDeviceSize indicesDrawNb{};

    void init(VulkanDevices const &devices,
              VulkanCommandPools const &cmdPools,
              VulkanQueues const &queues);
    void clear();
};

#endif // VK_MANDELBROT_VULKANMANDELBROTPIPELINEDATA_HPP
