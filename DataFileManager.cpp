#include "DataFileManager.h"

#include <fstream>
#include <sstream>
#include <string>

DataFileManager::DataFileManager()
    : m_filesLoaded(0), m_duplicateCount(0), m_fileCount(0)
{
    // m_timestampBst default-constructs to an empty tree
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

    // Reset the duplicate-detection BST for this load session.
    // Bst<long long> copy-assignment replaces the existing tree with
    // a freshly default-constructed (empty) one.
    m_timestampBst = Bst<long long>();

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
                // O(log n) duplicate check via Bst<long long>::search
                if (!IsDuplicateRecord(records, tempRecords[j]))
                {
                    long long key = MakeTimestampKey(tempRecords[j]);
                    m_timestampBst.insert(key);     // O(log n) insert
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

bool DataFileManager::IsDuplicateRecord(const Vector<WeatherRecord>&,
                                        const WeatherRecord& newRecord) const
{
    // O(log n) BST search
    return m_timestampBst.search(MakeTimestampKey(newRecord));
}

long long DataFileManager::MakeTimestampKey(const WeatherRecord& record) const
{
    // Encode date and time into a single 64-bit integer:
    //   year * 100,000,000 + month * 1,000,000 + day * 10,000
    //                      + hour  *       100 + minute
    // Unique for any valid (year, month, day, hour, minute) combination.
    Date d = record.GetDate();
    Time t = record.GetTime();

    return static_cast<long long>(d.GetYear())  * 100000000LL
         + static_cast<long long>(d.GetMonth()) *   1000000LL
         + static_cast<long long>(d.GetDay())   *     10000LL
         + static_cast<long long>(t.GetHour())  *       100LL
         + static_cast<long long>(t.GetMinute());
}
