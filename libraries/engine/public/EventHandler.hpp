#ifndef VK_MANDELBROT_EVENTHANDLER_HPP
#define VK_MANDELBROT_EVENTHANDLER_HPP

#include <chrono>

#include "IOEvents.hpp"
#include "IOManager.hpp"
#include "VulkanRenderer.hpp"
#include "Ui.hpp"

class EventHandler final
{
  public:
    EventHandler() = default;
    ~EventHandler() = default;
    EventHandler(EventHandler const &src) = delete;
    EventHandler &operator=(EventHandler const &rhs) = delete;
    EventHandler(EventHandler &&src) = delete;
    EventHandler &operator=(EventHandler &&rhs) = delete;

    void setIOManager(IOManager *ioManager);
    void setVkRenderer(VulkanRenderer *renderer);
    void setUi(Ui *ui);

    void processEvents();

  private:
    // Timer related
    static constexpr double const SYSTEM_TIMER_SECONDS = 1.0;
    static constexpr double const CONFIG_TIMER_SECONDS = 0.5;
    static constexpr double const ACTION_TIMER_SECONDS = 0.25;
    static constexpr double const FAST_ACTION_TIMER_SECONDS = 0.01;

    enum EventTimersTypes
    {
        ET_SYSTEM = 0,
        ET_CONFIG,
        ET_LEFT_MOUSE,
        ET_MIDDLE_MOUSE,
        ET_RIGHT_MOUSE,
        ET_KEYBOARD_MOVEMENTS,
        ET_NB_EVENT_TIMER_TYPES
    };

    struct EventTimers final
    {
        std::array<uint8_t, ET_NB_EVENT_TIMER_TYPES> acceptEvent{};
        std::array<uint8_t, ET_NB_EVENT_TIMER_TYPES> updated{};
        std::array<std::chrono::steady_clock::time_point,
                   ET_NB_EVENT_TIMER_TYPES>
          timeRef{};
        std::array<double, ET_NB_EVENT_TIMER_TYPES> timerDiff{};
        std::array<double, ET_NB_EVENT_TIMER_TYPES> timerValues = {
            SYSTEM_TIMER_SECONDS,      CONFIG_TIMER_SECONDS,
            FAST_ACTION_TIMER_SECONDS, FAST_ACTION_TIMER_SECONDS,
            FAST_ACTION_TIMER_SECONDS, FAST_ACTION_TIMER_SECONDS
        };
    };

    // Multiplier
    static constexpr uint32_t const ITER_NO_MULTIPLIER = 1;
    static constexpr uint32_t const ITER_WITH_MULTIPLIER = 100;

    static constexpr float const KEYBOARD_MVT_NO_MULTIPLIER = 0.01f;
    static constexpr float const KEYBOARD_MVT_WITH_MULTIPLIER = 0.1f;

    static constexpr float const ZOOM_NO_MULTIPLIER = 2.0f;
    static constexpr float const ZOOM_WITH_MULTIPLIER = 8.0f;

    // Default val
    static constexpr float const DEFAULT_ZOOM = 1.0f;

    // IO Event handling functions
    inline void closeWinEvent();
    inline void toggleFullscreen();
    inline void up();
    inline void down();
    inline void right();
    inline void left();
    inline void resetZoomScreenCenter();
    inline void incIter();
    inline void decIter();
    inline void resetIter();
    inline void displayControlMenu();
    inline void displayUi();
    inline void displayInfo();
    inline void displayFps();
    inline void displayHelp();

    // processEvents subFunctions
    inline void initMultipliers(IOEvents const &ioEvents);
    inline void zoomHandling(IOEvents const &ioEvents, glm::vec2 const &fbSize);
    inline void keyboardMvtHandling();
    inline glm::vec2 computeMouseOffset(glm::vec2 const &mousePos,
                                        glm::ivec2 const &fbSize);
    inline void processIoEvents(IOEvents const &ioEvents);
    inline void processUiEvents(UiEvents const &uiEvents);

    // Pointers to managers
    IOManager *_ioManager{};
    VulkanRenderer *_renderer{};
    Ui *_ui{};

    // Timers
    EventTimers _timers{};

    // Values for mandelbrot computation
    glm::ivec2 _keyboardMvt{};
    uint32_t _iterStepValue{};
    float _keyboardMvtStepValue{};
    float _zoomStepValue{};
    float _screenRatio{};
    float _zoomVal = DEFAULT_ZOOM;
    bool _skipZoomHandling{};
};

#endif // VK_MANDELBROT_EVENTHANDLER_HPP
