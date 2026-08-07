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

bool ConfigManager::IsLocked() const
{
    return locked;
}

int ConfigManager::GetVisibleMetricCount() const
{
    return visibleMetricCount;
}