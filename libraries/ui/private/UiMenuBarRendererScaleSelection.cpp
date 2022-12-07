#include "UiMenuBarRendererScaleSelection.hpp"

#include <cstring>

bool
UiMenuBarRendererScaleSelection::drawMenu()
{
    bool ret = false;
    if (ImGui::BeginMenu("Scale")) {
        uint32_t i = 0;
        while (true) {
            auto clicked = itemInMenu(i);

            if (!ret && clicked) {
                ret = true;
            }
            ++i;
            if (i >= URSV_TOTAL_NB) {
                break;
            }
            ImGui::Separator();
        }
        ImGui::EndMenu();
    }
    return (ret);
}

bool
UiMenuBarRendererScaleSelection::itemInMenu(uint32_t index)
{
    if (ImGui::MenuItem(INDEX_STR[index], NULL, indexActivity[index])) {
        memset(indexActivity, 0, URSV_TOTAL_NB * sizeof(bool));
        activeIndex = index;
        indexActivity[activeIndex] = true;
        return (true);
    }
    return (false);
}
