#include "HUDWindow.h"

#include "imgui/imgui.h"
#include "ConfigManager.h"
#include "Core/MetricRegistry.h"

HUDWindow::HUDWindow()

{
}

void HUDWindow::Render(ConfigManager& config, MetricRegistry& metrics)
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

        if (metrics.GetFPSMetric().enabled)
        {
            ImGui::Text("FPS: %.1f", metrics.GetFPS());
        }

        ImGui::Text("Version 0.1.0");
        ImGui::TextDisabled("Development Build");
    }

    ImGui::End();
}