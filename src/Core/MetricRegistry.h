#pragma once
struct MetricDefinition
{
    const char* name;
    bool enabled;
    int order;
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
    

private:
    MetricDefinition fpsMetric = { "FPS", true, 0 };
    float fps = 0.0f;
};