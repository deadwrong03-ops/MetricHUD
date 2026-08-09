#pragma once
#include <vector>
enum class MetricID
{
    FPS
};
struct MetricDefinition
{
    MetricID id;
    const char* name;
    bool enabled;
    int order;
    float value;
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
    

private:
    std::vector<MetricDefinition> metrics;
    MetricDefinition fpsMetric =
    {
        MetricID::FPS,
        "FPS",
        true,
        0,
        0.0f
    };
    
};