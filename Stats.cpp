#include "Stats.h"
#include "Vector.h"
#include <cmath>

float CalculateMean(const Vector<float>& data)
{
    if (data.Size() <= 0) return 0.0f;
    float sum = 0.0f;
    for (int i = 0; i < data.Size(); i++) sum += data[i];
    return sum / data.Size();
}

float CalculateSampleStdDev(const Vector<float>& data, float mean)
{
    if (data.Size() <= 1) return 0.0f;
    float sumSq = 0.0f;
    for (int i = 0; i < data.Size(); i++)
    {
        float d = data[i] - mean;
        sumSq += d * d;
    }
    return std::sqrt(sumSq / (data.Size() - 1));
}

float CalculateMAD(const Vector<float>& data, float mean)
{
    if (data.Size() == 0) return 0.0f;
    float sum = 0.0f;
    for (int i = 0; i < data.Size(); i++)
        sum += std::fabs(data[i] - mean);
    return sum / data.Size();
}

float CalculateSPCC(const Vector<float>& x, const Vector<float>& y)
{
    int n = x.Size();
    if (n < 2 || n != y.Size()) return 0.0f;

    float meanX = CalculateMean(x);
    float meanY = CalculateMean(y);

    float num = 0.0f, dX = 0.0f, dY = 0.0f;
    for (int i = 0; i < n; i++)
    {
        float dx = x[i] - meanX;
        float dy = y[i] - meanY;
        num += dx * dy;
        dX  += dx * dx;
        dY  += dy * dy;
    }

    float denom = std::sqrt(dX * dY);
    if (denom == 0.0f) return 0.0f;
    return num / denom;
}
