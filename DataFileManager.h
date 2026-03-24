#ifndef DATA_FILE_MANAGER_H_INCLUDED
#define DATA_FILE_MANAGER_H_INCLUDED

#include "Vector.h"
#include "WeatherRecord.h"
#include "DataLoader.h"
#include "Constants.h"
#include <string>
#include <unordered_set>

    /**
     * @file DataFileManager.h
     * @brief DataFileManager class — reads a file list and coordinates CSV loading
     *
     * Reads a plain-text configuration file that lists one CSV filename per line,
     * then uses DataLoader to load each file. Duplicate records (same date and
     * time stamp) are detected and filtered so the caller receives a unique set.
     *
     * Per-file result strings are stored internally and retrieved via
     * GetFileResult(). This class contains no console output.
     *
     * @author Deston
     * @version 1.0
     * @date 19/02/2026
     */

class DataFileManager
{
public:
    /**
     * @brief Default constructor — initialises all counters to zero
     */
    DataFileManager();

    /**
     * @brief Read CSV filenames from a data-source configuration file
     * @param dataSourceFile Full path to the configuration file
     * @param filenames      Vector to append filenames to
     * @return Number of filenames read, or 0 on error
     */
    int ReadDataSourceFile(const std::string& dataSourceFile,
                           Vector<std::string>& filenames);

    /**
     * @brief Load weather records from each CSV file in the list
     * @param filenames Vector of bare filenames (without directory prefix)
     * @param records   Vector to append loaded records to
     * @param loader    DataLoader instance used for each file
     * @param dataPath  Directory prefix prepended to each filename
     * @return Total number of unique records loaded across all files
     *
     * Use GetFilesLoaded(), GetDuplicateCount(), and GetFileResult() after
     * this call to obtain per-file status information.
     */
    int LoadDataFiles(const Vector<std::string>& filenames,
                      Vector<WeatherRecord>& records,
                      DataLoader& loader,
                      const std::string& dataPath);

    /** @brief Number of files successfully loaded */
    int GetFilesLoaded() const;

    /** @brief Number of duplicate records that were filtered out */
    int GetDuplicateCount() const;

    /** @brief Total number of files that were attempted */
    int GetFileCount() const;

    /**
     * @brief Retrieve the result string for one file
     * @param index Zero-based file index (must be < GetFileCount())
     * @return Result string, or empty string if index is out of range
     */
    std::string GetFileResult(int index) const;

private:
    int m_filesLoaded;    /**< Files successfully loaded */
    int m_duplicateCount; /**< Duplicate records filtered */
    int m_fileCount;      /**< Total files attempted */

    /** @brief Maximum number of files that can be tracked */
    static const int MAX_FILE_RESULTS = 100;

    std::string m_fileResults[MAX_FILE_RESULTS]; /**< Per-file result strings */

    /**
     * @brief Check whether a record with the same date and time already exists
     * @param records   Existing records to search
     * @param newRecord Record to check for duplication
     * @return true if a duplicate was found
     */
    bool IsDuplicateRecord(const Vector<WeatherRecord>& records,
                           const WeatherRecord& newRecord) const;

    /**
     * @brief Build a compact 64-bit integer key from a record's timestamp
     * @param record WeatherRecord whose date/time components are encoded
     * @return Integer key unique for each distinct date+time pair
     *
     * Encoding avoids dynamic string allocation for minimal per-record cost.
     */
    long long MakeTimestampKey(const WeatherRecord& record) const;

    std::unordered_set<long long> m_timestampSet; /**< O(1) duplicate-detection store */
};

#endif // DATA_FILE_MANAGER_H_INCLUDED
