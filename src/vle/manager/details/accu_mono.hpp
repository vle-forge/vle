/*
 * Copyright (C) 2015-2015 INRA
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VLE_MANAGER_ACCU_MONO_HPP_
#define VLE_MANAGER_ACCU_MONO_HPP_

#include <algorithm>
#include <vector>
#include <numeric>
#include <limits>
#include <memory>
#include <math.h>

#include <vle/utils/Exception.hpp>

namespace vle {
namespace manager {


namespace vu = vle::utils;
/**
 * @brief Different types of accumulators.
 */
enum AccuType
{
    STANDARD, //computes mean, std, var, min, max
    MEAN,     //compute mean only
    QUANTILE,  //compute quantiles
    ORDERED   //keep order: can access to specific value
};

/**
 * @brief Different types of statistics
 */
enum AccuStat
{
    S_mean,
    S_moment2,
    S_deviationSquareSum,
    S_stdDeviation,
    S_variance,
    S_squareSum,
    S_count,
    S_sum,
    S_min,
    S_max,
    S_at,
    S_quantile
};

class AccuMono
{

public:
    /**
     * @brief gives the optimal storage for a wanted statistics
     */
    static AccuType storageTypeForStat(AccuStat s)
    {
        switch(s){
        case S_mean:
            return MEAN;
            break;
        case S_moment2:
        case S_deviationSquareSum:
        case S_stdDeviation:
        case S_variance:
        case S_squareSum:
            return STANDARD;
            break;
        case S_count:
        case S_sum:
            return MEAN;
            break;
        case S_min:
        case S_max:
            return STANDARD;
            break;
        case S_at:
            return ORDERED;
            break;
        case S_quantile:
            return QUANTILE;
            break;
        }
        return STANDARD;
    }


    /**
     * @brief AccuMono dfault constructor
     */
    AccuMono() :
        accu(STANDARD),  msum(0), mcount(0), msquareSum(0),
        mmin(std::numeric_limits<double>::max()),
        mmax(std::numeric_limits<double>::min()),
        msorted(false), mvalues(nullptr), mquantile(0.5), mat(0)
    {
    }

    /**
     * @brief constructor with the specified storage strategy
     * @param type, the AccuType
     */
    AccuMono(AccuType type) :
        accu(type), msum(0), mcount(0), msquareSum(0),
        mmin(std::numeric_limits<double>::max()),
        mmax(std::numeric_limits<double>::min()),
        msorted(false), mvalues(nullptr), mquantile(0.5), mat(0)
    {
        if (accu == ORDERED or accu == QUANTILE) {
            mvalues.reset(new std::vector<double>());
        }
        if (accu == QUANTILE) {
            msorted = true;
        }
    }

    /**
     * @brief constructor with the wanted statistic
     * The storage is optimized according the wanted statistic
     * @param s, the AccuStat
     */
    AccuMono(AccuStat s) :
        accu(), msum(0), mcount(0), msquareSum(0),
        mmin(std::numeric_limits<double>::max()),
        mmax(std::numeric_limits<double>::min()),
        msorted(false), mvalues(nullptr), mquantile(0.5), mat(0)
    {
        accu = AccuMono::storageTypeForStat(s);
        if (accu == ORDERED or accu == QUANTILE) {
            mvalues.reset(new std::vector<double>());
        }
        if (accu == QUANTILE) {
            msorted = true;
        }
    }

    /**
     * @brief copy constructor
     * @param acc, the accu to copy
     */
    AccuMono(const AccuMono& acc):
        accu(acc.accu), msum(acc.msum), mcount(acc.mcount),
        msquareSum(acc.msquareSum), mmin(acc.mmin),  mmax(acc.mmax),
        msorted(acc.msorted), mvalues(nullptr), mquantile(0.5), mat(0)
    {
        if (acc.mvalues) {
            mvalues.reset(new std::vector<double>(acc.mvalues->begin(),
                    acc.mvalues->end()));
        }
    }

    /**
     * @brief destructor
     */
    ~AccuMono()
    {
    }

    /**
     * @brief set default quantile for 'quantile' stat
     * @param quantile
     */
    inline void setDefaultQuantile(double quantile)
    {
        mquantile = quantile;
    }


