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
    firstDamageTime = 0;
    lastDamageTime = 0;
    lastSkillID = 0;
    lastActivation = 0;
    recentSkills.clear();
    recentRecords.clear();
}
void CombatAnalyzer::ResetSession()
{
    eventCount = 0;
    damageEventCount = 0;
    totalDirectDamage = 0;
    firstDamageTime = 0;
    lastDamageTime = 0;
    lastSkillID = 0;
    lastActivation = 0;
    recentSkills.clear();
    recentRecords.clear();
    skillUseCounts.clear();
    lastSkillEventTimes.clear();
}
void CombatAnalyzer::CaptureLastFight()
{
    lastFightDamage = totalDirectDamage;
    lastFightDurationSeconds = GetCombatDurationSeconds();
    lastFightDPS = GetDPS();
}

void CombatAnalyzer::Shutdown()
{
}

void CombatAnalyzer::ProcessEvent(
    const ArcDPS::CombatEvent* event,
    const char* skillName)
{
    if (event == nullptr)
    {
        return;
    }

    if (event->SkillID != 0 &&
        skillName != nullptr &&
        skillName[0] != '\0')
    {
        skillNames[event->SkillID] = skillName;
    }
    if (event->SkillID != 0)
    {
        const uint64_t currentTime = event->Time;
        const uint64_t lastTime = lastSkillEventTimes[event->SkillID];

        if (lastTime == 0 || currentTime - lastTime > 50)
        {
            skillUseCounts[event->SkillID]++;
        }

        lastSkillEventTimes[event->SkillID] = currentTime;
    }

    // existing eventCount / damage / record code continues here

    CombatRecord record;

    record.time = event->Time;
    record.skillID = event->SkillID;
    record.sourceAgent = event->SourceAgent;
    record.sourceInstanceID = event->SourceInstanceID;
    record.sourceMasterInstanceID = event->SrcMasterInstanceID;
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

        if (firstDamageTime == 0)
        {
            firstDamageTime = event->Time;
        }

        lastDamageTime = event->Time;
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
double CombatAnalyzer::GetCombatDurationSeconds() const
{
    if (firstDamageTime == 0 || lastDamageTime <= firstDamageTime)
    {
        return 0.0;
    }

    return static_cast<double>(lastDamageTime - firstDamageTime) / 1000.0;
}
double CombatAnalyzer::GetDPS() const
{
    const double duration = GetCombatDurationSeconds();

    if (duration <= 0.0)
    {
        return 0.0;
    }

    return static_cast<double>(totalDirectDamage) / duration;
}

int64_t CombatAnalyzer::GetLastFightDamage() const
{
    return lastFightDamage;
}

double CombatAnalyzer::GetLastFightDurationSeconds() const
{
    return lastFightDurationSeconds;
}

double CombatAnalyzer::GetLastFightDPS() const
{
    return lastFightDPS;
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
std::string CombatAnalyzer::GetSkillName(uint32_t skillID) const
{
    auto it = skillNames.find(skillID);

    if (it != skillNames.end())
    {
        return it->second;
    }

    return "Unknown";
}
const std::unordered_map<uint32_t, uint32_t>& CombatAnalyzer::GetSkillUseCounts() const
{
    return skillUseCounts;
}