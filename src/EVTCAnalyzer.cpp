#include "EVTCAnalyzer.h"
#include <fstream>
#include "miniz.h"

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

const std::vector<EVTCSkillCast>& EVTCAnalyzer::GetSkillCasts() const
{
    return skillCasts;
}

void EVTCAnalyzer::Clear()
{
    header = EVTCHeader{};
    agents.clear();
    skillCasts.clear();
}