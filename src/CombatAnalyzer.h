#pragma once

#include <cstdint>
#include "ArcDPS.h"

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

private:
    uint64_t eventCount = 0;
    uint32_t lastSkillID = 0;
};
