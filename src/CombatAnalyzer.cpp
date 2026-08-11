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
}

uint64_t CombatAnalyzer::GetEventCount() const
{
    return eventCount;
}