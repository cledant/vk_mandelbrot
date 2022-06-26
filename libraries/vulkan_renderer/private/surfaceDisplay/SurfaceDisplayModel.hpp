#ifndef VK_MANDELBROT_SURFACEDISPLAYMODEL_HPP
#define VK_MANDELBROT_SURFACEDISPLAYMODEL_HPP

#include <array>

#include "glm/glm.hpp"

static constexpr std::array const SURFACE_DISPLAY_VERTICES = {
    glm::vec3(-1.0f, -1.0f, 0.0f),
    glm::vec3(1.0f, -1.0f, 0.0f),
    glm::vec3(1.0f, 1.0f, 0.0f),
    glm::vec3(-1.0f, 1.0f, 0.0f),
};

static constexpr std::array const SURFACE_DISPLAY_INDICES = {
    0, 1, 2, 2, 3, 0
};

#endif // VK_MANDELBROT_SURFACEDISPLAYMODEL_HPP