    /**
     * @brief get the default quantile value
     * @return the default quantile value
     */
    inline double defaultQuantile() const
    {
        return mquantile;
    }


    /**
     * @brief set default index for 'at' stat
     * @param at_index
     */
    inline void setDefaultAtIndex(unsigned int at_index)
    {
        mat = at_index;
    }


    /**
     * @brief get the default at index for 'at' stat
     * @return the default at index
     */
    inline double defaultAtIntex() const
    {
        return mat;
    }

    /**
     * @brief Inserts a real into the accumulator
     * @param v the real value
     */
    inline void insert(double v)
    {
        switch (accu) {
        case STANDARD: {
            msum += v;
            mcount++;
            msquareSum += pow(v,2);
            mmin = std::min(v, mmin);
            mmax = std::max(v, mmax);
            break;
        } case MEAN: {
            msum += v;
            mcount++;
            break;
        } case QUANTILE: {
            mvalues->push_back(v);
            msorted = false;
            break;
        } case ORDERED: {
            mvalues->push_back(v);
            break;
        } default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (1)");
            break;
        }
    }

    /**
     * @brief Mean statistic extractor
     * @return the mean value
     */
    inline double mean() const
    {
        switch (accu) {
        case STANDARD:
        case MEAN: {
            return msum / mcount;
            break;
        } case QUANTILE:
          case ORDERED: {
            double sum = std::accumulate(mvalues->begin(),mvalues->end(),0.0);
            return sum / mvalues->size();
        } default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (2)");
            return 0;
            break;
        }
    }
    /**
     * @brief Moment of order 2 extractor :
     * (sum of squares) divided by count
     * @return the moment of order 2 value
     */

    inline double moment2() const
    {
        switch (accu) {
        case STANDARD: {
            return msquareSum / mcount;
            break;
        } case MEAN: {
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        } default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (2)");
            return 0;
            break;
        }
    }
    /**
     * @brief Deviation Square sum extractor (requires rewriting) :
     * let N = count ; X = mean ; x_i the ieme value inserted.
     * sum_i [(x_i - X) ^ 2] = N * X^2 + sum_i (x_i ^ 2) - 2X sum_i(x_i)
     * @return the deviation square sum
     */
    inline double deviationSquareSum() const
    {
        switch (accu) {
        case STANDARD: {
            double mean = msum / mcount;
            return mcount * pow(mean,2) + msquareSum - 2 * mean * msum;
            break;
        } case MEAN:
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (2)");
            return 0;
            break;
        }
    }
    /**
     * @brief Corrected standard deviation,
     * @return the stdDeviation
     */
    inline double stdDeviation() const
    {
        switch (accu) {
        case STANDARD: {
            if (mcount < 2) {
                return 0;
            } else {
                return sqrt(deviationSquareSum()/(mcount-1));
            }
            break;
        } case MEAN:
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (2)");
            return 0;
            break;
        }
    }
    /**
     * @brief Corrected variance,
     * @return the variance
     */
    inline double variance() const
    {
        switch (accu) {
        case STANDARD: {
            if (mcount < 2) {
                return 0;
            } else {
                return deviationSquareSum()/(mcount-1);
            }
            break;
        } case MEAN:
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (2)");
            return 0;
            break;
        }
    }
    /**
     * @brief Square sum extractor
     * @return square sum value
     */
    inline double squareSum() const
    {
        switch (accu) {
        case STANDARD:
            return msquareSum;
            break;
        case MEAN:
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        case QUANTILE:
        case ORDERED: {
            double squareSum = 0;
            std::vector<double>::const_iterator itb = mvalues->begin();
            std::vector<double>::const_iterator ite = mvalues->end();
            for (; itb != ite; itb++) {
                squareSum += pow(*itb,2);
            }
            return squareSum;
        }}
        return 0;
    }
    /**
     * @brief Count extractor
     * @return the count value
     */
    inline unsigned int count() const
    {
        switch (accu) {
        case STANDARD:
        case MEAN:
            return mcount;
            break;
        case QUANTILE:
        case ORDERED:
            return mvalues->size();
            break;
        }
        return 0;

    }
    /**
     * @brief Sum extractor
     * @return the sum value
     */
    inline double sum() const
    {
        switch (accu) {
        case STANDARD:
        case MEAN:
            return msum;
            break;
        case QUANTILE:
        case ORDERED:
            return std::accumulate(mvalues->begin(),mvalues->end(),0.0);
            break;
        }
        return 0;
    }
    /**
     * @brief Minimal value extractor
     * @return the minimal value
     */
    inline double min() const
    {
        switch (accu) {
        case STANDARD:
            return mmin;
            break;
        case MEAN:
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        case QUANTILE:
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (5)");
            return 0;
            break;
        }
    }

