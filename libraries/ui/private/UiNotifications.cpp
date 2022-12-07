#include "UiNotifications.hpp"

#include <cstring>

#include "imgui.h"

void
UiNotifications::draw()
{
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = now - lastNotificationTime;
    if (diff.count() >= REMOVE_NOTIFICATION_TIME) {
        memset(numberOfNotifications, 0, sizeof(uint32_t) * UNT_TOTAL);
        return;
    }

    static constexpr char const *NOTIFICATION_TEXT_ARRAY[UNT_TOTAL + 1]{
        "Failed to save screenshot", "Screenshot saved", nullptr
    };
    static constexpr float const WIN_HEIGHT = 30.0f;
    static constexpr ImVec2 const NOTIFICATION_TEXT_SIZE[UNT_TOTAL]{
        { 230, WIN_HEIGHT }, { 170, WIN_HEIGHT }
    };
    static constexpr float const PADDING_HEIGHT = 10.0f;
    static constexpr ImGuiWindowFlags const WIN_FLAGS =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
      ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
    static constexpr ImVec2 const WIN_POS_PIVOT = { 0.5f, 0.0f };
    static constexpr float const WIN_ALPHA = 0.35f;

    uint32_t nbNotification{};
    ImGuiViewport const *viewport = ImGui::GetMainViewport();
    float menuBarHeight = viewport->Size.y - viewport->WorkSize.y;
    for (uint32_t i = 0; i < UNT_TOTAL; ++i) {
        if (numberOfNotifications[i]) {
            ImVec2 windowPos{ (viewport->WorkSize.x / 2),
                              (menuBarHeight + PADDING_HEIGHT) +
                                WIN_HEIGHT * nbNotification };
            char winName[256]{};

            snprintf(winName, 256, "Notification_%u", i);
            ImGui::SetNextWindowSize(NOTIFICATION_TEXT_SIZE[i]);
            ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, WIN_POS_PIVOT);
            ImGui::SetNextWindowBgAlpha(WIN_ALPHA);
            if (ImGui::Begin(winName, nullptr, WIN_FLAGS)) {
                if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootWindow) &&
                    ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    numberOfNotifications[i] = 0;
                } else {
                    char displayedText[256]{};

                    if (numberOfNotifications[i] == 1) {
                        snprintf(
                          displayedText, 256, "%s", NOTIFICATION_TEXT_ARRAY[i]);
                    } else {
                        snprintf(displayedText,
                                 256,
                                 "(%u) %s",
                                 numberOfNotifications[i],
                                 NOTIFICATION_TEXT_ARRAY[i]);
                    }
                    ImGui::SetCursorPosX(
                      (NOTIFICATION_TEXT_SIZE[i].x -
                       ImGui::CalcTextSize(displayedText).x) *
                      0.5f);
                    ImGui::Text("%s", displayedText);
                }
                ImGui::End();
            }
            ++nbNotification;
        }
    }
}
