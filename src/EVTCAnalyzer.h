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
};
struct EVTCAgent
{
    uint64_t address = 0;
    uint32_t profession = 0;
    uint32_t elite = 0;
    uint16_t toughness = 0;
    uint16_t concentration = 0;
    uint16_t healing = 0;
    uint16_t hitboxWidth = 0;
    uint16_t condition = 0;
    uint16_t hitboxHeight = 0;
    std::string name;
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
    const std::vector<EVTCSkillCast>& GetSkillCasts() const;

    void Clear();

    

private:
    EVTCHeader header;
    std::vector<EVTCAgent> agents;
    std::vector<EVTCSkillCast> skillCasts;

};
