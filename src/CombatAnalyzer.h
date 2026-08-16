#pragma once

#include <cstdint>
#include "ArcDPS.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace ArcDPS
{
    struct CombatEvent;
}

struct CombatRecord
{
    uint64_t time = 0;
    uint64_t eventID = 0;
    uint32_t skillID = 0;
    uint64_t sourceAgent = 0;
    uint16_t sourceInstanceID = 0;
    uint16_t sourceMasterInstanceID = 0;
    uint64_t destinationAgent = 0;
    uint16_t destinationInstanceID = 0;
    int32_t value = 0;
    int32_t buffDamage = 0;
    uint32_t overstackValue;
    uint8_t buff = 0;
    uint8_t result = 0;
    uint8_t iff = 0;
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
    void ResetSession();

    void ProcessEvent(
        const ArcDPS::CombatEvent* event,
        const char* skillName,
        uint64_t eventID
    );

    uint64_t GetEventCount() const;
    uint64_t GetZeroSkillDamageEventCount() const;
   
    uint64_t GetDamageEventCount() const;
    int64_t GetTotalDirectDamage() const;
    int64_t GetTotalBuffDamage() const;
    double GetCombatDurationSeconds() const;
    
    uint64_t GetFirstDamageTime() const;
    uint64_t GetLastDamageTime() const;
    double GetDPS() const;
    void SetPlayerInstanceID(uint16_t instanceID);
    uint16_t GetPlayerInstanceID() const;
    int64_t GetLastFightDamage() const;
    double GetLastFightDurationSeconds() const;
    double GetLastFightDPS() const;
    void CaptureLastFight();
    uint32_t GetLastSkillID() const;
    uint8_t GetLastActivation() const;
    const std::vector<uint32_t>& GetRecentSkills() const;
    const std::vector<CombatRecord>& GetRecentRecords() const;
    std::string GetSkillName(uint32_t skillID) const;
    const std::unordered_map<uint32_t, uint32_t>& GetSkillUseCounts() const;
    const std::unordered_map<uint32_t, int64_t>& GetDamageBySkill() const;
    
    

private:
    uint64_t eventCount = 0;
    uint64_t damageEventCount = 0;
    uint64_t zeroSkillDamageEventCount = 0;
    int64_t totalDirectDamage = 0;
    int64_t totalBuffDamage = 0;
    uint64_t firstDamageTime = 0;
    uint64_t lastDamageTime = 0;
    int64_t lastFightDamage = 0;
    double lastFightDurationSeconds = 0.0;
    double lastFightDPS = 0.0;
    uint32_t lastSkillID = 0;
    uint8_t lastActivation = 0;
    uint16_t playerInstanceID = 0;
    
    std::vector<uint32_t> recentSkills;
    std::vector<CombatRecord> recentRecords;
    std::unordered_map<uint32_t, std::string> skillNames;
    std::unordered_map<uint32_t, uint32_t> skillUseCounts;
    std::unordered_map<uint32_t, int64_t> damageBySkill;
    std::unordered_map<uint32_t, uint64_t> lastSkillEventTimes;
    
   
  
};
