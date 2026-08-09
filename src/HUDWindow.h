#pragma once

class ConfigManager;

class HUDWindow
{
public:
    HUDWindow();

    void Render(ConfigManager& config);
};