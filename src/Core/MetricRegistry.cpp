#include "MetricRegistry.h"

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
            1,
            42.0f,
            MetricFormat::Milliseconds
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
