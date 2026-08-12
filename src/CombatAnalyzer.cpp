#include "CombatAnalyzer.h"
#include "ArcDPS.h"

CombatAnalyzer::CombatAnalyzer()
{
}
void CombatAnalyzer::Initialize()
{
    eventCount = 0;
    damageEventCount = 0;
    totalDirectDamage = 0;
    lastSkillID = 0;
    lastActivation = 0;
    recentSkills.clear();
    recentRecords.clear();
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

    CombatRecord record;

    record.time = event->Time;
    record.skillID = event->SkillID;
    record.value = event->Value;
    record.buffDamage = event->BuffDamage;
    record.isActivation = event->IsActivation;
    record.isBuffRemove = event->IsBuffRemove;
    record.isNinety = event->IsNinety;
    record.isFifty = event->IsFifty;
    record.isMoving = event->IsMoving;

    recentRecords.push_back(record);

    if (recentRecords.size() > 20)
    {
        recentRecords.erase(recentRecords.begin());
    }
    eventCount++;
    if (event->SkillID != 0 &&
        event->Value < 0 &&
        event->BuffDamage == 0)
    {
        damageEventCount++;
        totalDirectDamage += -static_cast<int64_t>(event->Value);
    }

    if (event->IsActivation != 0)
    {
        lastActivation = event->IsActivation;
    }

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
uint64_t CombatAnalyzer::GetDamageEventCount() const
{
    return damageEventCount;
}
int64_t CombatAnalyzer::GetTotalDirectDamage() const
{
    return totalDirectDamage;
}
uint32_t CombatAnalyzer::GetLastSkillID() const
{
    return lastSkillID;
}

uint8_t CombatAnalyzer::GetLastActivation() const
{
    return lastActivation;
}

const std::vector<uint32_t>& CombatAnalyzer::GetRecentSkills() const
{
    return recentSkills;
}
const std::vector<CombatRecord>& CombatAnalyzer::GetRecentRecords() const
{
    return recentRecords;
}