#include "HUDWindow.h"

#include "imgui/imgui.h"
#include "ConfigManager.h"
#include "Core/MetricRegistry.h"
static void DrawMetric(const MetricDefinition& metric)
{
    switch (metric.format)
    {
    case MetricFormat::Float:
        ImGui::Text("%s: %.1f", metric.name, metric.value);
        break;

    case MetricFormat::Integer:
        ImGui::Text("%s: %.0f", metric.name, metric.value);
        break;

    case MetricFormat::Milliseconds:
        ImGui::Text("%s: %.0f ms", metric.name, metric.value);
        break;

    case MetricFormat::Time:
        ImGui::Text("%s: %.0f s", metric.name, metric.value);
        break;

    case MetricFormat::Percent:
        ImGui::Text("%s: %.0f%%", metric.name, metric.value);
        break;
    }
}
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
                DrawMetric(metric);
            }
        }

        ImGui::Text("Version 0.1.0");
        ImGui::TextDisabled("Development Build");
    }

    ImGui::End();
}