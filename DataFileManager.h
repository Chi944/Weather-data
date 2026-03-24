#ifndef DATA_FILE_MANAGER_H_INCLUDED
#define DATA_FILE_MANAGER_H_INCLUDED

#include "Vector.h"
#include "WeatherRecord.h"
#include "DataLoader.h"
#include "Bst.h"
#include "Constants.h"
#include <string>

    /**
     * @file DataFileManager.h
     * @brief DataFileManager class — reads a file list and coordinates CSV loading.
     *
     * Reads a plain-text configuration file that lists one CSV filename per line,
     * then uses DataLoader to load each file. Duplicate records (same date and
     * time stamp) are detected and filtered so the caller receives a unique set.
     *
     * Duplicate detection uses a Bst<long long> keyed on a compact integer
     * encoding of each record's date and time.  Bst<long long> provides
     * O(log n) search and insert, satisfies the assignment's requirement to
     * use no unwrapped STL containers, and gives an additional concrete use
     * of the BST data structure beyond the year index in WeatherDataStore.
     *
     * Per-file result strings are stored internally and retrieved via
     * GetFileResult(). This class contains no console output.
     *
     * @author Deston
     * @version 2.0
     * @date 24/03/2026
     */
class DataFileManager
{
public:
    /**
     * @brief Default constructor — initialises all counters to zero.
     */
    DataFileManager();

    /**
     * @brief Read CSV filenames from a data-source configuration file.
     * @param dataSourceFile Full path to the configuration file.
     * @param filenames      Vector to append filenames to.
     * @return Number of filenames read, or 0 on error.
     */
    int ReadDataSourceFile(const std::string& dataSourceFile,
                           Vector<std::string>& filenames);

    /**
     * @brief Load weather records from each CSV file in the list.
     *
     * Iterates through filenames, loads each via DataLoader, then inserts
     * each new record's timestamp key into the duplicate-detection BST.
     * Records whose key is already present in the BST are skipped.
     *
     * @param filenames Vector of bare filenames (without directory prefix).
     * @param records   Vector to append loaded records to.
     * @param loader    DataLoader instance used for each file.
     * @param dataPath  Directory prefix prepended to each filename.
     * @return Total number of unique records loaded across all files.
     */
    int LoadDataFiles(const Vector<std::string>& filenames,
                      Vector<WeatherRecord>& records,
                      DataLoader& loader,
                      const std::string& dataPath);

    /** @brief Number of files successfully loaded. */
    int GetFilesLoaded() const;

    /** @brief Number of duplicate records that were filtered out. */
    int GetDuplicateCount() const;

    /** @brief Total number of files that were attempted. */
    int GetFileCount() const;

    /**
     * @brief Retrieve the result string for one file.
     * @param index Zero-based file index (must be < GetFileCount()).
     * @return Result string, or empty string if index is out of range.
     */
    std::string GetFileResult(int index) const;

private:
    int m_filesLoaded;    /**< Files successfully loaded. */
    int m_duplicateCount; /**< Duplicate records filtered. */
    int m_fileCount;      /**< Total files attempted. */

    /** @brief Maximum number of files that can be tracked. */
    static const int MAX_FILE_RESULTS = 100;

    std::string m_fileResults[MAX_FILE_RESULTS]; /**< Per-file result strings. */

    /**
     * @brief Bst<long long> used for O(log n) duplicate-timestamp detection.
     *
     * Each record's date and time are encoded into a single 64-bit integer
     * key (see MakeTimestampKey). The BST rejects duplicate key insertions
     * automatically, making search() a clean O(log n) existence check.
     *
     * Using Bst<long long> here satisfies the assignment requirement that
     * no unwrapped STL containers appear in application code, while also
     * providing a second concrete use of the BST beyond the year index.
     */
    Bst<long long> m_timestampBst;

    /**
     * @brief Check whether a record's timestamp already exists in the BST.
     * @param records   Unused parameter (kept for interface compatibility).
     * @param newRecord Record to check for duplication.
     * @return true if a duplicate key is found in m_timestampBst.
     */
    bool IsDuplicateRecord(const Vector<WeatherRecord>& records,
                           const WeatherRecord& newRecord) const;

    /**
     * @brief Build a compact 64-bit integer key from a record's timestamp.
     *
     * Encoding:
     *   key = year * 100,000,000
     *       + month *   1,000,000
     *       + day   *      10,000
     *       + hour  *         100
     *       + minute
     *
     * This encoding is unique for any valid (year, month, day, hour, minute)
     * combination within the weather data range. It avoids dynamic string
     * allocation, keeping per-record overhead minimal.
     *
     * @param record WeatherRecord whose date/time components are encoded.
     * @return Integer key unique for each distinct date+time pair.
     */
    long long MakeTimestampKey(const WeatherRecord& record) const;
};

#endif // DATA_FILE_MANAGER_H_INCLUDED
