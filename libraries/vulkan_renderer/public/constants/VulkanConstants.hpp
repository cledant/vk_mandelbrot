#ifndef VK_MANDELBROT_VULKAN_CONSTANTS_HPP
#define VK_MANDELBROT_VULKAN_CONSTANTS_HPP

#include "glm/glm.hpp"

struct mandelbrotConstants final
{
    static constexpr glm::vec4 DEFAULT_BACKGROUND_COLOR{ 0.5, 0.3, 0.3, 0.0 };
    static constexpr glm::vec2 DEFAULT_OFFSET{ 0.0, 0.0 };
    static constexpr uint32_t DEFAULT_MAX_ITER = 250;
    static constexpr float DEFAULT_ZOOM = 1.0f;

    alignas(16) glm::vec4 backgroundColor = DEFAULT_BACKGROUND_COLOR;
    alignas(4) float fwW{};
    alignas(4) float fwH{};
    alignas(4) float screenRatio{};
    alignas(4) uint32_t maxIter = DEFAULT_MAX_ITER;
    alignas(4) float zoom = DEFAULT_ZOOM;
    alignas(8) glm::vec2 offset = DEFAULT_OFFSET;
};

static_assert(sizeof(mandelbrotConstants) <= 128);

#endif // VK_MANDELBROT_VULKAN_CONSTANTS_HPP
