#include "UiInfoOverview.hpp"

#include "imgui.h"

void
UiInfoOverview::draw(bool &fps, bool &info) const
{
    static constexpr float const PADDING = 10.0f;
    static constexpr ImGuiWindowFlags const WIN_FLAGS =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
      ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
    static constexpr ImVec2 const WIN_SIZE_INFO = ImVec2(435, 105);
    static constexpr ImVec2 const WIN_SIZE_FPS = ImVec2(165, 50);
    static constexpr ImVec2 const WIN_SIZE_BOTH = ImVec2(435, 140);
    static constexpr ImVec2 const WIN_POS_PIVOT = { 1.0f, 0.0f };
    static constexpr float const WIN_ALPHA = 0.35f;
    static constexpr ImVec4 const RED = { 255, 0, 0, 255 };
    static constexpr ImVec4 const YELLOW = { 255, 255, 0, 255 };
    static constexpr ImVec4 const GREEN = { 0, 255, 0, 255 };

    if (info || fps) {
        ImGuiViewport const *viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos;
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos{ (work_pos.x + work_size.x - PADDING),
                           (work_pos.y + PADDING) };

        if (fps && info) {
            ImGui::SetNextWindowSize(WIN_SIZE_BOTH);
        } else if (fps) {
            ImGui::SetNextWindowSize(WIN_SIZE_FPS);
        } else {
            ImGui::SetNextWindowSize(WIN_SIZE_INFO);
        }
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, WIN_POS_PIVOT);
        ImGui::SetNextWindowBgAlpha(WIN_ALPHA);
        if (ImGui::Begin("Info Overview", nullptr, WIN_FLAGS)) {
            if (fps) {
                ImGui::Text("Current FPS: ");
                if (avgFps <= 20.0f) {
                    ImGui::PushStyleColor(ImGuiCol_Text, RED);
                } else if (avgFps <= 40.0f) {
                    ImGui::PushStyleColor(ImGuiCol_Text, YELLOW);
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Text, GREEN);
                }
                ImGui::SameLine();
                ImGui::Text("%.1f\n", currentFps);
                ImGui::PopStyleColor();
                ImGui::Text("Avg FPS: ");
                if (avgFps <= 20.0f) {
                    ImGui::PushStyleColor(ImGuiCol_Text, RED);
                } else if (avgFps <= 40.0f) {
                    ImGui::PushStyleColor(ImGuiCol_Text, YELLOW);
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Text, GREEN);
                }
                ImGui::SameLine();
                ImGui::Text("%.1f\n", avgFps);
                ImGui::PopStyleColor();
            }
            if (fps && info) {
                ImGui::Separator();
            }
            if (info) {
                ImGui::Text("Camera position: X = %.30f", cameraPos.x);
                ImGui::Text("Camera position: Y = %.30f", cameraPos.y);
                ImGui::Text("Iteration: %u", maxIteration);
                ImGui::Text("Zoom : %.30e", 1.0f / zoom);
                ImGui::Text("Renderer scale : %.2f", renderScale);
            }
            ImGui::End();
        }
    }
}
