#include "VulkanInstance.hpp"

#include <cstring>
#include <stdexcept>
#include <set>

#include "fmt/core.h"

#include "VulkanDebug.hpp"
#include "utils/VulkanPhysicalDevice.hpp"
#include "utils/VulkanCommandBuffer.hpp"
#include "utils/VulkanImageUtils.hpp"

VkInstance
VulkanInstance::createInstance(std::string const &app_name,
                               std::string const &engine_name,
                               uint32_t app_version,
                               uint32_t engine_version,
                               std::vector<char const *> &&required_extensions)
{
    if (ENABLE_VALIDATION_LAYER && !checkValidationLayerSupport()) {
        throw std::runtime_error(
          "VulkanInstance: Validation layer not available");
    }

    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = app_name.c_str();
    app_info.applicationVersion = app_version;
    app_info.pEngineName = engine_name.c_str();
    app_info.engineVersion = engine_version;
    app_info.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo create_info{};

    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount =
      static_cast<uint32_t>(required_extensions.size());
    create_info.ppEnabledExtensionNames = required_extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
    if constexpr (ENABLE_VALIDATION_LAYER) {
        setupVkDebugInfo(debug_create_info);
        create_info.enabledLayerCount =
          static_cast<uint32_t>(VALIDATION_LAYERS.size());
        create_info.ppEnabledLayerNames = VALIDATION_LAYERS.data();
        create_info.pNext = &debug_create_info;
    } else {
        create_info.enabledLayerCount = 0;
        create_info.pNext = nullptr;
    }

    VkInstance instance;
    if (vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("VulkanInstance: Failed to create instance");
    }
    return (instance);
}

void
VulkanInstance::init(VkSurfaceKHR windowSurface,
                     VulkanInstanceOptions const &options)
{
    surface = windowSurface;
    setupVkDebugMsg();
    selectPhysicalDevice(options);
    createQueues();
    cmdPools.renderCommandPool =
      createCommandPool(devices.device,
                        queues.graphicFamilyIndex,
                        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    cmdPools.computeCommandPool =
      (queues.computeFamilyIndex == queues.graphicFamilyIndex)
        ? cmdPools.renderCommandPool
        : createCommandPool(devices.device, queues.computeFamilyIndex, 0);
    depthFormat =
      findSupportedFormat(devices.physicalDevice,
                          { VK_FORMAT_D32_SFLOAT,
                            VK_FORMAT_D32_SFLOAT_S8_UINT,
                            VK_FORMAT_D24_UNORM_S8_UINT },
                          VK_IMAGE_TILING_OPTIMAL,
                          VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

void
VulkanInstance::clear()
{
    if (cmdPools.computeCommandPool != cmdPools.renderCommandPool) {
        vkDestroyCommandPool(
          devices.device, cmdPools.computeCommandPool, nullptr);
    }
    vkDestroyCommandPool(devices.device, cmdPools.renderCommandPool, nullptr);
    vkDestroyDevice(devices.device, nullptr);
    if constexpr (ENABLE_VALIDATION_LAYER) {
        destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
    instance = nullptr;
    surface = nullptr;
    debugMessenger = nullptr;
    devices = VulkanDevices{};
    queues = VulkanQueues{};
    cmdPools = VulkanCommandPools{};
    depthFormat = {};
}

void
VulkanInstance::setupVkDebugMsg()
{
    if constexpr (!ENABLE_VALIDATION_LAYER) {
        return;
    }
    VkDebugUtilsMessengerCreateInfoEXT dbg_info{};
    setupVkDebugInfo(dbg_info);

    createDebugUtilsMessengerEXT(instance, &dbg_info, nullptr, &debugMessenger);
}

void
VulkanInstance::selectPhysicalDevice(
  VulkanInstanceOptions const &instanceOptions)
{
    uint32_t nb_physical_device = 0;
    vkEnumeratePhysicalDevices(instance, &nb_physical_device, nullptr);
    if (!nb_physical_device) {
        throw std::runtime_error("VulkanInstance: No physical device");
    }

    std::vector<VkPhysicalDevice> phy_devices(nb_physical_device);
    vkEnumeratePhysicalDevices(
      instance, &nb_physical_device, phy_devices.data());

    devices.physicalDevice =
      selectBestDevice(phy_devices, surface, instanceOptions);
    if (devices.physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("VulkanInstance: No Suitable device found");
    }
    getDeviceName(deviceName, devices.physicalDevice);
    fmt::print("Device: {}\n", deviceName);
}

void
VulkanInstance::createQueues()
{
    auto dfr = getDeviceRequirement(devices.physicalDevice, surface);
    std::set<uint32_t> queue_families = { dfr.graphicFamilyIndex.value(),
                                          dfr.presentFamilyIndex.value(),
                                          dfr.computeFamilyIndex.value() };
    std::vector<VkDeviceQueueCreateInfo> vec_queue_create_info;

    // Graphic queue info
    float queue_priority = 1.0f;
    for (auto const &it : queue_families) {
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = it;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        vec_queue_create_info.emplace_back(queue_create_info);
    }

    // Device info
    VkPhysicalDeviceFeatures physical_device_features{};
    physical_device_features.geometryShader = VK_FALSE;
    physical_device_features.samplerAnisotropy = VK_TRUE;
    physical_device_features.fillModeNonSolid = VK_FALSE;
    physical_device_features.shaderFloat64 = VK_TRUE;
    VkDeviceCreateInfo device_create_info{};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pQueueCreateInfos = vec_queue_create_info.data();
    device_create_info.queueCreateInfoCount = vec_queue_create_info.size();
    device_create_info.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();
    device_create_info.enabledExtensionCount = DEVICE_EXTENSIONS.size();
    if constexpr (ENABLE_VALIDATION_LAYER) {
        device_create_info.enabledLayerCount =
          static_cast<uint32_t>(VALIDATION_LAYERS.size());
        device_create_info.ppEnabledLayerNames = VALIDATION_LAYERS.data();
    } else {
        device_create_info.enabledLayerCount = 0;
    }
    device_create_info.pEnabledFeatures = &physical_device_features;

    // Device creation
    if (vkCreateDevice(devices.physicalDevice,
                       &device_create_info,
                       nullptr,
                       &devices.device) != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanInstance: Failed to create logical device");
    }
    vkGetDeviceQueue(
      devices.device, dfr.graphicFamilyIndex.value(), 0, &queues.graphicQueue);
    vkGetDeviceQueue(
      devices.device, dfr.presentFamilyIndex.value(), 0, &queues.presentQueue);
    vkGetDeviceQueue(
      devices.device, dfr.computeFamilyIndex.value(), 0, &queues.computeQueue);
    queues.graphicFamilyIndex = dfr.graphicFamilyIndex.value();
    queues.presentFamilyIndex = dfr.presentFamilyIndex.value();
    queues.computeFamilyIndex = dfr.computeFamilyIndex.value();
}

// Dbg related
bool
VulkanInstance::checkValidationLayerSupport()
{
    uint32_t nb_avail_layers;
    vkEnumerateInstanceLayerProperties(&nb_avail_layers, nullptr);

    std::vector<VkLayerProperties> avail_layers(nb_avail_layers);
    vkEnumerateInstanceLayerProperties(&nb_avail_layers, avail_layers.data());

    for (auto const &it_val_layer : VALIDATION_LAYERS) {
        bool layer_found = false;

        for (auto const &it_avail_layer : avail_layers) {
            if (!std::strcmp(it_avail_layer.layerName, it_val_layer)) {
                layer_found = true;
                break;
            }
        }
        if (!layer_found) {
            return (false);
        }
    }
    return (true);
}