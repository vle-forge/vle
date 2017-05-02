/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <vle/manager/ExperimentGenerator.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/XML.hpp>
#include <vle/vpz/BaseModel.hpp>
#include <vle/vpz/Condition.hpp>
#include <vle/vpz/Vpz.hpp>

namespace vle {
namespace manager {

//
// Private implementation of the ExperimentGenerator.
//

class ExperimentGenerator::Pimpl
{
    Pimpl(const Pimpl& other);
    Pimpl& operator=(const Pimpl& other);

    int computeMaximumValue()
    {
        int result = 0;

        const vpz::Conditions& cnds(mVpz.project().experiment().conditions());

        auto it = cnds.conditionlist().begin();
        while (it != cnds.conditionlist().end()) {
            const vpz::Condition& cnd(it->second);
            vpz::ConditionValues::const_iterator jt;

            if (not cnd.conditionvalues().empty()) {
                for (jt = cnd.conditionvalues().begin();
                     jt != cnd.conditionvalues().end();
                     ++jt) {

                    int conditionsize = jt->second.size();

                    if (result == 0 or result == 1) {
                        result = conditionsize;
                    } else {
                        if (conditionsize != 0 and conditionsize != 1 and
                            result != conditionsize) {
                            throw utils::InternalError(
                              (fmt(_("ExperimentGenerator: bad combination "
                                     "size for the condition `%1%' port "
                                     "`%2%': %3%")) %
                               it->first % jt->first % conditionsize)
                                .str());
                        }
                    }
                }
                ++it;
            }
        }

        return result;
    }

    void computeRange()
    {
        mCompleteSize = computeMaximumValue();

        uint32_t number = mCompleteSize / mWorld;
        uint32_t modulo = mCompleteSize % mWorld;

        mMin =
          (number + 1) * std::min(mRank, modulo) +
          number * uint32_t(std::max((int32_t)0, int32_t(mRank - modulo)));
        mMax = std::min(mCompleteSize,
                        mMin + number + 1 * uint32_t(mRank < modulo));
    }

public:
    vpz::Vpz mVpz;
    uint32_t mRank;
    uint32_t mWorld;
    uint32_t mCompleteSize;
    uint32_t mMin;
    uint32_t mMax;

    Pimpl(const std::string& filename, uint32_t rank, uint32_t size)
      : mVpz(filename)
      , mRank(rank)
      , mWorld(size)
      , mCompleteSize(0)
      , mMin(0)
      , mMax(0)
    {
        if (rank >= size) {
            throw utils::InternalError(_("Bad rank"));
        }

        computeRange();
    }

    Pimpl(const vpz::Vpz& vpz, uint32_t rank, uint32_t size)
      : mVpz(vpz)
      , mRank(rank)
      , mWorld(size)
      , mCompleteSize(0)
      , mMin(0)
      , mMax(0)
    {
        if (rank >= size) {
            throw utils::InternalError(_("Bad rank"));
        }

        computeRange();
    }

    void get(uint32_t index, vpz::Conditions* conditions)
    {
        const vpz::Conditions& cnds(mVpz.project().experiment().conditions());
        conditions->deleteValueSet();
        vpz::ConditionList& cdldst(conditions->conditionlist());

        vpz::ConditionList::const_iterator it;
        for (it = cnds.begin(); it != cnds.end(); ++it) {

            std::pair<vpz::ConditionList::iterator, bool> r = cdldst.insert(
              std::make_pair(it->first, vpz::Condition(it->first)));

            const vpz::ConditionValues& cnvsrc = it->second.conditionvalues();
            vpz::ConditionValues& cnvdst = r.first->second.conditionvalues();

            for (const auto& elem : cnvsrc) {
                std::vector<std::shared_ptr<value::Value>> cpy;

                if (elem.second.size() == 1) {
                    cpy.emplace_back(elem.second[0]);
                } else if (elem.second.size() > 1 and
                           elem.second.size() > index) {
                    cpy.emplace_back(elem.second[index]);
                } else {
                    throw utils::InternalError(
                      (fmt(_("ExperimentGenerator can not access to the index"
                             " `%1%' of the condition `%2%' port `%3%' ")) %
                       index % it->first % elem.first)
                        .str());
                }

                cnvdst[elem.first] = std::move(cpy);
            }
        }
    }
};

//
// Public API of the ExperimentGenerator
//

ExperimentGenerator::ExperimentGenerator(const std::string& vpz,
                                         uint32_t rank,
                                         uint32_t size)
  : mPimpl(new ExperimentGenerator::Pimpl(vpz, rank, size))
{
}

ExperimentGenerator::ExperimentGenerator(const vpz::Vpz& vpz,
                                         uint32_t rank,
                                         uint32_t size)
  : mPimpl(new ExperimentGenerator::Pimpl(vpz, rank, size))
{
}

ExperimentGenerator::~ExperimentGenerator()
{
    delete mPimpl;
}

void
ExperimentGenerator::get(uint32_t index, vpz::Conditions* conditions)
{
    mPimpl->get(index, conditions);
}

uint32_t
ExperimentGenerator::min() const
{
    return mPimpl->mMin;
}

uint32_t
ExperimentGenerator::max() const
{
    return mPimpl->mMax;
}

uint32_t
ExperimentGenerator::size() const
{
    return mPimpl->mCompleteSize;
}
}
} // namespace vle manager
