#ifndef VK_MANDELBROT_UITEXTBOX_HPP
#define VK_MANDELBROT_UITEXTBOX_HPP

#include <string>

struct UiTextBox final
{
    bool isOpen = false;
    std::string windowName = "No Name";
    std::string windowText = "No Text";

  private:
    void draw();

    friend class Ui;
};

#endif // VK_MANDELBROT_UITEXTBOX_HPP
