#ifndef PARTICLE_SYS_IOEVENTS_HPP
#define PARTICLE_SYS_IOEVENTS_HPP

#include <cstdint>
#include <array>

#include "glm/glm.hpp"

enum IOEventsTypes
{
    IOET_MOUSE_EXCLUSIVE = 0,
    IOET_QUIT,
    IOET_FULLSCREEN,
    IOET_UP,
    IOET_DOWN,
    IOET_RIGHT,
    IOET_LEFT,
    IOET_LEFT_MOUSE,
    IOET_MIDDLE_MOUSE,
    IOET_RIGHT_MOUSE,
    IOET_HELP,
    IOET_NB,
};

struct IOEvents final
{
    std::array<uint8_t, IOET_NB> events{};
    glm::vec2 mousePosition{};
    float mouseScroll{};
};

#endif // PARTICLE_SYS_IOEVENTS_HPP
