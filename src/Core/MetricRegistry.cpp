#include "MetricRegistry.h"

MetricRegistry::MetricRegistry()
{
}

void MetricRegistry::Initialize()
{
    metrics.clear();
    metrics.push_back(fpsMetric);
}

void MetricRegistry::Shutdown()
{
}
void MetricRegistry::SetFPS(float value)
{
    fpsMetric.value = value;

    if (!metrics.empty())
    {
        metrics[0].value = value;
    }
}

float MetricRegistry::GetFPS() const
{
    return fpsMetric.value;
}

const MetricDefinition& MetricRegistry::GetFPSMetric() const
{
    return fpsMetric;
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
