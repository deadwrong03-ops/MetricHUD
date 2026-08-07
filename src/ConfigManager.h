#pragma once
class ConfigManager
{
public:
    ConfigManager();

    void Initialize();
    void Shutdown();

    bool ShowHUD() const;
    bool IsLocked() const;
    int GetVisibleMetricCount() const;

private:
    bool showHUD = true;
    bool locked = false;
    int visibleMetricCount = 5;
};