#ifndef VK_MANDELBROT_UIEVENTS_HPP
#define VK_MANDELBROT_UIEVENTS_HPP

enum UiEventTypes
{
    UET_EXIT = 0,
    UET_FULLSCREEN,
    UET_VSYNC,
    UET_SAVE_TO_FILE,
    UET_TOTAL_NB,
};

struct UiEvents
{
    bool events[UET_TOTAL_NB] = { false };
};

#endif // VK_MANDELBROT_UIEVENTS_HPP
