#include "HUDWindow.h"

#include "imgui/imgui.h"

HUDWindow::HUDWindow()
{
}

void HUDWindow::Render()
{
    if (ImGui::Begin("MetricHUD"))
    {
        ImGui::Text("MetricHUD");
        ImGui::Separator();
        ImGui::Text("Version 0.1.0");
        ImGui::TextDisabled("Development Build");
    }

    ImGui::End();
}