#ifndef VK_MANDELBROT_VULKAN_CONSTANTS_HPP
#define VK_MANDELBROT_VULKAN_CONSTANTS_HPP

#include "glm/glm.hpp"

struct mandelbrotPushConstants final
{
    static constexpr glm::vec4 DEFAULT_COLOR{ 0.6, 0.3, 0.3, 0.0 };
    static constexpr glm::vec4 DEFAULT_MAX_ITER_COLOR{ 0.1, 0.1, 0.3, 0.0 };
    static constexpr glm::vec2 DEFAULT_OFFSET{ 0.5, 0.5 };
    static constexpr uint32_t DEFAULT_MAX_ITER = 200;
    static constexpr float DEFAULT_ZOOM = 4.0f;

    alignas(16) glm::vec4 color = DEFAULT_COLOR;
    alignas(16) glm::vec4 maxIterColor = DEFAULT_MAX_ITER_COLOR;
    alignas(4) float fwW{};
    alignas(4) float fwH{};
    alignas(4) uint32_t maxIter = DEFAULT_MAX_ITER;
    alignas(4) float zoom = DEFAULT_ZOOM;
    alignas(4) float zoomMultScreenRatio{};
    alignas(8) glm::vec2 offset = DEFAULT_OFFSET;
};

static_assert(sizeof(mandelbrotPushConstants) <= 128);

#endif // VK_MANDELBROT_VULKAN_CONSTANTS_HPP
