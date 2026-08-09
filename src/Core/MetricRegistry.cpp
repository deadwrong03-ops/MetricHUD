#include "MetricRegistry.h"

MetricRegistry::MetricRegistry()
{
}

void MetricRegistry::Initialize()
{
}

void MetricRegistry::Shutdown()
{
}

void MetricRegistry::SetFPS(float value)
{
    fps = value;
}

float MetricRegistry::GetFPS() const
{
    return fps;
}

const MetricDefinition& MetricRegistry::GetFPSMetric() const
{
    return fpsMetric;
}
void MetricRegistry::SetFPSEnabled(bool enabled)
{
    fpsMetric.enabled = enabled;
}
