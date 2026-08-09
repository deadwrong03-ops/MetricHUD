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