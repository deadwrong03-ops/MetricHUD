#pragma once

#include <cstdint>
#include "ArcDPS.h"
#include <vector>

namespace ArcDPS
{
    struct CombatEvent;
}

struct CombatRecord
{
    uint64_t time = 0;
    uint32_t skillID = 0;
    int32_t value = 0;
    int32_t buffDamage = 0;
    uint8_t isActivation = 0;
    uint8_t isBuffRemove = 0;
    uint8_t isNinety = 0;
    uint8_t isFifty = 0;
    uint8_t isMoving = 0;
};


class CombatAnalyzer
{
public:
    CombatAnalyzer();

    void Initialize();
    void Shutdown();

    void ProcessEvent(const ArcDPS::CombatEvent* event);

    uint64_t GetEventCount() const;
    uint64_t GetDamageEventCount() const;
    uint32_t GetLastSkillID() const;
    uint8_t GetLastActivation() const;
    const std::vector<uint32_t>& GetRecentSkills() const;
    const std::vector<CombatRecord>& GetRecentRecords() const;

private:
    uint64_t eventCount = 0;
    uint64_t damageEventCount = 0;
    uint32_t lastSkillID = 0;
    uint8_t lastActivation = 0;
    std::vector<uint32_t> recentSkills;
    std::vector<CombatRecord> recentRecords;
};
