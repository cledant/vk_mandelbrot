#ifndef VK_MANDELBROT_VULKANDEBUG_HPP
#define VK_MANDELBROT_VULKANDEBUG_HPP

#include <array>

#include <vulkan/vulkan.h>

#ifdef NDEBUG
inline constexpr bool const ENABLE_VALIDATION_LAYER = false;
#else
inline constexpr bool const ENABLE_VALIDATION_LAYER = true;
#endif
inline constexpr std::array const VALIDATION_LAYERS{
    "VK_LAYER_KHRONOS_validation",
};

void setupVkDebugInfo(VkDebugUtilsMessengerCreateInfoEXT &create_info);
VKAPI_ATTR VkBool32 VKAPI_CALL
VkDebugMsgCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                   VkDebugUtilsMessageTypeFlagsEXT messageType,
                   VkDebugUtilsMessengerCallbackDataEXT const *pCallbackData,
                   void *pUserData);
VkResult createDebugUtilsMessengerEXT(
  VkInstance instance,
  VkDebugUtilsMessengerCreateInfoEXT const *pCreateInfo,
  VkAllocationCallbacks const *pAllocator,
  VkDebugUtilsMessengerEXT *pDebugMessenger);
void destroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   VkAllocationCallbacks const *pAllocator);
char const *getDbgSeverityStr(VkDebugUtilsMessageSeverityFlagBitsEXT severity);
char const *getDbgMessageTypeStr(VkDebugUtilsMessageTypeFlagsEXT type);

#endif // VK_MANDELBROT_VULKANDEBUG_HPP