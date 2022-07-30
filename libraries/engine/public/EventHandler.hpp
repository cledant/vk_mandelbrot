#ifndef VK_MANDELBROT_VULKAN_EVENTHANDLER_HPP
#define VK_MANDELBROT_VULKAN_EVENTHANDLER_HPP

#include <chrono>

#include "IOEvents.hpp"
#include "IOManager.hpp"
#include "VulkanRenderer.hpp"

class EventHandler final
{
  public:
    EventHandler() = default;
    ~EventHandler() = default;
    EventHandler(EventHandler const &src) = delete;
    EventHandler &operator=(EventHandler const &rhs) = delete;
    EventHandler(EventHandler &&src) = delete;
    EventHandler &operator=(EventHandler &&rhs) = delete;

    void setIOManager(IOManager *io_manager);
    void setVkRenderer(VulkanRenderer *renderer);

    void processEvents(IOEvents const &ioEvents);

  private:
    // Timer related
    static constexpr double const SYSTEM_TIMER_SECONDS = 1.0;
    static constexpr double const CONFIG_TIMER_SECONDS = 0.5;
    static constexpr double const ACTION_TIMER_SECONDS = 0.5;
    static constexpr double const FAST_ACTION_TIMER_SECONDS = 0.01;

    enum EventTimersTypes
    {
        ET_SYSTEM = 0,
        ET_CONFIG,
        ET_LEFT_MOUSE,
        ET_MIDDLE_MOUSE,
        ET_RIGHT_MOUSE,
        ET_KEYBOARD_CONTROLS,
        ET_NB_EVENT_TIMER_TYPES
    };

    struct EventTimers final
    {
        std::array<uint8_t, ET_NB_EVENT_TIMER_TYPES> accept_event{};
        std::array<uint8_t, ET_NB_EVENT_TIMER_TYPES> updated{};
        std::array<std::chrono::steady_clock::time_point,
                   ET_NB_EVENT_TIMER_TYPES>
          time_ref{};
        std::array<double, ET_NB_EVENT_TIMER_TYPES> timer_diff{};
        std::array<double, ET_NB_EVENT_TIMER_TYPES> timer_values = {
            SYSTEM_TIMER_SECONDS,      CONFIG_TIMER_SECONDS,
            FAST_ACTION_TIMER_SECONDS, FAST_ACTION_TIMER_SECONDS,
            FAST_ACTION_TIMER_SECONDS, ACTION_TIMER_SECONDS
        };
    };

    // IO related
    static constexpr uint32_t const MULTI_ITER_INC_DEC_VALUE = 100;

    // IO Event handling functions
    inline void _closeWinEvent();
    inline void _toggleFullscreen();
    inline void _up();
    inline void _down();
    inline void _right();
    inline void _left();
    inline void _setScreenCenter();
    inline void _resetZoomScreenCenter();
    inline void _singleIncIter();
    inline void _singleDecIter();
    inline void _multipleIncIter();
    inline void _multipleDecIter();
    inline void _resetIter();

    IOManager *_ioManager{};
    VulkanRenderer *_renderer{};

    EventTimers _timers{};

    glm::ivec3 _movements{};
    glm::vec2 _mousePos{};
    glm::vec2 _previousMousePos{};
    bool _mousePosSkip = true;
};

#endif // VK_MANDELBROT_VULKAN_EVENTHANDLER_HPP
