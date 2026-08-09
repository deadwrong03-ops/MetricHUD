#pragma once

class ConfigManager;
class MetricRegistry;

class HUDWindow
{
public:
    HUDWindow();

    void Render(ConfigManager& config, MetricRegistry& metrics);
    
};
