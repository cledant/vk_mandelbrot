#ifndef VK_MANDELBROT_UI_HPP
#define VK_MANDELBROT_UI_HPP

#include <chrono>

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"
#include "UiEvents.hpp"
#include "UiInfoOverview.hpp"
#include "UiSimpleInputWindow.hpp"
#include "UiAboutBox.hpp"
#include "UiTextBox.hpp"
#include "UiNotifications.hpp"
#include "UiMenuBarRendererScaleSelection.hpp"

class Ui final
{
  public:
    Ui() = default;
    ~Ui() = default;
    Ui(Ui const &src) = delete;
    Ui &operator=(Ui const &rhs) = delete;
    Ui(Ui &&src) = delete;
    Ui &operator=(Ui &&rhs) = delete;

    void init(void *win);
    void clear();

    [[nodiscard]] UiEvents const &getUiEvents() const;
    [[nodiscard]] bool isUiHovered() const;

    // Triggers from keyboard
    bool fullscreen = false;
    bool displayUi = true;
    bool showInfoPosition = false;
    bool showInfoFps = false;
    UiTextBox helpBox{};

    // Updated from outside
    UiInfoOverview infoOverview{};
    UiNotifications notifications{};

    // Info for outside
    bool vsync = true;
    UiMenuBarRendererScaleSelection rendererScaleSelection{};

    // Draw related
    void draw();

  private:
    UiAboutBox _aboutBox{};

    // Draw related
    void drawMenuBar();

    // Compute fps
    void computeFps();
    uint64_t _nbFrames{};
    std::chrono::steady_clock::time_point _avgFpsTimeRef;
    std::chrono::steady_clock::time_point _prevFrameTimeRef;

    // Events from ui interaction
    UiEvents _uiEvents{};
};

#endif // VK_MANDELBROT_UI_HPP
