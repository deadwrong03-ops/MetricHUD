#include "ConfigManager.h"

ConfigManager::ConfigManager()
{
}

void ConfigManager::Initialize()
{
}

void ConfigManager::Shutdown()
{
}

bool ConfigManager::ShowHUD() const
{
    return showHUD;
}

void ConfigManager::SetShowHUD(bool value)
{
    showHUD = value;
}

bool ConfigManager::IsLocked() const
{
    return locked;
}

void ConfigManager::SetLocked(bool value)
{
    locked = value;
}

int ConfigManager::GetVisibleMetricCount() const
{
    return visibleMetricCount;
}
