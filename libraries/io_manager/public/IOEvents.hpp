#ifndef VK_MANDELBROT_IOEVENTS_HPP
#define VK_MANDELBROT_IOEVENTS_HPP

#include <cstdint>
#include <array>

#include "glm/glm.hpp"

enum IOEventsTypes
{
    IOET_QUIT = 0,
    IOET_FULLSCREEN,
    IOET_UP,
    IOET_DOWN,
    IOET_RIGHT,
    IOET_LEFT,
    IOET_RESET_ZOOM_SCREEN_CENTER,
    IOET_INC_ITER,
    IOET_DEC_ITER,
    IOET_RESET_ITER,
    IOET_DISPLAY_UI,
    IOET_DISPLAY_INFO,
    IOET_DISPLAY_FPS,
    IOET_DISPLAY_HELP,
    IOET_TAKE_SCREENSHOT,
    IOET_NB,
};

struct IOEvents final
{
    std::array<uint8_t, IOET_NB> events{};
    glm::vec2 mousePosition{};
    float mouseScroll{};
    bool multiplier{};
};

#endif // VK_MANDELBROT_IOEVENTS_HPP
