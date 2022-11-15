#ifndef VK_MANDELBROT_UIRGBCOLORINPUTWINDOW_HPP
#define VK_MANDELBROT_UIRGBCOLORINPUTWINDOW_HPP

#include <string>

#include "imgui.h"

struct UiRGBColorInputWindow final
{
    std::string windowName = "No Name";
    uint32_t winW = 400;
    uint32_t winH = 0;
    ImVec4 color{};
    bool isInputOpen = false;

  private:
    bool draw();

    friend class Ui;
};

#endif // VK_MANDELBROT_UIRGBCOLORINPUTWINDOW_HPP
