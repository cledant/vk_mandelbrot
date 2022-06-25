#ifndef PARTICLE_SYS_VULKAN_IOMANAGERWINDOWCREATIONOPTION_HPP
#define PARTICLE_SYS_VULKAN_IOMANAGERWINDOWCREATIONOPTION_HPP

#include <string>
#include <cstdint>

#include "glm/glm.hpp"

static constexpr glm::ivec2 const DEFAULT_WIN_SIZE = glm::ivec2(1280, 720);

struct IOManagerWindowCreationOption final
{
    bool resizable{};
    bool cursorHiddenOnWindow{};
    bool fullscreen{};
    bool mouseExclusive{};
    glm::ivec2 size{};
    std::string winName;
};

#endif // PARTICLE_SYS_VULKAN_IOMANAGERWINDOWCREATIONOPTION_HPP
