#include "utils/VulkanPhysicalDeviceUtils.hpp"

#include <map>
#include <cstring>
#include <cassert>
#include <set>
#include <string>

#include "utils/VulkanSwapChainUtils.hpp"

VkPhysicalDevice
selectBestDevice(std::vector<VkPhysicalDevice> const &devices,
                 VkSurfaceKHR surface,
                 VulkanInstanceOptions const &requiredOptions)
{
    auto best_device = std::make_pair<VkPhysicalDevice, int>(VK_NULL_HANDLE, 0);
    std::multimap<VkPhysicalDevice, int> rating;

    for (auto const &it : devices) {
        auto score = rateDevice(it, surface, requiredOptions);
        rating.insert(std::make_pair(it, score));
    }

    for (auto const &it : rating) {
        if (it.second > best_device.second) {
            best_device = it;
        }
    }
    return (best_device.first);
}

int
rateDevice(VkPhysicalDevice device,
           VkSurfaceKHR surface,
           VulkanInstanceOptions const &requiredOptions)
{
    auto dfr = getDeviceRequirement(device, surface);
    if (!dfr.isValid(requiredOptions)) {
        return (0);
    }

    if (!checkSwapChainSupport(device, surface)) {
        return (0);
    }

    int score = 0;
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);
    switch (properties.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            score += 1000000;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            score += 5000;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            score += 1;
            break;
        default:
            score += 1000;
            break;
    }

    VkPhysicalDeviceMemoryProperties mem;
    vkGetPhysicalDeviceMemoryProperties(device, &mem);
    std::optional<uint32_t> index_local_memory;
    for (uint32_t i = 0; i < mem.memoryTypeCount; ++i) {
        if (mem.memoryTypes[i].propertyFlags &
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
            index_local_memory = mem.memoryTypes[i].heapIndex;
            break;
        }
    }
    if (!index_local_memory.has_value()) {
        return (0);
    }
    score += mem.memoryHeaps[index_local_memory.value()].size / (1024 * 1024);
    return (score);
}

char *
getDeviceName(char *dst, VkPhysicalDevice device)
{
    assert(dst);

    VkPhysicalDeviceProperties prop;
    vkGetPhysicalDeviceProperties(device, &prop);
    std::strncpy(dst, prop.deviceName, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE);
    return (dst);
}

DeviceRequirement
getDeviceRequirement(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    DeviceRequirement dr{};

    checkDeviceFeaturesSupport(device, dr);
    checkDeviceExtensionSupport(device, dr);
    getDeviceQueues(device, surface, dr);
    return (dr);
}

void
checkDeviceFeaturesSupport(VkPhysicalDevice device, DeviceRequirement &dr)
{
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);
    if (features.geometryShader) {
        dr.options.geometryShader = VK_TRUE;
    }
    if (features.samplerAnisotropy) {
        dr.options.samplerAniso = VK_TRUE;
    }
    if (features.fillModeNonSolid) {
        dr.options.fillModeNonSolid = VK_TRUE;
    }
    if (features.shaderFloat64) {
        dr.options.shaderFloat64 = VK_TRUE;
    }
}

void
checkDeviceExtensionSupport(VkPhysicalDevice device, DeviceRequirement &dr)
{
    uint32_t nb_extension;
    vkEnumerateDeviceExtensionProperties(
      device, nullptr, &nb_extension, nullptr);
    std::vector<VkExtensionProperties> vec_ext_prop(nb_extension);
    vkEnumerateDeviceExtensionProperties(
      device, nullptr, &nb_extension, vec_ext_prop.data());

    std::set<std::string> req_extension(DEVICE_EXTENSIONS.begin(),
                                        DEVICE_EXTENSIONS.end());

    for (auto const &supported_ext : vec_ext_prop) {
        req_extension.erase(supported_ext.extensionName);
    }
    dr.allExtensionSupported = (req_extension.empty()) ? VK_TRUE : VK_FALSE;
}

void
getDeviceQueues(VkPhysicalDevice device,
                VkSurfaceKHR surface,
                DeviceRequirement &dr)
{
    uint32_t nb_family_queue;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &nb_family_queue, nullptr);
    std::vector<VkQueueFamilyProperties> families(nb_family_queue);
    vkGetPhysicalDeviceQueueFamilyProperties(
      device, &nb_family_queue, families.data());

    uint32_t index = 0;
    for (auto const &it : families) {
        if (it.queueFlags & VK_QUEUE_GRAPHICS_BIT && it.queueCount > 0) {
            dr.graphicFamilyIndex = index;
        }
        if (it.queueFlags & VK_QUEUE_COMPUTE_BIT && it.queueCount > 0) {
            dr.computeFamilyIndex = index;
        }

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(
          device, index, surface, &present_support);
        if (present_support) {
            dr.presentFamilyIndex = index;
        }

        if (dr.graphicFamilyIndex.has_value() &&
            dr.presentFamilyIndex.has_value() &&
            dr.computeFamilyIndex.has_value()) {
            break;
        }
        ++index;
    }

    // Check for dedicated compute queue family
    index = 0;
    if (dr.computeFamilyIndex == dr.graphicFamilyIndex) {
        for (auto const &it : families) {
            if (it.queueFlags & VK_QUEUE_COMPUTE_BIT && it.queueCount > 0 &&
                index != dr.computeFamilyIndex) {
                dr.computeFamilyIndex = index;
                break;
            }
            ++index;
        }
    }
}

VkDeviceSize
getMinUniformBufferOffsetAlignment(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);

    return (properties.limits.minUniformBufferOffsetAlignment);
}

bool
getLinearBlittingSupport(VkPhysicalDevice device, VkFormat imgFormat)
{
    VkFormatProperties prop;
    vkGetPhysicalDeviceFormatProperties(device, imgFormat, &prop);

    return (prop.optimalTilingFeatures &
            VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT);
}

VkPhysicalDeviceProperties
getPhysicalDeviceProperties(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);

    return (properties);
}
