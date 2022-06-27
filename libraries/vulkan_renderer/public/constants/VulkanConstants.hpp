#ifndef VK_MANDELBROT_VULKAN_CONSTANTS_HPP
#define VK_MANDELBROT_VULKAN_CONSTANTS_HPP

#include "glm/glm.hpp"

struct mandelbrotConstants final
{
    alignas(16) glm::vec4 backgroundColor{};
};

#endif // VK_MANDELBROT_VULKAN_CONSTANTS_HPP
