#include "Ui.hpp"

#include <chrono>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include "UiTexts.hpp"
#include "UiSimpleNumberInputWindowPrivate.hpp"

void
Ui::init(void *win)
{
    assert(win);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow *>(win), true);
    _avgFpsTimeRef = std::chrono::steady_clock::now();
    // Init help
    helpBox.windowName = UiTexts::INPUT_HELP_WIN_NAME;
    helpBox.windowText = UiTexts::INPUT_HELP_WIN_TEXT;
    // Init nb iteration input
    iterationInput.windowName = UiTexts::INPUT_ITERATION_WIN_NAME;
    iterationInput.windowText = UiTexts::INPUT_ITERATION_WIN_TEXT;
    iterationInput._errorWin.windowName = UiTexts::INPUT_ERROR_TEXT;
    iterationInput.winW = 300;
}

void
Ui::clear()
{
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

UiEvents const &
Ui::getUiEvents() const
{
    return (_uiEvents);
}

bool
Ui::isUiHovered() const
{
    return (ImGui::IsAnyItemHovered() ||
            ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow));
}

void
Ui::draw()
{
    computeFps();
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (!displayUi) {
        ImGui::Render();
        return;
    }

    _uiEvents = {};
    drawMenuBar();
    infoOverview.draw(showInfoFps, showInfoPosition);
    notifications.draw();
    _uiEvents.events[UET_NB_ITERATION] = iterationInput.drawInput();
    _aboutBox.draw();
    helpBox.draw();
    ImGui::Render();
}

void
Ui::drawMenuBar()
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            _uiEvents.events[UET_SAVE_TO_FILE] =
              ImGui::MenuItem("Save fractal to file", "F10");
            ImGui::Separator();
            _uiEvents.events[UET_EXIT] = ImGui::MenuItem("Exit", "F12");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Display UI", "F2", &displayUi);
            ImGui::Separator();
            ImGui::MenuItem("Info", "F3", &showInfoPosition);
            ImGui::Separator();
            ImGui::MenuItem("Show Fps", "F4", &showInfoFps);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Renderer")) {
            if ((_uiEvents.events[UET_VSYNC] =
                   ImGui::MenuItem("Vsync", "", &vsync))) {
            }
            ImGui::Separator();
            if ((_uiEvents.events[UET_FULLSCREEN] =
                   ImGui::MenuItem("Fullscreen", "F11", &fullscreen))) {
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Set number of iteration")) {
                iterationInput.isInputOpen = !iterationInput.isInputOpen;
            }
            ImGui::Separator();
            _uiEvents.events[UET_RENDERER_SCALE] =
              rendererScaleSelection.drawMenu();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("Help", "F1")) {
                helpBox.isOpen = !helpBox.isOpen;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("About")) {
                _aboutBox.isOpen = !_aboutBox.isOpen;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void
Ui::computeFps()
{
    ++_nbFrames;
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff_frame = now - _prevFrameTimeRef;
    infoOverview.currentFps = 1.0f / diff_frame.count();
    _prevFrameTimeRef = now;

    std::chrono::duration<double> diff_avg = now - _avgFpsTimeRef;
    if (diff_avg.count() > 1.0f) {
        infoOverview.avgFps = _nbFrames;
        _nbFrames = 0;
        _avgFpsTimeRef = now;
    }
}
