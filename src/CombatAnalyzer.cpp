#include "CombatAnalyzer.h"
#include "ArcDPS.h"

CombatAnalyzer::CombatAnalyzer()
{
}

void CombatAnalyzer::Initialize()
{
    eventCount = 0;
}

void CombatAnalyzer::Shutdown()
{
}

void CombatAnalyzer::ProcessEvent(const ArcDPS::CombatEvent* event)
{
    if (event == nullptr)
    {
        return;
    }

    eventCount++;

    if (event->SkillID != 0)
    {
        lastSkillID = event->SkillID;
    }
}

uint64_t CombatAnalyzer::GetEventCount() const
{
    return eventCount;
}

uint32_t CombatAnalyzer::GetLastSkillID() const
{
    return lastSkillID;
}