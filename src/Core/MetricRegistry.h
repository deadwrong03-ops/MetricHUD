#pragma once
#include <vector>
enum class MetricID
{
    FPS,
    Ping
};
enum class MetricFormat
{
    Float,
    Integer,
    Milliseconds,
    Time,
    Percent
};
struct MetricDefinition
{
    MetricID id;
    const char* name;
    bool enabled;
    int order;
    float value;
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
    const MetricDefinition& GetFPSMetric() const;
    void SetFPSEnabled(bool enabled);
    const std::vector<MetricDefinition>& GetMetrics() const;
    MetricDefinition* GetMetric(MetricID id);
    void SetMetricValue(MetricID id, float value);
    void SetMetricEnabled(MetricID id, bool enabled);
    

private:
    std::vector<MetricDefinition> metrics;
    MetricDefinition fpsMetric =
    {
        MetricID::FPS,
        "FPS",
        true,
        0,
        0.0f,
        MetricFormat::Float
    };
    
};