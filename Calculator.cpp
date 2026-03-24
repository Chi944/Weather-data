#include "Calculator.h"
#include "Stats.h"

Calculator::Calculator()
{
}

float Calculator::ToKwhM2(float wm2)
{
    return wm2 / SOLAR_CONVERSION_DIVISOR;
}

bool Calculator::CalculateWindStats(const WeatherDataStore& store,
                                    int month, int year,
                                    float& mean, float& stdDev, float& mad) const
{
    const MonthlyData* md = store.GetMonthData(year, month);
    if (md == nullptr || md->windSpeeds.Empty()) return false;

    mean   = CalculateMean(md->windSpeeds);
    stdDev = CalculateSampleStdDev(md->windSpeeds, mean);
    mad    = CalculateMAD(md->windSpeeds, mean);
    return true;
}

bool Calculator::CalculateTempStats(const WeatherDataStore& store,
                                    int month, int year,
                                    float& mean, float& stdDev, float& mad) const
{
    const MonthlyData* md = store.GetMonthData(year, month);
    if (md == nullptr || md->temperatures.Empty()) return false;

    mean   = CalculateMean(md->temperatures);
    stdDev = CalculateSampleStdDev(md->temperatures, mean);
    mad    = CalculateMAD(md->temperatures, mean);
    return true;
}

bool Calculator::CalculateSolarTotal(const WeatherDataStore& store,
                                     int month, int year,
                                     float& total) const
{
    const MonthlyData* md = store.GetMonthData(year, month);
    if (md == nullptr) return false;

    total = 0.0f;
    bool found = false;

    for (int i = 0; i < md->solarRaw.Size(); i++)
    {
        if (md->solarRaw[i] >= MIN_SOLAR_RADIATION)
        {
            total += ToKwhM2(md->solarRaw[i]);
            found  = true;
        }
    }
    return found;
}

bool Calculator::CalculateCorrelations(const WeatherDataStore& store, int month,
                                        float& s_t, float& s_r, float& t_r) const
{
    Vector<float> allWinds, allTemps, allSolarRaw;
    store.GetMonthDataAllYears(month, allWinds, allTemps, allSolarRaw);

    int n = allWinds.Size();
    if (n == 0) return false;

    // S_T: all records for this month across all years
    s_t = CalculateSPCC(allWinds, allTemps);

    // S_R and T_R: only records where solar >= threshold (daytime readings)
    Vector<float> wFiltered, tFiltered, srFiltered;
    for (int i = 0; i < n; i++)
    {
        if (allSolarRaw[i] >= MIN_SOLAR_RADIATION)
        {
            wFiltered.Append(allWinds[i]);
            tFiltered.Append(allTemps[i]);
            srFiltered.Append(ToKwhM2(allSolarRaw[i]));
        }
    }

    s_r = (wFiltered.Size() >= 2) ? CalculateSPCC(wFiltered,  srFiltered) : 0.0f;
    t_r = (tFiltered.Size() >= 2) ? CalculateSPCC(tFiltered,  srFiltered) : 0.0f;

    return true;
}
