#ifndef STATS_H_INCLUDED
#define STATS_H_INCLUDED

#include "Vector.h"

    /**
     * @file Stats.h
     * @brief Generic statistical free functions for float vectors.
     *
     * Provides mean, sample standard deviation, mean absolute deviation,
     * and sample Pearson correlation coefficient.
     * No dependency on any weather-domain type.
     *
     * @author Deston
     * @version 2.0
     * @date 24/03/2026
     */

/**
 * @brief Calculate the arithmetic mean.
 * @param data  Vector of float values.
 * @return Mean, or 0.0 if empty.
 */
float CalculateMean(const Vector<float>& data);

/**
 * @brief Calculate the sample standard deviation.
 * @param data  Vector of float values.
 * @param mean  Pre-calculated mean.
 * @return Sample stdev (n-1 denominator), or 0.0 if size <= 1.
 */
float CalculateSampleStdDev(const Vector<float>& data, float mean);

/**
 * @brief Calculate the Mean Absolute Deviation.
 * @param data  Vector of float values.
 * @param mean  Pre-calculated mean.
 * @return MAD, or 0.0 if empty.
 */
float CalculateMAD(const Vector<float>& data, float mean);

/**
 * @brief Calculate the Sample Pearson Correlation Coefficient.
 * @param x  First variable (must be same size as y).
 * @param y  Second variable.
 * @return sPCC in [-1, 1], or 0.0 if n < 2 or denominator is 0.
 */
float CalculateSPCC(const Vector<float>& x, const Vector<float>& y);

#endif // STATS_H_INCLUDED
