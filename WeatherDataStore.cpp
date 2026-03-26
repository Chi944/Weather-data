#include "WeatherDataStore.h"

WeatherDataStore::WeatherDataStore()
{
    // Map<> and Bst<int> default-construct to empty — nothing else needed
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

        // operator[] creates the inner Map and MonthlyData if absent,
        // then returns a reference
        MonthlyData& md = m_data[year][month];
        md.windSpeeds.Append(ToKmh(r.GetWindSpeed()));
        md.temperatures.Append(r.GetTemperature());
        md.solarRaw.Append(r.GetSolarRadiation());

        // BST silently rejects duplicate years
        m_yearBst.insert(year);
    }
}

bool WeatherDataStore::HasData(int year, int month) const
{
    // getPtr returns nullptr if the year key is absent.
    // getPtr(month) != nullptr
    const Map<int, MonthlyData>* yearMap = m_data.getPtr(year);
    if (yearMap == nullptr) return false;

    return yearMap->getPtr(month) != nullptr;
}

const MonthlyData* WeatherDataStore::GetMonthData(int year, int month) const
{
    const Map<int, MonthlyData>* yearMap = m_data.getPtr(year);
    if (yearMap == nullptr) return nullptr;

    return yearMap->getPtr(month);
}

void WeatherDataStore::GetMonthDataAllYears(int month,
                                             Vector<float>& winds,
                                             Vector<float>& temps,
                                             Vector<float>& solarRaw) const
{
    // Index-based iteration over the outer Map (all years)
    for (int i = 0; i < m_data.size(); i++)
    {
        const Map<int, MonthlyData>* monthMapPtr = m_data.getPtr(m_data.getKey(i));
        if (monthMapPtr == nullptr) continue;

        const MonthlyData* md = monthMapPtr->getPtr(month);
        if (md == nullptr) continue;   // this year has no data for 'month'

        for (int j = 0; j < md->windSpeeds.Size(); j++)
        {
            winds.Append(md->windSpeeds[j]);
            temps.Append(md->temperatures[j]);
            solarRaw.Append(md->solarRaw[j]);
        }
    }
}

void WeatherDataStore::TraverseYears(void (*visit)(int&)) const
{
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
