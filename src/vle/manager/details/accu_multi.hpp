 /*
 * Copyright (C) 2009-2010 INRA
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
#ifndef VLE_MANAGER_ACCU_MULTI_HPP_
#define VLE_MANAGER_ACCU_MULTI_HPP_

#include <algorithm>
#include <vector>
#include <math.h>
#include <functional>
#include <numeric>
#include <limits>

#include <vle/utils/Exception.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Table.hpp>

#include "accu_mono.hpp"

namespace vu = vle::utils;

namespace vle {
namespace manager {

/**
 * @brief Instantiation of multi dimensional accumulator STORE
 */

class AccuMulti
{
public:
    /**
     * @brief Accu constructor
     * @param  args, common accumulator initialization structure
     */
    AccuMulti(AccuType type) : accu(type),
        init_size(false), mstats()
    {
    }
    AccuMulti(AccuStat s) : accu(),
        init_size(false), mstats()
    {
        accu = AccuMono::storageTypeForStat(s);
    }

    AccuMulti(const AccuMulti& acc) : accu(acc.accu),
        init_size(acc.init_size), mstats(acc.mstats.begin(), acc.mstats.end())
    {
    }

    ~AccuMulti()
    {}

    inline bool hasSize()
    {
        return init_size;
    }



    inline unsigned int size() const
    {
        return mstats.size();
    }

    inline void setSize(unsigned int s)
    {
        if (mstats.size() == s) {
            return;
        }

        double quantile = 0.5;
        double at_index = 0;
        if (mstats.size() > 0) {
            quantile = mstats[0].defaultQuantile();
            at_index = mstats[0].defaultAtIntex();
        }

        mstats.clear();
        for (unsigned int i=0; i<s; i++){
            mstats.emplace_back(accu);
            AccuMono& s = mstats[i];
            s.setDefaultQuantile(quantile);
            s.setDefaultAtIndex(at_index);
        }
    }

    inline void setDefaultQuantile(double quantile)
    {
        for (AccuMono& a : mstats){
            a.setDefaultQuantile(quantile);
        }
    }
    inline void setDefaultAtIndex(unsigned at_index)
    {
        for (AccuMono& a : mstats){
            a.setDefaultAtIndex(at_index);
        }
    }

    void insertAccuStat(AccuMulti& a, AccuStat s)
    {
        if (a.size() != size()) {
            throw vle::utils::ArgError(" [accu_multi] error size ");
        }
        for (unsigned int i=0; i < a.size() ; i++) {
            mstats[i].insert(a.mstats[i].getStat(s));
        }
    }

    void insertColumn(const vle::value::Matrix& m,
            unsigned int col)
    {
        unsigned int nbEl = m.rows();
        bool hasName = not m.get(0,0)->isDouble();
        if (hasName) {//first element is the col name
            nbEl --;
        }
        if (not hasSize()) {
            setSize(nbEl);
        } else {
            if (nbEl != mstats.size()) {
                throw "error";
            }
        }
        for (unsigned int i=0; i < nbEl ; i++) {
            if (hasName) {
                mstats[i].insert(m.getDouble(col, i+1));
            } else {
                mstats[i].insert(m.getDouble(col, i));
            }
        }
    }

    /**
     * @brief generic get Stat
     * @param res[out], tuple filled with stat of all Accu
     * @param s, the stat to use
     */
    void fillStat(vle::value::Tuple& res, AccuStat s)
    {
        res.value().resize(mstats.size());
        for (unsigned int i = 0; i < res.size(); i++) {
            res[i] = mstats[i].getStat(s);
        }
    }

    /**
     * @brief generic get Stat
     * @param res[out], tuple filled with stat of all Accu
     * @param col[in], index of the colume of res to fill
     * @param s[in], the stat to use
     */
    void fillStat(vle::value::Table& res, unsigned int col, AccuStat s)
    {
        for (unsigned int i = 0; i < mstats.size(); i++) {
            res(col, i) = mstats[i].getStat(s);
        }
    }

    /**
     * @brief Mean extractor
     * @param s, index of the accumulator
     * @param the mean value of accu s
     */
    inline double mean(unsigned int s) const
    {
        return mstats[s].mean();
    }



    /**
     * @brief Mean vector extractor
     * @param the mean values
     */
    void mean(vle::value::Tuple& res) const
    {
        res.value().resize(mstats.size());
        for (unsigned int i = 0; i < res.size(); i++) {
            res[i] = mstats[i].mean();
        }
    }
    /**
     * @brief Quantile vector extractor
     * @param the vector of results
     * @param quantileOrder the quantile order
     */
    void quantile(vle::value::Tuple& res, double quantileOrder)
    {
        res.value().resize(mstats.size());
        for (unsigned int i = 0; i < res.size(); i++) {
            res[i] = mstats[i].quantile(quantileOrder);
        }
    }

    /**
     * @brief Count extractor
     * @return the number of MultiDim objects accumulated
     */
    inline unsigned int count()
    {
        return mstats[0].count();
    }

private:
    AccuType accu;
    bool init_size;
    std::vector<AccuMono> mstats;
};

}} //namespaces

#endif

