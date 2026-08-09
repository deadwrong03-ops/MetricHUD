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
    
    fpsMetric.value = value;

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
}
