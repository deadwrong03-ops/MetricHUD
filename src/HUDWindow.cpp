#include "HUDWindow.h"

#include "imgui/imgui.h"
#include "ConfigManager.h"

HUDWindow::HUDWindow()
{
}

void HUDWindow::Render(ConfigManager& config)
{
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;

    if (config.IsLocked())
    {
        windowFlags |= ImGuiWindowFlags_NoMove;
    }

    if (ImGui::Begin("MetricHUD", nullptr, windowFlags))
    {
        ImGui::Text("MetricHUD");
        ImGui::Separator();
        ImGui::Text("Version 0.1.0");
        ImGui::TextDisabled("Development Build");
    }

    ImGui::End();
}