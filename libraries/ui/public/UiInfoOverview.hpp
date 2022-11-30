#ifndef VK_MANDELBROT_UIINFOOVERVIEW_HPP
#define VK_MANDELBROT_UIINFOOVERVIEW_HPP

#include <string>

#include "glm/glm.hpp"

struct UiInfoOverview final
{
    float avgFps{};
    float currentFps{};
    float renderScale{};
    float zoom{};
    glm::vec2 cameraPos{};
    uint32_t maxIteration{};

  private:
    void draw(bool &fps, bool &info) const;

    friend class Ui;
};

#endif // VK_MANDELBROT_UIINFOOVERVIEW_HPP
