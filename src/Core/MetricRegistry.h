#pragma once

#include <vector>
#include <string>

enum class MetricID
{
    FPS,
    Ping,
    CombatTime,
    DPS,
    Damage,
    DownedCount,
    DeathCount,
    LastFightDPS,
    LastFightTime,
    LastFightDamage,
    LastFightDownedCount,
    LastFightDeathCount,
    MapName,
    MapID,
    CharacterName,
    PlayerSpeed
    
};

enum class MetricFormat
{
    Float,
    Integer,
    Milliseconds,
    Time,
    Percent,
    Text
};
struct MetricDefinition
{
    MetricID id;
    const char* name;
    bool enabled;
    int order;
    float value;
    std::string textValue;
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
    void SetDPS(double dps);
    double GetDPS() const;
    void SetDamage(double damage);
    double GetDamage() const;
    void SetDownedCount(double count);
    double GetDownedCount() const;
    void SetDeathCount(double count);
    double GetDeathCount() const;
    void SetLastFightDPS(double dps);
    double GetLastFightDPS() const;
    void SetLastFightTime(double seconds);
    double GetLastFightTime() const;
    void SetLastFightDamage(double damage);
    double GetLastFightDamage() const;
    void SetLastFightDownedCount(double count);
    double GetLastFightDownedCount() const;

    void SetLastFightDeathCount(double count);
    double GetLastFightDeathCount() const;
    const MetricDefinition& GetFPSMetric() const;
    void SetFPSEnabled(bool enabled);

    const std::vector<MetricDefinition>& GetMetrics() const;
    MetricDefinition* GetMetric(MetricID id);
    void SetMetricValue(MetricID id, float value);
    void SetMetricText(MetricID id, const char* text);
    void SetMetricEnabled(MetricID id, bool enabled);

private:
    std::vector<MetricDefinition> metrics;
    double dps = 0.0;
    double damage = 0.0;
    double downedCount = 0.0;
    double deathCount = 0.0;
    double lastFightDPS = 0.0;
    double lastFightTime = 0.0;
    double lastFightDamage = 0.0;
    double lastFightDownedCount = 0.0;
    double lastFightDeathCount = 0.0;
    MetricDefinition fpsMetric =
    {
        MetricID::FPS,
        "FPS",
        true,
        1,
        0.0f,
        "",
        MetricFormat::Float
    };
};