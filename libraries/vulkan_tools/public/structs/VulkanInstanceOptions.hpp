#ifndef VK_MANDELBROTINSTANCEOPTIONS_HPP
#define VK_MANDELBROTINSTANCEOPTIONS_HPP

#include <vulkan/vulkan.h>

struct VulkanInstanceOptions final
{
    VkBool32 geometryShader{};
    VkBool32 samplerAniso{};
    VkBool32 fillModeNonSolid{};
    VkBool32 shaderFloat64{};
    VkBool32 vsync{};
    float rendererScale = 1.0f;

    [[nodiscard]] bool hasRequiredOptions(
      VulkanInstanceOptions const &physicalDeviceSupportedOpt) const;
};

#endif // VK_MANDELBROTINSTANCEOPTIONS_HPP
