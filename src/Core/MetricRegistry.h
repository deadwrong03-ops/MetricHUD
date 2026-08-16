#pragma once

#include <vector>
#include <string>

enum class MetricID
{
    FPS,
    Ping,
    CombatTime,
    DPS,
    LastFightDPS,
    MapName,
    MapID,
    CharacterName,
    PlayerSpeed
    
};

enum class MetricFormat
{
    Float,
    Integer,
    Milliseconds,
    Time,
    Percent,
    Text
};
struct MetricDefinition
{
    MetricID id;
    const char* name;
    bool enabled;
    int order;
    float value;
    std::string textValue;
    MetricFormat format;
};

class MetricRegistry
{
public:
    MetricRegistry();

    void Initialize();
    void Shutdown();

    void SetFPS(float value);
    float GetFPS() const;
    void SetDPS(double dps);
    double GetDPS() const;
    void SetLastFightDPS(double dps);
    double GetLastFightDPS() const;
    const MetricDefinition& GetFPSMetric() const;
    void SetFPSEnabled(bool enabled);

    const std::vector<MetricDefinition>& GetMetrics() const;
    MetricDefinition* GetMetric(MetricID id);
    void SetMetricValue(MetricID id, float value);
    void SetMetricText(MetricID id, const char* text);
    void SetMetricEnabled(MetricID id, bool enabled);

private:
    std::vector<MetricDefinition> metrics;
    double dps = 0.0;
    double lastFightDPS = 0.0;
    MetricDefinition fpsMetric =
    {
        MetricID::FPS,
        "FPS",
        true,
        1,
        0.0f,
        "",
        MetricFormat::Float
    };
};