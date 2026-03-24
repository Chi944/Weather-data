#include "WeatherDataStore.h"

WeatherDataStore::WeatherDataStore()
{
    // std::map and Bst<int> default-construct to empty — nothing else needed
}

float WeatherDataStore::ToKmh(float mps)
{
    return mps * WIND_MPS_TO_KMH;
}

void WeatherDataStore::Build(const Vector<WeatherRecord>& records)
{
    for (int i = 0; i < records.Size(); i++)
    {
        const WeatherRecord& r = records[i];
        int year  = r.GetDate().GetYear();
        int month = r.GetDate().GetMonth();

        // Insert into map — bucket created automatically if absent
        MonthlyData& md = m_data[year][month];
        md.windSpeeds.Append(ToKmh(r.GetWindSpeed()));
        md.temperatures.Append(r.GetTemperature());
        md.solarRaw.Append(r.GetSolarRadiation());

        // Insert year into BST — duplicate silently rejected by Bst::insert
        m_yearBst.insert(year);
    }
}

bool WeatherDataStore::HasData(int year, int month) const
{
    std::map<int, std::map<int, MonthlyData> >::const_iterator yit =
        m_data.find(year);
    if (yit == m_data.end()) return false;

    std::map<int, MonthlyData>::const_iterator mit =
        yit->second.find(month);
    return (mit != yit->second.end() && !mit->second.windSpeeds.Empty());
}

const MonthlyData* WeatherDataStore::GetMonthData(int year, int month) const
{
    std::map<int, std::map<int, MonthlyData> >::const_iterator yit =
        m_data.find(year);
    if (yit == m_data.end()) return nullptr;

    std::map<int, MonthlyData>::const_iterator mit =
        yit->second.find(month);
    if (mit == yit->second.end()) return nullptr;

    return &(mit->second);
}

void WeatherDataStore::GetMonthDataAllYears(int month,
                                             Vector<float>& winds,
                                             Vector<float>& temps,
                                             Vector<float>& solarRaw) const
{
    std::map<int, std::map<int, MonthlyData> >::const_iterator yit;
    for (yit = m_data.begin(); yit != m_data.end(); ++yit)
    {
        std::map<int, MonthlyData>::const_iterator mit =
            yit->second.find(month);

        if (mit == yit->second.end()) continue;

        const MonthlyData& md = mit->second;
        for (int i = 0; i < md.windSpeeds.Size(); i++)
        {
            winds.Append(md.windSpeeds[i]);
            temps.Append(md.temperatures[i]);
            solarRaw.Append(md.solarRaw[i]);
        }
    }
}

void WeatherDataStore::TraverseYears(void (*visit)(int&)) const
{
    // inOrderTraversal is the Bst<T> method that accepts an f1Typ callback.
    // This is the function-pointer usage required by Assignment 2.
    m_yearBst.inOrderTraversal(visit);
}

bool WeatherDataStore::YearExists(int year) const
{
    return m_yearBst.search(year);
}

int WeatherDataStore::GetYearCount() const
{
    return m_yearBst.size();
}

bool WeatherDataStore::IsEmpty() const
{
    return m_yearBst.isEmpty();
}
