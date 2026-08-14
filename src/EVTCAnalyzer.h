#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct EVTCHeader
{
    std::string version;
    uint8_t revision = 0;
    uint16_t encounterID = 0;
    uint32_t agentCount = 0;
    uint32_t skillCount = 0;
    uint64_t extractedSize = 0;
    uint64_t skillSectionOffset = 0;
    uint64_t combatEventOffset = 0;
    uint64_t combatEventCount = 0;
};
struct EVTCAgent
{
    uint64_t address = 0;
    uint32_t profession = 0;
    uint32_t elite = 0;
    int16_t toughness = 0;
    int16_t concentration = 0;
    int16_t healing = 0;
    uint16_t hitboxWidth = 0;
    int16_t condition = 0;
    uint16_t reserved = 0;
    std::string name;
};
struct EVTCSkill
{
    uint32_t id = 0;
    std::string name;
};
struct EVTCCombatEvent
{
    uint64_t time = 0;
    uint64_t srcAgent = 0;
    uint64_t dstAgent = 0;
    int32_t value = 0;
    int32_t buffDmg = 0;
    uint32_t overstackValue = 0;
    uint32_t skillID = 0;

    uint16_t srcInstid = 0;
    uint16_t dstInstid = 0;
    uint16_t srcMasterInstid = 0;
    uint16_t dstMasterInstid = 0;

    uint8_t iff = 0;
    uint8_t buff = 0;
    uint8_t result = 0;
    uint8_t isActivation = 0;
    uint8_t isBuffRemove = 0;
    uint8_t isNinety = 0;
    uint8_t isFifty = 0;
    uint8_t isMoving = 0;
    uint8_t isStateChange = 0;
    uint8_t isFlanking = 0;
    uint8_t isShields = 0;
    uint8_t isOffcycle = 0;
    uint8_t pad61 = 0;
    uint8_t pad62 = 0;
    uint8_t pad63 = 0;
    uint8_t pad64 = 0;
};


struct EVTCSkillCast
{
    uint64_t time = 0;
    uint32_t skillID = 0;
    std::string skillName;
};

class EVTCAnalyzer
{
public:
    EVTCAnalyzer() = default;

    bool LoadFile(const std::string& filePath);
    const EVTCHeader& GetHeader() const;

    const std::vector<EVTCAgent>& GetAgents() const;
    const std::vector<EVTCSkill>& GetSkills() const;
    const std::vector<EVTCCombatEvent>& GetCombatEvents() const;
    const std::vector<EVTCSkillCast>& GetSkillCasts() const;

    void Clear();

    

private:
    EVTCHeader header;
    std::vector<EVTCAgent> agents;
    std::vector<EVTCSkill> skills;
    std::vector<EVTCCombatEvent> combatEvents;
    std::vector<EVTCSkillCast> skillCasts;

};
