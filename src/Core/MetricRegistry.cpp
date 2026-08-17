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
            "",
            MetricFormat::Milliseconds
        });
    metrics.push_back(
        {
            MetricID::LastFightDPS,
            "Last Fight DPS",
            false,
            7,
            0.0f,
            "",
            MetricFormat::Integer
        });
    metrics.push_back(
        {
            MetricID::LastFightTime,
            "Last Fight Time",
            false,
            8,
            0.0f,
            "",
            MetricFormat::Time
        });

    metrics.push_back(
        {
            MetricID::CombatTime,
            "Combat Time",
            true,
            2,
            0.0f,
            "",
            MetricFormat::Time
        });
    metrics.push_back(
        {
            MetricID::DPS,
            "DPS",
            true,
            3,
            0.0f,
            "",
            MetricFormat::Integer
        });

    metrics.push_back(
        {
            MetricID::MapName,
            "Map",
            true,
            3,
            0.0f,
            "Unknown",
            MetricFormat::Text
        });

    metrics.push_back(
        {
            MetricID::MapID,
            "Map ID",
            true,
            4,
            0.0f,
            "",
            MetricFormat::Integer
        });
    metrics.push_back(
        {
            MetricID::CharacterName,
            "Character",
            true,
            5,
            0.0f,
            "Unknown",
            MetricFormat::Text
        });
    metrics.push_back(
        {
            MetricID::PlayerSpeed,
            "Speed",
            true,
            6,
            0.0f,
            "",
            MetricFormat::Float
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
void MetricRegistry::SetDPS(double value)
{
    dps = value;
    SetMetricValue(MetricID::DPS, static_cast<float>(value));
}

double MetricRegistry::GetDPS() const
{
    return dps;
}
void MetricRegistry::SetLastFightDPS(double value)
{
    lastFightDPS = value;
    SetMetricValue(MetricID::LastFightDPS, static_cast<float>(value));
}

double MetricRegistry::GetLastFightDPS() const
{
    return lastFightDPS;
}
void MetricRegistry::SetLastFightTime(double seconds)
{
    lastFightTime = seconds;
    SetMetricValue(MetricID::LastFightTime, static_cast<float>(seconds));
}

double MetricRegistry::GetLastFightTime() const
{
    return lastFightTime;
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
void MetricRegistry::SetMetricText(MetricID id, const char* text)
{
    MetricDefinition* metric = GetMetric(id);

    if (metric != nullptr)
    {
        metric->textValue = text;
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