    /**
     * @brief Maximal value extractor
     * @return the maximal value
     */
    inline double max() const
    {
        switch (accu) {
        case STANDARD:
            return mmax;
            break;
        default:
            throw vle::utils::ArgError(" [accu_mono] not available");
            return 0;
            break;
        }
    }

    /**
     * @brief access to a specific value
     * @param i, the index of value
     * @return the i-th inserted value
     */
    inline double at(unsigned int i) const
    {
        switch (accu) {
        case STANDARD:
        case MEAN:
        case QUANTILE:
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        case ORDERED:
            return mvalues->at(i);
            break;
        }
        return 0;
    }

    /**
     * @brief access to a specific value
     * @return the mat-th inserted value
     */
    inline double at() const
    {
        return at(mat);
    }

    /**
     * @brief Quantile value extractor (weigthed average method)
     * @param quantileOrder the quantile order
     * @returns the quantile value of order quantileOrder
     */
    inline double quantile(double quantileOrder)
    {
        switch (accu) {
        case STANDARD:
        case MEAN:
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        case QUANTILE: {
            if (!msorted) {
                std::sort(mvalues->begin(),mvalues->end());
                msorted = true;
            }
            return quantileOnSortedVect(*mvalues, quantileOrder);
            break;
        } case ORDERED: {
            std::vector<double> vals;
            vals.insert(vals.begin(), mvalues->begin(),mvalues->end());
            return quantileOnSortedVect(vals, quantileOrder);
            break;
        }}
        return 0;
    }
    /**
     * @brief Quantile value extractor (weigthed average method)
     * @returns the quantile value of order mquantile
     */
    inline double quantile()
    {
        return quantile(mquantile);
    }
    /**
     * @brief generic function to get a specified stat
     */
    inline double getStat(AccuStat s)
    {
        switch(s){
        case S_mean:
            return mean();
            break;
        case S_moment2:
            return moment2();
            break;
        case S_deviationSquareSum:
            return deviationSquareSum();
            break;
        case S_stdDeviation:
            return stdDeviation();
            break;
        case S_variance:
            return variance();
            break;
        case S_squareSum:
            return squareSum();
            break;
        case S_count:
            return count();
            break;
        case S_sum:
            return sum();
            break;
        case S_min:
            return min();
            break;
        case S_max:
            return max();
            break;
        case S_at:
            return at();
            break;
        case S_quantile:
            return quantile();
            break;
        }
        return std::numeric_limits<double>::min();
    }


    void clear()
    {
        msum = 0;
        mcount = 0;
        msquareSum = 0;
        mmin = std::numeric_limits<double>::max();
        mmax = std::numeric_limits<double>::min();
        msorted = false;
        mvalues.reset(nullptr);
    }

protected:
    //Type 7 of continuous quantile estimation (see R quantile doc)
    double quantileOnSortedVect(const std::vector<double>& x, double p)
    {

        double quantile = 0;
        unsigned int n = x.size();
        if (quantile == 1.0) {
            quantile = x[n - 1];
        } else {
            double m = 1-p;
            unsigned int j = floor(n*p + m);
            double g = n*p+1-p-j;
            quantile = (1-g)*x[j-1]  + g*x[j];
        }
        return quantile;
    }

    AccuType accu;
    double msum;
    unsigned int mcount;
    double msquareSum;
    double mmin;
    double mmax;
    bool msorted;//true if the vector of values is sorted
    std::unique_ptr<std::vector<double>> mvalues;
    double mquantile; //default value of quantile for stat 'quantile'
    unsigned int mat;//default value of index for stat 'at'
};

}} //namespaces

#endif

