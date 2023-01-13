#ifndef VK_MANDELBROT_VULKANBASICSTRUCTS_HPP
#define VK_MANDELBROT_VULKANBASICSTRUCTS_HPP

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"

struct VulkanDevices final
{
    VkPhysicalDevice physicalDevice{};
    VkDevice device{};
};

struct VulkanCommandPools final
{
    VkCommandPool renderCommandPool{};
};

struct VulkanQueues final
{
    VkQueue graphicQueue{};
    VkQueue presentQueue{};
    uint32_t graphicFamilyIndex{};
    uint32_t presentFamilyIndex{};
};

struct VulkanSimpleVertex final
{
    glm::vec3 position{};
    glm::vec2 texCoord{};
};

#endif // VK_MANDELBROT_VULKANBASICSTRUCTS_HPP
