#include "CombatAnalyzer.h"
#include "ArcDPS.h"

CombatAnalyzer::CombatAnalyzer()
{
}

void CombatAnalyzer::Initialize()
{
    eventCount = 0;
    lastSkillID = 0;
    recentSkills.clear();
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
        recentSkills.push_back(event->SkillID);

        if (recentSkills.size() > 10)
        {
            recentSkills.erase(recentSkills.begin());
        }
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

const std::vector<uint32_t>& CombatAnalyzer::GetRecentSkills() const
{
    return recentSkills;
}