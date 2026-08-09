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

        for (const auto& metric : metrics.GetMetrics())
        {
            if (metric.enabled)
            {
                ImGui::Text("%s: %.1f", metric.name, metric.value);
            }
        }

        ImGui::Text("Version 0.1.0");
        ImGui::TextDisabled("Development Build");
    }

    ImGui::End();
}