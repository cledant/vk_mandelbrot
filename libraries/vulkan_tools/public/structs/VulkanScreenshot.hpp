#ifndef VK_MANDELBROT_VULKANSCREENSHIT_HPP
#define VK_MANDELBROT_VULKANSCREENSHIT_HPP

#include <string>
#include <memory>

#include <vulkan/vulkan.h>

struct VulkanScreenshot final
{
    std::unique_ptr<uint8_t[]> data;
    int32_t width{};
    int32_t height{};
    uint32_t nbChannel{};

    [[nodiscard]] bool saveScreenshotToFile(std::string const &filepath) const;
};

#endif // VK_MANDELBROT_VULKANSCREENSHIT_HPP
