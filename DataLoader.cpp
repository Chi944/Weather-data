#include "DataLoader.h"
#include "Constants.h"

#include <fstream>
#include <sstream>
#include <string>

DataLoader::DataLoader() : m_lastError("")
{
}

bool DataLoader::LoadFromFile(const std::string& filename, Vector<WeatherRecord>& records)
{
    if (filename.empty())
    {
        m_lastError = "Empty filename provided";
        return false;
    }

    std::ifstream file(filename.c_str());

    if (!file.is_open())
    {
        m_lastError = "Cannot open file: " + filename;
        return false;
    }

    if (file.peek() == std::ifstream::traits_type::eof())
    {
        m_lastError = "File is empty: " + filename;
        file.close();
        return false;
    }

    std::string line;

    if (!std::getline(file, line))
    {
        m_lastError = "Cannot read header from file: " + filename;
        file.close();
        return false;
    }

    if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos)
    {
        m_lastError = "Empty header in file: " + filename;
        file.close();
        return false;
    }

    int wastIdx = -1, windIdx = -1, solarIdx = -1, tempIdx = -1;

    if (!ParseHeader(line, wastIdx, windIdx, solarIdx, tempIdx))
    {
        m_lastError = "WAST column not found in header of: " + filename;
        file.close();
        return false;
    }

    int successCount = 0;

    while (std::getline(file, line))
    {
        if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos)
        {
            continue;
        }

        WeatherRecord record;

        if (ParseLine(line, wastIdx, windIdx, solarIdx, tempIdx, record))
        {
            records.Append(record);
            successCount++;
        }
    }

    file.close();

    if (successCount == 0)
    {
        m_lastError = "No valid records found in: " + filename;
        return false;
    }

    return true;
}

std::string DataLoader::GetLastError() const
{
    return m_lastError;
}

bool DataLoader::ParseHeader(const std::string& header, int& wastIdx, int& windIdx,
                             int& solarIdx, int& tempIdx)
{
    wastIdx = windIdx = solarIdx = tempIdx = -1;

    std::istringstream ss(header);
    std::string field;
    int col = 0;

    while (std::getline(ss, field, ','))
    {
        field = Trim(field);

        if      (field == "WAST") wastIdx  = col;
        else if (field == "S")    windIdx  = col;
        else if (field == "SR")   solarIdx = col;
        else if (field == "T")    tempIdx  = col;

        col++;
    }

    return wastIdx >= 0;
}

bool DataLoader::ParseLine(const std::string& line, int wastIdx, int windIdx,
                           int solarIdx, int tempIdx, WeatherRecord& record)
{
    std::string fields[MAX_CSV_FIELDS];
    int fieldCount = 0;

    std::istringstream ss(line);
    std::string field;

    while (std::getline(ss, field, ',') && fieldCount < MAX_CSV_FIELDS)
    {
        fields[fieldCount++] = Trim(field);
    }

    if (wastIdx < 0 || wastIdx >= fieldCount)
    {
        return false;
    }

    Date date;
    Time time;

    if (!ParseWAST(fields[wastIdx], date, time))
    {
        return false;
    }

    record.SetDate(date);
    record.SetTime(time);

    if (windIdx  >= 0 && windIdx  < fieldCount)
    {
        record.SetWindSpeed(SafeStringToFloat(fields[windIdx],  0.0f));
    }

    if (solarIdx >= 0 && solarIdx < fieldCount)
    {
        record.SetSolarRadiation(SafeStringToFloat(fields[solarIdx], 0.0f));
    }

    if (tempIdx  >= 0 && tempIdx  < fieldCount)
    {
        record.SetTemperature(SafeStringToFloat(fields[tempIdx], 0.0f));
    }

    return true;
}

bool DataLoader::ParseWAST(const std::string& wast, Date& date, Time& time)
{
    if (wast.empty()) return false;

    std::istringstream ss(wast);
    std::string dateStr, timeStr;

    if (!(ss >> dateStr >> timeStr)) return false;

    int d, m, y;
    char sep1, sep2;
    std::istringstream ds(dateStr);

    if (!(ds >> d >> sep1 >> m >> sep2 >> y)) return false;
    if (sep1 != '/' || sep2 != '/')           return false;
    if (y < 1900 || y > 2100)                 return false;
    if (!date.SetDate(d, m, y))               return false;

    int hr, min;
    char colon;
    std::istringstream ts(timeStr);

    if (!(ts >> hr >> colon >> min)) return false;
    if (colon != ':')                return false;
    if (!time.SetTime(hr, min))      return false;

    return true;
}

std::string DataLoader::Trim(const std::string& str)
{
    std::size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
    {
        return "";
    }
    std::size_t last = str.find_last_not_of(" \t\r\n");

    return str.substr(first, last - first + 1);
}

float DataLoader::SafeStringToFloat(const std::string& str, float defaultVal)
{
    if (str.empty()) return defaultVal;

    bool negative = false;
    std::size_t pos = 0;

    if (str[pos] == '-')
    {
        negative = true;
        pos++;
    }
    else if (str[pos] == '+')
    {
        pos++;
    }

    float result    = 0.0f;
    bool  hasDigits = false;

    while (pos < str.length() && str[pos] >= '0' && str[pos] <= '9')
    {
        result = result * 10.0f + (str[pos] - '0');
        hasDigits = true;
        pos++;
    }

    if (pos < str.length() && str[pos] == '.')
    {
        pos++;
        float divisor = 10.0f;

        while (pos < str.length() && str[pos] >= '0' && str[pos] <= '9')
        {
            result += (str[pos] - '0') / divisor;
            divisor *= 10.0f;
            hasDigits = true;
            pos++;
        }
    }

    if (!hasDigits) return defaultVal;
    return negative ? -result : result;
}
