#ifndef VK_MANDELBROT_MANDELBROTPUSHCONSTANTS_HPP
#define VK_MANDELBROT_MANDELBROTPUSHCONSTANTS_HPP

#include "glm/glm.hpp"

struct mandelbrotPushConstants final
{
    static constexpr double DEFAULT_OFFSET_X = 0.0;
    static constexpr double DEFAULT_OFFSET_Y = 0.0;
    static constexpr uint32_t DEFAULT_MAX_ITER = 350;
    static constexpr double DEFAULT_ZOOM = 4.0;

    alignas(8) double fbW{};
    alignas(8) double fbH{};
    alignas(4) uint32_t maxIter = DEFAULT_MAX_ITER;
    alignas(8) double zoom = DEFAULT_ZOOM;
    alignas(8) double zoomMultScreenRatio{};
    alignas(8) double offsetX = DEFAULT_OFFSET_X;
    alignas(8) double offsetY = DEFAULT_OFFSET_Y;
};

static_assert(sizeof(mandelbrotPushConstants) <= 128);

#endif // VK_MANDELBROT_MANDELBROTPUSHCONSTANTS_HPP
