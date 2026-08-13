#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct EVTCHeader
{
    std::string version;
    uint8_t revision = 0;
    uint16_t encounterID = 0;
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

    const std::vector<EVTCSkillCast>& GetSkillCasts() const;

    void Clear();

private:
    EVTCHeader header;
    std::vector<EVTCSkillCast> skillCasts;

};
