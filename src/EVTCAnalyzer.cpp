#include "EVTCAnalyzer.h"
#include <fstream>
#include "miniz.h"
#include <cstring>

bool EVTCAnalyzer::LoadFile(const std::string& filePath)
{
    Clear();

    // ---------------------------------------------------------
    // Compressed ArcDPS EVTC log (.zevtc)
    // ---------------------------------------------------------
    if (filePath.size() >= 6 &&
        filePath.compare(filePath.size() - 6, 6, ".zevtc") == 0)
    {
        mz_zip_archive zipArchive = {};

        if (!mz_zip_reader_init_file(
            &zipArchive,
            filePath.c_str(),
            0))
        {
            return false;
        }

        const mz_uint fileCount =
            mz_zip_reader_get_num_files(&zipArchive);

        if (fileCount == 0)
        {
            mz_zip_reader_end(&zipArchive);
            return false;
        }

        size_t extractedSize = 0;

        void* extractedData =
            mz_zip_reader_extract_to_heap(
                &zipArchive,
                0,
                &extractedSize,
                0
            );

        if (extractedData == nullptr || extractedSize < 20)
        {
            if (extractedData != nullptr)
            {
                mz_free(extractedData);
            }

            mz_zip_reader_end(&zipArchive);
            return false;
        }

        const unsigned char* data =
            static_cast<const unsigned char*>(extractedData);
        header.extractedSize =
            static_cast<uint64_t>(extractedSize);

        // First 12 bytes = EVTC version string.
        header.version.assign(
            reinterpret_cast<const char*>(data),
            12
        );

        // Byte 12 = EVTC revision.
        header.revision = data[12];

        // Bytes 13-14 = encounter ID.
        header.encounterID =
            static_cast<uint16_t>(data[13]) |
            (static_cast<uint16_t>(data[14]) << 8);

        // Byte 15 is reserved/padding.

        // Bytes 16-19 = agent count.
        header.agentCount =
            static_cast<uint32_t>(data[16]) |
            (static_cast<uint32_t>(data[17]) << 8) |
            (static_cast<uint32_t>(data[18]) << 16) |
            (static_cast<uint32_t>(data[19]) << 24);
        // Parse all EVTC agents.
 // Header is 20 bytes. Each agent record is 96 bytes.
        const size_t agentRecordSize = 96;
        const size_t agentSectionOffset = 20;

        if (header.agentCount > 0)
        {
            const size_t requiredSize =
                agentSectionOffset +
                (static_cast<size_t>(header.agentCount) * agentRecordSize);

            if (extractedSize >= requiredSize)
            {
                agents.reserve(header.agentCount);

                for (uint32_t i = 0; i < header.agentCount; ++i)
                {
                    const unsigned char* agentData =
                        data + agentSectionOffset +
                        (static_cast<size_t>(i) * agentRecordSize);

                    EVTCAgent agent;

                    std::memcpy(&agent.address, agentData + 0, 8);
                    std::memcpy(&agent.profession, agentData + 8, 4);
                    std::memcpy(&agent.elite, agentData + 12, 4);
                    std::memcpy(&agent.toughness, agentData + 16, 2);
                    std::memcpy(&agent.concentration, agentData + 18, 2);
                    std::memcpy(&agent.healing, agentData + 20, 2);
                    std::memcpy(&agent.hitboxWidth, agentData + 22, 2);
                    std::memcpy(&agent.condition, agentData + 24, 2);
                    std::memcpy(&agent.reserved, agentData + 26, 2);

                    const char* nameData =
                        reinterpret_cast<const char*>(agentData + 28);

                    size_t nameLength = 0;

                    while (nameLength < 64 && nameData[nameLength] != '\0')
                    {
                        ++nameLength;
                    }

                    agent.name.assign(nameData, nameLength);

                    agents.push_back(agent);
                }
            }
        }
        // Skill table begins immediately after the agent table.
        const size_t skillCountOffset =
            agentSectionOffset +
            (static_cast<size_t>(header.agentCount) * agentRecordSize);

        // Need at least 4 bytes for the skill count.
        if (extractedSize < skillCountOffset + 4)
        {
            mz_free(extractedData);
            mz_zip_reader_end(&zipArchive);
            return false;
        }
        header.skillCount =
            static_cast<uint32_t>(data[skillCountOffset + 0]) |
            (static_cast<uint32_t>(data[skillCountOffset + 1]) << 8) |
            (static_cast<uint32_t>(data[skillCountOffset + 2]) << 16) |
            (static_cast<uint32_t>(data[skillCountOffset + 3]) << 24);

        // Parse first EVTC skill record for validation.
 // Skill table begins immediately after the 4-byte skill count.
 // Each skill record is 68 bytes:
 // 4-byte skill ID + 64-byte UTF-8 name.

        const size_t skillRecordSize = 68;
        const size_t skillSectionOffset = skillCountOffset + 4;
        header.skillSectionOffset =
            static_cast<uint64_t>(skillSectionOffset);

        if (header.skillCount > 0 &&
            extractedSize >= skillSectionOffset + skillRecordSize)
        {
            const unsigned char* skillData =
                data + skillSectionOffset;

            EVTCSkill skill;

            std::memcpy(
                &skill.id,
                skillData + 0,
                4
            );

            const char* skillNameData =
                reinterpret_cast<const char*>(skillData + 4);

            size_t skillNameLength = 0;

            while (skillNameLength < 64 &&
                skillNameData[skillNameLength] != '\0')
            {
                ++skillNameLength;
            }

            skill.name.assign(
                skillNameData,
                skillNameLength
            );

            skills.push_back(skill);
        }
        
        mz_free(extractedData);
        mz_zip_reader_end(&zipArchive);

        return true;
    }

    // ---------------------------------------------------------
    // Raw EVTC log (.evtc)
    // ---------------------------------------------------------
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

    // Skip the reserved/padding byte at offset 15.
    file.seekg(1, std::ios::cur);

    file.read(
        reinterpret_cast<char*>(&header.agentCount),
        sizeof(header.agentCount)
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

const std::vector<EVTCAgent>& EVTCAnalyzer::GetAgents() const
{
    return agents;
}
const std::vector<EVTCSkill>& EVTCAnalyzer::GetSkills() const
{
    return skills;
}

const std::vector<EVTCSkillCast>& EVTCAnalyzer::GetSkillCasts() const
{
    return skillCasts;
}

void EVTCAnalyzer::Clear()
{
    header = EVTCHeader{};
    agents.clear();
    skillCasts.clear();
    skills.clear();
}