#ifndef VK_MANDELBROT_VULKANPHYSICALDEVICEUTILS_HPP
#define VK_MANDELBROT_VULKANPHYSICALDEVICEUTILS_HPP

#include <vector>
#include <array>

#include <vulkan/vulkan.h>

#include "structs/DeviceRequirement.hpp"

[[maybe_unused]] constexpr std::array const DEVICE_EXTENSIONS{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

VkPhysicalDevice selectBestDevice(std::vector<VkPhysicalDevice> const &devices,
                                  VkSurfaceKHR surface,
                                  VulkanInstanceOptions const &requiredOptions);
int rateDevice(VkPhysicalDevice device,
               VkSurfaceKHR surface,
               VulkanInstanceOptions const &requiredOptions);
char *getDeviceName(char *dst, VkPhysicalDevice device);
DeviceRequirement getDeviceRequirement(VkPhysicalDevice device,
                                       VkSurfaceKHR surface);
void checkDeviceFeaturesSupport(VkPhysicalDevice device, DeviceRequirement &dr);
void checkDeviceExtensionSupport(VkPhysicalDevice device,
                                 DeviceRequirement &dr);
void getDeviceQueues(VkPhysicalDevice device,
                     VkSurfaceKHR surface,
                     DeviceRequirement &dr);
VkDeviceSize getMinUniformBufferOffsetAlignment(VkPhysicalDevice device);
bool getLinearBlittingSupport(VkPhysicalDevice device, VkFormat imgFormat);
VkPhysicalDeviceProperties getPhysicalDeviceProperties(VkPhysicalDevice device);

#endif // VK_MANDELBROT_VULKANPHYSICALDEVICEUTILS_HPP
