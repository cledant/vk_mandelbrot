#ifndef VK_MANDELBROT_VULKANSURFACEDISPLAYPIPELINEDATA_HPP
#define VK_MANDELBROT_VULKANSURFACEDISPLAYPIPELINEDATA_HPP

#include <array>
#include <vector>

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"

#include "common/VulkanCommonStruct.hpp"

struct VulkanSurfaceDisplayPipelineData
{
    VulkanBuffer data{};
    VkDeviceSize verticesSize{};
    VkDeviceSize indicesSize{};
    VkDeviceSize indicesOffset{};
    VkDeviceSize indicesDrawNb{};

    void init(VulkanDevices const &devices,
              VulkanCommandPools const &cmdPools,
              VulkanQueues const &queues,
              VkExtent2D const &screenSize);
    void clear();

  private:
    static inline void forceSquareRatio(VkExtent2D const &screenSize,
                                        std::array<glm::vec3, 4> &vertices);
};

#endif // VK_MANDELBROT_VULKANSURFACEDISPLAYPIPELINEDATA_HPP
