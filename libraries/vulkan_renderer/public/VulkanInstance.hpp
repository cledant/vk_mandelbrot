#ifndef VK_MANDELBROT_VULKAN_VULKANINSTANCE_HPP
#define VK_MANDELBROT_VULKAN_VULKANINSTANCE_HPP

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#include "common/VulkanCommonStruct.hpp"
#include "VulkanInstanceOptions.hpp"

class VulkanInstance final
{
  public:
    VulkanInstance() = default;
    ~VulkanInstance() = default;
    VulkanInstance(VulkanInstance const &src) = delete;
    VulkanInstance &operator=(VulkanInstance const &rhs) = delete;
    VulkanInstance(VulkanInstance &&src) = delete;
    VulkanInstance &operator=(VulkanInstance &&rhs) = delete;

    static VkInstance createInstance(
      std::string const &app_name,
      std::string const &engine_name,
      uint32_t app_version,
      uint32_t engine_version,
      std::vector<char const *> &&required_extensions);
    void init(VkSurfaceKHR windowSurface, VulkanInstanceOptions const &options);
    void clear();

    VkInstance instance{};
    VkSurfaceKHR surface{};
    VkDebugUtilsMessengerEXT debugMessenger{};

    VulkanDevices devices;
    char deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE]{};

    VulkanQueues queues;
    VulkanCommandPools cmdPools;

  private:
    inline void setupVkDebugMsg();
    inline void selectPhysicalDevice(
      VulkanInstanceOptions const &instanceOptions);
    inline void createQueues();

    // Dbg related
    static inline bool checkValidationLayerSupport();
};

#endif // VK_MANDELBROT_VULKAN_VULKANINSTANCE_HPP
