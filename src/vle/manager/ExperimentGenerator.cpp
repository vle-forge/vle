/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
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
#include <vle/vpz/Condition.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/vpz/BaseModel.hpp>

namespace vle { namespace manager {

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

        vpz::ConditionList::const_iterator it = cnds.conditionlist().begin();
        while (it != cnds.conditionlist().end()) {
            const vpz::Condition& cnd(it->second);
            vpz::ConditionValues::const_iterator jt;

            if (not cnd.conditionvalues().empty()) {
                for (jt = cnd.conditionvalues().begin(); jt !=
                     cnd.conditionvalues().end(); ++jt) {

                    int conditionsize = jt->second->size();

                    if (result == 0 or result == 1) {
                        result = conditionsize;
                    } else {
                        if (conditionsize != 0 and conditionsize != 1
                            and result != conditionsize) {
                            throw utils::InternalError(
                                fmt(_("ExperimentGenerator: bad combination "
                                      "size for the condition `%1%' port "
                                      "`%2%': %3%")) % it->first % jt->first %
                                conditionsize);
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

        mMin = (number+1) * std::min(mRank, modulo) + number *
                uint32_t(std::max((int32_t) 0, int32_t(mRank-modulo)));
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
        : mVpz(filename), mRank(rank), mWorld(size), mCompleteSize(0), mMin(0),
        mMax(0)
    {
        if (rank >= size) {
            throw utils::InternalError(_("Bad rank"));
        }

        computeRange();
    }

    Pimpl(const vpz::Vpz& vpz, uint32_t rank, uint32_t size)
        : mVpz(vpz), mRank(rank), mWorld(size), mCompleteSize(0), mMin(0),
        mMax(0)
    {
        if (rank >= size) {
            throw utils::InternalError(_("Bad rank"));
        }

        computeRange();
    }

    ~Pimpl()
    {
        delete mVpz.project().model().model();
    }

    void get(uint32_t index, vpz::Conditions *conditions)
    {
        const vpz::Conditions& cnds(mVpz.project().experiment().conditions());
        conditions->deleteValueSet();
        vpz::ConditionList& cdldst(conditions->conditionlist());

        vpz::ConditionList::const_iterator it;
        for (it = cnds.begin(); it != cnds.end(); ++it) {

            std::pair < vpz::ConditionList::iterator, bool > r =
                cdldst.insert(std::make_pair(
                        it->first, vpz::Condition(it->first)));

            const vpz::ConditionValues& cnvsrc = it->second.conditionvalues();
            vpz::ConditionValues& cnvdst = r.first->second.conditionvalues();

            for (vpz::ConditionValues::const_iterator jt = cnvsrc.begin();
                 jt != cnvsrc.end(); ++jt) {

                value::Set *cpy = new value::Set();

                if (jt->second->size() == 1) {
                    cpy->add(jt->second->get(0)->clone());
                } else if (jt->second->size() > 1 and jt->second->size() >
                           index) {
                    cpy->add(jt->second->get(index)->clone());
                } else {
                    throw utils::InternalError(fmt(
                            _("ExperimentGenerator can not access to the index"
                              " `%1%' of the condition `%2%' port `%3%' ")) %
                        index % it->first % jt->first);
                }

                delete cnvdst[jt->first];
                cnvdst[jt->first] = cpy;
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

ExperimentGenerator::ExperimentGenerator(const vpz::Vpz& vpz, uint32_t rank,
                                         uint32_t size)
    : mPimpl(new ExperimentGenerator::Pimpl(vpz, rank, size))
{
}

ExperimentGenerator::~ExperimentGenerator()
{
    delete mPimpl;
}

void ExperimentGenerator::get(uint32_t index, vpz::Conditions *conditions)
{
    mPimpl->get(index, conditions);
}

uint32_t ExperimentGenerator::min() const
{
    return mPimpl->mMin;
}

uint32_t ExperimentGenerator::max() const
{
    return mPimpl->mMax;
}

uint32_t ExperimentGenerator::size() const
{
    return mPimpl->mCompleteSize;
}

}}  // namespace vle manager
