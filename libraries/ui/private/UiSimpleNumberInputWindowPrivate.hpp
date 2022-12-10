#ifndef VK_MANDELBROT_UISIMPLENUMBERINPUTWINDOWPRIVATE_HPP
#define VK_MANDELBROT_UISIMPLENUMBERINPUTWINDOWPRIVATE_HPP

#include "UiSimpleNumberInputWindow.hpp"

#include "imgui.h"
#include "fmt/core.h"

template<typename T>
requires std::integral<T> || std::floating_point<T>
bool
UiSimpleNumberInputWindow<T>::drawBox()
{
    static constexpr ImGuiWindowFlags const WIN_FLAGS =
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;
    static constexpr ImVec2 const WIN_POS_PIVOT = { 0.5f, 0.5f };
    static constexpr ImGuiInputTextFlags const INPUT_TEXT_FLAGS =
      ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_NoUndoRedo;

    if (!isInputOpen) {
        return (false);
    }
    _errorWin.isOpen = false;

    ImGuiViewport const *viewport = ImGui::GetMainViewport();
    auto viewport_center = viewport->GetCenter();
    ImVec2 window_pos{ viewport_center.x, viewport_center.y };
    bool trigger = false;

    ImGui::SetNextWindowSize(ImVec2(winW, winH));
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, WIN_POS_PIVOT);
    ImGui::Begin(windowName.c_str(), &isInputOpen, WIN_FLAGS);

    bool keyboard_input_ended = ImGui::InputText(
      windowText.c_str(), input, IM_ARRAYSIZE(input), INPUT_TEXT_FLAGS);
    if (forceFocus) {
        ImGui::SetKeyboardFocusHere(0);
        forceFocus = false;
    }
    if (ImGui::IsItemDeactivated() &&
        ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape))) {
        isInputOpen = false;
        forceFocus = true;
    }
    bool ok_pressed = ImGui::Button("Ok");
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
        isInputOpen = false;
        forceFocus = true;
    }
    if (keyboard_input_ended || ok_pressed) {
        trigger = true;
        isInputOpen = false;
        forceFocus = true;
    }
    ImGui::End();
    return (trigger);
}

template<>
bool
UiSimpleNumberInputWindow<uint32_t>::drawInput()
{
    bool hasParsedValue = false;

    if (drawBox()) {
        try {
            auto parsedNbIteration = std::stoul(input);
            if (parsedNbIteration <= UINT32_MAX) {
                parsedInput = parsedNbIteration;
                hasParsedValue = true;
            } else {
                isInputOpen = false;
                _errorWin.isOpen = true;
                _errorWin.windowText = fmt::format(
                  "{} should be between {} and {}", windowText, 0, UINT32_MAX);
            }
        } catch (std::exception const &e) {
            isInputOpen = false;
            _errorWin.isOpen = true;
            _errorWin.windowText = "Invalid number";
        }
    }
    _errorWin.draw();
    return (hasParsedValue);
}

#endif // VK_MANDELBROT_UISIMPLENUMBERINPUTWINDOWPRIVATE_HPP