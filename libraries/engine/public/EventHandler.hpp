#ifndef VK_MANDELBROT_EVENTHANDLER_HPP
#define VK_MANDELBROT_EVENTHANDLER_HPP

#include <chrono>
#include <future>

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
    static constexpr double const SCREENSHOT_TIMER_SECONDS = 0.35;

    enum EventTimersTypes
    {
        ET_SYSTEM = 0,
        ET_CONFIG,
        ET_LEFT_MOUSE,
        ET_MIDDLE_MOUSE,
        ET_RIGHT_MOUSE,
        ET_KEYBOARD_MOVEMENTS,
        ET_SCREENSHOT,
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
            FAST_ACTION_TIMER_SECONDS, FAST_ACTION_TIMER_SECONDS,
            SCREENSHOT_TIMER_SECONDS
        };
    };

    // Multiplier
    static constexpr uint32_t const ITER_NO_MULTIPLIER = 1;
    static constexpr uint32_t const ITER_WITH_MULTIPLIER = 100;

    static constexpr double const KEYBOARD_MVT_NO_MULTIPLIER = 0.01;
    static constexpr double const KEYBOARD_MVT_WITH_MULTIPLIER = 0.1;

    static constexpr double const ZOOM_MULTIPLIER = 2.0;

    // Default val
    static constexpr double const DEFAULT_ZOOM = 1.0;

    // IO Events handling functions
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
    inline void takeScreenshot();

    // Ui Events handling functions
    inline void uiCloseWinEvent();
    inline void uiToggleFullscreen();
    inline void uiToggleVsync();
    inline void uiSaveFractalToFile();

    // processEvents subFunctions
    inline void initMultipliers(IOEvents const &ioEvents);
    inline void zoomHandling(IOEvents const &ioEvents, glm::vec2 const &fbSize);
    inline void keyboardMvtHandling();
    inline void recreateSwapchain();
    static inline double computeMouseOffset(double mousePos,
                                     double fbSize,
                                     double zoom);
    inline void processIoEvents(IOEvents const &ioEvents);
    inline void processUiEvents(UiEvents const &uiEvents);
    inline void setUiInfoValues();

    // Screenshot related functions
    inline void screenshotHandling();
    [[nodiscard]] static inline std::string generateScrenshotName(
      std::string const &folderPath);
    static inline bool saveScreenshotHelper(VulkanScreenshot &&screenshot,
                                            std::string &&filepath);

    // Pointers to managers
    IOManager *_ioManager{};
    VulkanRenderer *_renderer{};
    Ui *_ui{};

    // Timers
    EventTimers _timers{};

    // Values for mandelbrot computation
    glm::ivec2 _keyboardMvt{};
    uint32_t _iterStepValue{};
    double _keyboardMvtStepValue{};
    double _screenRatio{};
    double _zoomVal = DEFAULT_ZOOM;

    // Other
    bool _skipZoomHandling{};
    bool _recreateSwapchain{};
    bool _vsync = true;
    bool _saveScreenshotTofile{};
    std::vector<std::future<bool>> _screenshotsResults;
};

#endif // VK_MANDELBROT_EVENTHANDLER_HPP
