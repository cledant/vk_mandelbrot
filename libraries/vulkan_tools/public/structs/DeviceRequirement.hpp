#ifndef VK_MANDELBROT_DEVICEREQUIREMENT_HPP
#define VK_MANDELBROT_DEVICEREQUIREMENT_HPP

#include <optional>

#include "structs/VulkanInstanceOptions.hpp"

struct DeviceRequirement final
{
    std::optional<uint32_t> graphicFamilyIndex;
    std::optional<uint32_t> presentFamilyIndex;
    std::optional<uint32_t> computeFamilyIndex;
    VkBool32 allExtensionSupported{};
    VulkanInstanceOptions options{};

    [[nodiscard]] bool isValid(
      VulkanInstanceOptions const &requiredOptions) const;
};

#endif // VK_MANDELBROT_DEVICEREQUIREMENT_HPP
