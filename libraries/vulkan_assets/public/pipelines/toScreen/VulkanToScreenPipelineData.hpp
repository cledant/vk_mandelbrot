#ifndef VK_MANDELBROT_VULKANTOSCREENPIPELINEDATA_HPP
#define VK_MANDELBROT_VULKANTOSCREENPIPELINEDATA_HPP

#include <array>
#include <vector>

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"

#include "structs/VulkanBasicStructs.hpp"
#include "structs/VulkanBuffer.hpp"

struct VulkanToScreenPipelineData
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

#endif // VK_MANDELBROT_VULKANTOSCREENPIPELINEDATA_HPP
