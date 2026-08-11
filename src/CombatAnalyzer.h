#pragma once

#include <cstdint>
#include "ArcDPS.h"
#include <vector>

namespace ArcDPS
{
    struct CombatEvent;
}

class CombatAnalyzer
{
public:
    CombatAnalyzer();

    void Initialize();
    void Shutdown();

    void ProcessEvent(const ArcDPS::CombatEvent* event);

    uint64_t GetEventCount() const;
    uint32_t GetLastSkillID() const;
    const std::vector<uint32_t>& GetRecentSkills() const;

private:
    uint64_t eventCount = 0;
    uint32_t lastSkillID = 0;
    std::vector<uint32_t> recentSkills;
};
