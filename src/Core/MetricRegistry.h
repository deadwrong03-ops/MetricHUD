#pragma once

class MetricRegistry
{
public:
    MetricRegistry();

    void Initialize();
    void Shutdown();

    void SetFPS(float value);
    float GetFPS() const;

private:
    float fps = 0.0f;
};