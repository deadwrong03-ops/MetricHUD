#include "MetricRegistry.h"
#include <algorithm>

MetricRegistry::MetricRegistry()
{
}

void MetricRegistry::Initialize()
{
    metrics.clear();

    metrics.push_back(fpsMetric);

    metrics.push_back(
        {
            MetricID::Ping,
            "Ping",
            true,
            0,
            42.0f,
            MetricFormat::Milliseconds
        });

    metrics.push_back(
        {
            MetricID::CombatTime,
            "Combat Time",
            true,
            2,
            0.0f,
            MetricFormat::Time
        });

    std::sort(metrics.begin(), metrics.end(),
        [](const MetricDefinition& a, const MetricDefinition& b)
        {
            return a.order < b.order;
        });
}

void MetricRegistry::Shutdown()
{
}
void MetricRegistry::SetFPS(float value)
{
    fpsMetric.value = value;
    SetMetricValue(MetricID::FPS, value);
}

float MetricRegistry::GetFPS() const
{
    return fpsMetric.value;
}

const MetricDefinition& MetricRegistry::GetFPSMetric() const
{
    return fpsMetric;
}
MetricDefinition* MetricRegistry::GetMetric(MetricID id)
{
    for (auto& metric : metrics)
    {
        if (metric.id == id)
        {
            return &metric;
        }
    }

    return nullptr;
}
void MetricRegistry::SetMetricValue(MetricID id, float value)
{
    MetricDefinition* metric = GetMetric(id);

    if (metric != nullptr)
    {
        metric->value = value;
    }
}
void MetricRegistry::SetMetricEnabled(MetricID id, bool enabled)
{
    MetricDefinition* metric = GetMetric(id);

    if (metric != nullptr)
    {
        metric->enabled = enabled;
    }
}
void MetricRegistry::SetFPSEnabled(bool enabled)
{
    fpsMetric.enabled = enabled;

    if (!metrics.empty())
    {
        metrics[0].enabled = enabled;
    }
}
const std::vector<MetricDefinition>& MetricRegistry::GetMetrics() const
{
    return metrics;
}
