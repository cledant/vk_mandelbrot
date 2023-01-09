#ifndef VK_MANDELBROT_VULKANINSTANCE_HPP
#define VK_MANDELBROT_VULKANINSTANCE_HPP

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#include "structs/VulkanCommonStruct.hpp"
#include "structs/VulkanInstanceOptions.hpp"

class VulkanInstance final
{
  public:
    VulkanInstance() = default;
    ~VulkanInstance() = default;
    VulkanInstance(VulkanInstance const &src) = delete;
    VulkanInstance &operator=(VulkanInstance const &rhs) = delete;
    VulkanInstance(VulkanInstance &&src) = delete;
    VulkanInstance &operator=(VulkanInstance &&rhs) = delete;

    // Default Values
    static constexpr VulkanInstanceOptions const DEFAULT_INSTANCE_OPTIONS = {
        VK_FALSE, VK_TRUE, VK_FALSE, VK_TRUE, VK_TRUE,
    };

    // Public values
    VkInstance instance{};
    VkSurfaceKHR surface{};
    char deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE]{};
    VulkanDevices devices;
    VulkanQueues queues;
    VulkanCommandPools cmdPools;
    VkFormat depthFormat{};

    void createInstance(std::string &&appName,
                        std::string &&engineName,
                        uint32_t appVersion,
                        uint32_t engineVersion,
                        std::vector<char const *> &&requiredExtensions);
    void createResources(VkSurfaceKHR windowSurface,
                         VulkanInstanceOptions const &options);
    void clearAll();

  private:
    std::string _appName;
    std::string _engineName;
    uint32_t _appVersion{};
    uint32_t _engineVersion{};

    VkDebugUtilsMessengerEXT _debugMessenger{};

    inline void setupVkDebugMsg();
    inline void selectPhysicalDevice(
      VulkanInstanceOptions const &instanceOptions);
    inline void createQueues();

    // Dbg related
    static inline bool checkValidationLayerSupport();
};

#endif // VK_MANDELBROT_VULKANINSTANCE_HPP
