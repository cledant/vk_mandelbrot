#ifndef VK_MANDELBROT_UINOTIFICATIONS_HPP
#define VK_MANDELBROT_UINOTIFICATIONS_HPP

#include <chrono>

enum UiNotificationTypes
{
    UNT_SAVE_SCREENSHOT_FAILED = 0,
    UNT_SAVE_SCREENSHOT_OK,
    UNT_TOTAL
};

struct UiNotifications final
{
    uint32_t numberOfNotifications[UNT_TOTAL]{};
    std::chrono::steady_clock::time_point lastNotificationTime{};

    static constexpr double REMOVE_NOTIFICATION_TIME = 3.0;

  private:
    void draw();

    friend class Ui;
};

#endif // VK_MANDELBROT_UINOTIFICATIONS_HPP
