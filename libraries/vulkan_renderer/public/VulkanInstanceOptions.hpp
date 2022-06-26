#ifndef VK_MANDELBROT_VULKANINSTANCEOPTIONS_HPP
#define VK_MANDELBROT_VULKANINSTANCEOPTIONS_HPP

#include <vulkan/vulkan.h>

struct VulkanInstanceOptions final
{
    VkBool32 geometryShader{};
    VkBool32 samplerAniso{};
    VkBool32 fillModeNonSolid{};

    [[nodiscard]] bool hasRequiredOptions(
      VulkanInstanceOptions const &physicalDeviceSupportedOpt) const;
};

#endif // VK_MANDELBROT_VULKANINSTANCEOPTIONS_HPP
