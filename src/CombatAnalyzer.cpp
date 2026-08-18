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
    downedCount = 0;
    deathCount = 0;
    recentSkills.clear();
    recentRecords.clear();
   
}
void CombatAnalyzer::ResetSession()
{
    eventCount = 0;
    damageEventCount = 0;
    zeroSkillDamageEventCount = 0;
    totalDirectDamage = 0;
    totalBuffDamage = 0;
    firstDamageTime = 0;
    lastDamageTime = 0;
    lastSkillID = 0;
    lastActivation = 0;
    downedCount = 0;
    deathCount = 0;
    recentSkills.clear();
    recentRecords.clear();
    skillUseCounts.clear();
    damageBySkill.clear();
    lastSkillEventTimes.clear();
    
    
    
}
void CombatAnalyzer::CaptureLastFight()
{
    lastFightDamage = totalDirectDamage + totalBuffDamage;
    lastFightDurationSeconds = GetArcStartToLastDamageSeconds();
    lastFightDPS = GetDPS();
    lastFightDownedCount = downedCount;
    lastFightDeathCount = deathCount;
}

void CombatAnalyzer::Shutdown()
{
}

void CombatAnalyzer::ProcessEvent(
    const ArcDPS::CombatEvent* event,
    const char* skillName,
    uint64_t eventID)
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

   
    if (event->SkillID != 0 && event->IsActivation == 1)
    {
     
    }
    

    // existing eventCount / damage / record code continues here

    CombatRecord record;

    record.time = event->Time;
    record.eventID = eventID;
    record.skillID = event->SkillID;
    record.sourceAgent = event->SourceAgent;
    record.sourceInstanceID = event->SourceInstanceID;
    record.sourceMasterInstanceID = event->SrcMasterInstanceID;
    record.destinationAgent = event->DestinationAgent;
    record.destinationInstanceID = event->DestinationInstanceID;
    record.value = event->Value;
    record.buffDamage = event->BuffDamage;
    record.overstackValue = event->OverstackValue;
    record.buff = event->Buff;
    record.result = event->Result;
    record.iff = event->IFF;
    record.isActivation = event->IsActivation;
    record.isBuffRemove = event->IsBuffRemove;
    record.isNinety = event->IsNinety;
    record.isFifty = event->IsFifty;
    record.isMoving = event->IsMoving;

    recentRecords.push_back(record);

    if (recentRecords.size() > 100)
    {
        recentRecords.erase(recentRecords.begin());
    }
    eventCount++;
    if (event->SkillID == 0 &&
        (event->Value < 0 || event->BuffDamage < 0))
    {
        zeroSkillDamageEventCount++;
    }

    if (event->SkillID != 0 &&
        event->Value < 0 &&
        event->BuffDamage == 0)
    {
        damageEventCount++;
        totalDirectDamage += -static_cast<int64_t>(event->Value);
        damageBySkill[event->SkillID] += -static_cast<int64_t>(event->Value);

        if (firstDamageTime == 0)
        {
            firstDamageTime = event->Time;
        }

        lastDamageTime = event->Time;
    }
    if (event->SkillID != 0 &&
        event->BuffDamage < 0)
    {
        damageEventCount++;
        totalBuffDamage += -static_cast<int64_t>(event->BuffDamage);
        damageBySkill[event->SkillID] += -static_cast<int64_t>(event->BuffDamage);

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
uint64_t CombatAnalyzer::GetZeroSkillDamageEventCount() const
{
    return zeroSkillDamageEventCount;
}
int64_t CombatAnalyzer::GetTotalDirectDamage() const
{
    return totalDirectDamage;
}
int64_t CombatAnalyzer::GetTotalBuffDamage() const
{
    return totalBuffDamage;
}
double CombatAnalyzer::GetCombatDurationSeconds() const
{
    if (firstDamageTime == 0 || lastDamageTime <= firstDamageTime)
    {
        return 0.0;
    }

    return static_cast<double>(lastDamageTime - firstDamageTime) / 1000.0;
}


double CombatAnalyzer::GetArcStartToLastDamageSeconds() const
{
    if (combatStartTime == 0 || lastDamageTime <= combatStartTime)
    {
        return 0.0;
    }

    return static_cast<double>(lastDamageTime - combatStartTime) / 1000.0;
}
uint64_t CombatAnalyzer::GetFirstDamageTime() const
{
    return firstDamageTime;
}

uint64_t CombatAnalyzer::GetLastDamageTime() const
{
    return lastDamageTime;
}
void CombatAnalyzer::SetCombatStartTime(uint64_t time)
{
    combatStartTime = time;
}


double CombatAnalyzer::GetDPS() const
{
    const double duration = GetArcStartToLastDamageSeconds();

    if (duration <= 0.0)
    {
        return 0.0;
    }

    return static_cast<double>(
        totalDirectDamage + totalBuffDamage
        ) / duration;
}
void CombatAnalyzer::SetPlayerInstanceID(uint16_t instanceID)
{
    playerInstanceID = instanceID;
}
uint16_t CombatAnalyzer::GetPlayerInstanceID() const
{
    return playerInstanceID;
}
void CombatAnalyzer::IncrementDownedCount()
{
    downedCount++;
}
uint32_t CombatAnalyzer::GetDownedCount() const
{
    return downedCount;
}
void CombatAnalyzer::IncrementDeathCount()
{
    deathCount++;
}

uint32_t CombatAnalyzer::GetDeathCount() const
{
    return deathCount;
}
uint32_t CombatAnalyzer::GetLastFightDownedCount() const
{
    return lastFightDownedCount;
}

uint32_t CombatAnalyzer::GetLastFightDeathCount() const
{
    return lastFightDeathCount;
}
void CombatAnalyzer::RefreshLastFightSurvivalCounts()
{
    lastFightDownedCount = downedCount;
    lastFightDeathCount = deathCount;
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

const std::unordered_map<uint32_t, int64_t>& CombatAnalyzer::GetDamageBySkill() const
{
    return damageBySkill;
}
    


