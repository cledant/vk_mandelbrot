#ifndef VK_MANDELBROT_VULKANBUFFER_HPP
#define VK_MANDELBROT_VULKANBUFFER_HPP

#include <vulkan/vulkan.h>

#include "structs/VulkanBasicStructs.hpp"

struct VulkanBuffer final
{
    VkBuffer buffer{};
    VkDeviceMemory memory{};
    VkDeviceSize size{};

    void allocate(VulkanDevices const &devices,
                  VkDeviceSize bufferSize,
                  VkBufferUsageFlags usage,
                  VkMemoryPropertyFlags properties);
    void clear();

  private:
    VulkanDevices _devices{};

    friend struct VulkanTextureStaging;
};

#endif // VK_MANDELBROT_VULKANBUFFER_HPP
