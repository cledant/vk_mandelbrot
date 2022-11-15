#ifndef VK_MANDELBROT_UIABOUTBOX_HPP
#define VK_MANDELBROT_UIABOUTBOX_HPP

struct UiAboutBox final
{
    bool isOpen = false;

  private:
    void draw();

    friend class Ui;
};

#endif // VK_MANDELBROT_UIABOUTBOX_HPP
