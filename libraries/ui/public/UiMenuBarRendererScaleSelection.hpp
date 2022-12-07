#ifndef VK_MANDELBROT_UIMENUBARRENDERERSCALESELECTION_HPP
#define VK_MANDELBROT_UIMENUBARRENDERERSCALESELECTION_HPP

#include <cstdint>

#include "imgui.h"

struct UiMenuBarRendererScaleSelection final
{
    enum UiRenderScaleValues
    {
        URSV_0_25 = 0,
        URSV_0_50,
        URSV_0_75,
        URSV_1_00,
        URSV_2_00,
        URSV_4_00,
        URSV_TOTAL_NB
    };
    static constexpr float const INDEX_VALUES[URSV_TOTAL_NB] = {
        0.25f, 0.50f, 0.75f, 1.00f, 2.00f, 4.00f,
    };
    static constexpr char const *INDEX_STR[URSV_TOTAL_NB] = {
        "x0.25", "x0.50", "x0.75", "x1.00", "x2.00", "x4.00",
    };

    uint32_t activeIndex = URSV_1_00;
    bool indexActivity[URSV_TOTAL_NB] = { false, false, false,
                                          true,  false, false };

  private:
    bool drawMenu();
    inline bool itemInMenu(uint32_t index);

    friend class Ui;
};

#endif // VK_MANDELBROT_UIMENUBARRENDERERSCALESELECTION_HPP
