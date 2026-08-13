#include "EVTCAnalyzer.h"
#include <fstream>

bool EVTCAnalyzer::LoadFile(const std::string& filePath)
{
    Clear();

    std::ifstream file(filePath, std::ios::binary);

    if (!file.is_open())
    {
        return false;
    }

    char versionBuffer[12] = {};

    file.read(versionBuffer, 12);

    if (!file)
    {
        return false;
    }

    header.version.assign(versionBuffer, 12);

    file.read(
        reinterpret_cast<char*>(&header.revision),
        sizeof(header.revision)
    );

    file.read(
        reinterpret_cast<char*>(&header.encounterID),
        sizeof(header.encounterID)
    );

    if (!file)
    {
        Clear();
        return false;
    }

    return true;
}
const EVTCHeader& EVTCAnalyzer::GetHeader() const
{
    return header;
}

const std::vector<EVTCSkillCast>& EVTCAnalyzer::GetSkillCasts() const
{
    return skillCasts;
}

void EVTCAnalyzer::Clear()
{
    header = EVTCHeader{};
    skillCasts.clear();
}