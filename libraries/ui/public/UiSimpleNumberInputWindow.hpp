#ifndef VK_MANDELBROT_UISIMPLENUMBERINPUTWINDOW_HPP
#define VK_MANDELBROT_UISIMPLENUMBERINPUTWINDOW_HPP

#include <string>
#include <concepts>

#include "UiTextBox.hpp"

template<typename T>
requires std::integral<T> || std::floating_point<T>
struct UiSimpleNumberInputWindow final
{
    std::string windowName = "No Name";
    std::string windowText = "No Text";
    uint32_t winW = 500;
    uint32_t winH = 80;
    char input[4096]{};
    T parsedInput{};
    bool forceFocus = true;
    bool isInputOpen = false;

  private:
    UiTextBox _errorWin{};

    bool drawBox();
    bool drawInput();

    friend class Ui;
};

#endif // VK_MANDELBROT_UISIMPLENUMBERINPUTWINDOW_HPP
