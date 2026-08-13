#include "EVTCAnalyzer.h"
#include <fstream>
#include "miniz.h"

bool EVTCAnalyzer::LoadFile(const std::string& filePath)
{
    Clear();

    // ---------------------------------------------------------
    // Compressed ArcDPS EVTC log (.zevtc)
    // For now, only verify that miniz can open the archive.
    // Actual extraction/parsing comes next.
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

        if (extractedData == nullptr || extractedSize < 16)
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

    // EVTC version string is 12 bytes.
    char versionBuffer[12] = {};

    file.read(versionBuffer, 12);

    if (!file)
    {
        return false;
    }

    header.version.assign(versionBuffer, 12);

    // Revision
    file.read(
        reinterpret_cast<char*>(&header.revision),
        sizeof(header.revision)
    );

    // Encounter ID
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