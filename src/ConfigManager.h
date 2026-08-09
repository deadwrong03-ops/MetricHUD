#pragma once
class ConfigManager
{
public:
    ConfigManager();

    void Initialize();
    void Shutdown();

    bool ShowHUD() const;
    void SetShowHUD(bool value);
    bool IsLocked() const;
    void SetLocked(bool value);
    int GetVisibleMetricCount() const;

private:
    bool showHUD = true;
    bool locked = false;
    int visibleMetricCount = 5;
};