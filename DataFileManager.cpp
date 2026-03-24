#include "DataFileManager.h"

#include <fstream>
#include <sstream>
#include <string>

DataFileManager::DataFileManager()
    : m_filesLoaded(0), m_duplicateCount(0), m_fileCount(0)
{
}

int DataFileManager::ReadDataSourceFile(const std::string& dataSourceFile,
                                        Vector<std::string>& filenames)
{
    std::ifstream file(dataSourceFile.c_str());
    int count = 0;

    if (!file.is_open())
    {
        return 0;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::size_t first = line.find_first_not_of(" \t\r\n");
        if (first != std::string::npos)
        {
            std::size_t last     = line.find_last_not_of(" \t\r\n");
            std::string filename = line.substr(first, last - first + 1);

            if (!filename.empty())
            {
                filenames.Append(filename);
                count++;
            }
        }
    }

    file.close();
    return count;
}

int DataFileManager::LoadDataFiles(const Vector<std::string>& filenames,
                                   Vector<WeatherRecord>& records,
                                   DataLoader& loader,
                                   const std::string& dataPath)
{
    int totalRecords  = 0;
    m_filesLoaded     = 0;
    m_duplicateCount  = 0;
    m_fileCount       = filenames.Size();

    // Pre-size hash set and records vector to avoid rehashing / reallocation.
    m_timestampSet.clear();
    m_timestampSet.reserve(700000);
    records.Reserve(700000);

    for (int i = 0; i < filenames.Size() && i < MAX_FILE_RESULTS; i++)
    {
        std::string fullPath = dataPath + filenames[i];
        Vector<WeatherRecord> tempRecords;

        if (loader.LoadFromFile(fullPath, tempRecords))
        {
            int newRecords     = 0;
            int fileDuplicates = 0;

            for (int j = 0; j < tempRecords.Size(); j++)
            {
                // O(1) average lookup instead of O(n) linear scan
                if (!IsDuplicateRecord(records, tempRecords[j]))
                {
                    m_timestampSet.insert(MakeTimestampKey(tempRecords[j]));
                    records.Append(tempRecords[j]);
                    newRecords++;
                }
                else
                {
                    fileDuplicates++;
                    m_duplicateCount++;
                }
            }

            totalRecords += newRecords;
            m_filesLoaded++;

            std::ostringstream oss;
            oss << "OK (" << tempRecords.Size() << " records, "
                << newRecords << " new, "
                << fileDuplicates << " duplicates skipped)";
            m_fileResults[i] = oss.str();
        }
        else
        {
            m_fileResults[i] = "FAILED - " + loader.GetLastError();
        }
    }

    return totalRecords;
}

int DataFileManager::GetFilesLoaded() const
{
    return m_filesLoaded;
}

int DataFileManager::GetDuplicateCount() const
{
    return m_duplicateCount;
}

int DataFileManager::GetFileCount() const
{
    return m_fileCount;
}

std::string DataFileManager::GetFileResult(int index) const
{
    if (index < 0 || index >= m_fileCount) return "";
    return m_fileResults[index];
}

bool DataFileManager::IsDuplicateRecord(const Vector<WeatherRecord>& /*records*/,
                                        const WeatherRecord& newRecord) const
{
    // O(1) average lookup via the pre-built integer hash set.
    return m_timestampSet.count(MakeTimestampKey(newRecord)) > 0;
}

long long DataFileManager::MakeTimestampKey(const WeatherRecord& record) const
{
    // Encode date and time into a single 64-bit integer:
    //   year * 100,000,000 + month * 1,000,000 + day * 10,000
    //                      + hour * 100 + minute
    Date d = record.GetDate();
    Time t = record.GetTime();

    return static_cast<long long>(d.GetYear())  * 100000000LL
         + static_cast<long long>(d.GetMonth()) *   1000000LL
         + static_cast<long long>(d.GetDay())   *     10000LL
         + static_cast<long long>(t.GetHour())  *       100LL
         + static_cast<long long>(t.GetMinute());
}
