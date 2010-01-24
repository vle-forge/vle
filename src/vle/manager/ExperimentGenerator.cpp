/**
 * @file vle/manager/ExperimentGenerator.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#include <vle/manager/ExperimentGenerator.hpp>
#include <vle/vpz/Conditions.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/value/Set.hpp>
#include <fstream>
#include <iostream>

namespace vle { namespace manager {

ExperimentGenerator::ExperimentGenerator(const vpz::Vpz& file,
                                         std::ostream& out, bool storecomb,
                                         bool commonseed, RandPtr rnd)
    : mFile(file), mTmpfile(file), mOut(out), mSaveVpz(false),
    mStoreComb(storecomb), mCommonSeed(commonseed), mMutex(0), mProdcond(0),
    mRand(rnd)
{
    vpz::Conditions& dest(mTmpfile.project().experiment().conditions());
    vpz::ConditionList::const_iterator itDest(dest.conditionlist().begin());
    vpz::ConditionValues::const_iterator itValueDest(
        itDest->second.conditionvalues().begin());

    const vpz::Conditions& orig(mFile.project().experiment().conditions());
    vpz::ConditionList::const_iterator itOrig(orig.conditionlist().begin());
    vpz::ConditionValues::const_iterator itValueOrig(
        itOrig->second.conditionvalues().begin());

    mTmpfile.project().experiment().conditions().rebuildValueSet();
}

ExperimentGenerator::~ExperimentGenerator()
{
    if (mTmpfile.project().model().model()) {
        delete mTmpfile.project().model().model();
    }
}

void ExperimentGenerator::build(OutputSimulationMatrix* matrix)
{
    mMatrix = matrix;

    buildReplicasList();
    buildConditionsList();

    OutputSimulationMatrix::extent_gen extent;

    mOut << fmt(_("Generator: build (%1% rep. x %2% cmb.)\n"))
        % mReplicasTab.size() % getCombinationNumber();
    mMatrix->resize(extent[mReplicasTab.size()][getCombinationNumber()]);

    buildCombinations();
}

void ExperimentGenerator::build(Glib::Mutex* mutex, Glib::Cond* prod,
                                OutputSimulationMatrix* matrix)
{
    mMutex = mutex;
    mProdcond = prod;
    mMatrix = matrix;

    buildReplicasList();
    buildConditionsList();
    {
        Glib::Mutex::Lock lock(*mMutex);
        OutputSimulationMatrix::extent_gen extent;
        mOut << fmt(_("Generator: build (%1% rep. x %2% cmb.)\n"))
            % mReplicasTab.size() % getCombinationNumber();
        mMatrix->resize(extent[mReplicasTab.size()][getCombinationNumber()]);
    }

    buildCombinations();
}

void ExperimentGenerator::buildReplicasList()
{
    if (mFile.project().experiment().replicas().number() <= 0) {
        throw utils::ArgError(
            _("The replicas's tag is not defined in the vpz file"));
    }

    mReplicasTab.resize(mFile.project().experiment().replicas().number());
    for (std::vector < guint32 >::iterator it = mReplicasTab.begin(); it
         != mReplicasTab.end(); ++it) {
        if (mCommonSeed) {
            *it = mRand->getInt();
        } else {
            *it = 0;
        }
    }
}

void ExperimentGenerator::buildConditionsList()
{
    const vpz::Conditions& cnds(mFile.project().experiment().conditions());
    vpz::ConditionList::const_iterator it;

    for (it = cnds.conditionlist().begin(); it != cnds.conditionlist().end();
         ++it) {
        const vpz::Condition& cnd(it->second);
        vpz::ConditionValues::const_iterator jt;

        if (not cnd.conditionvalues().empty()) {
            for (jt = cnd.conditionvalues().begin(); jt
                 != cnd.conditionvalues().end(); ++jt) {
                mCondition.push_back(cond_t());

                mCondition[mCondition.size() - 1].sz = jt->second->size();
                mCondition[mCondition.size() - 1].pos = 0;
            }
        } else {
            mCondition.push_back(cond_t());
            mCondition[mCondition.size() - 1].sz = 1;
            mCondition[mCondition.size() - 1].pos = 0;
        }
    }
}

void ExperimentGenerator::buildCombinations()
{
    if (mStoreComb) {
        mCombinationDiff.resize(getCombinationNumber());
    }
    size_t nb = 0;
    do {
        mTmpfile.project().experiment().conditions().rebuildValueSet();
        buildCombinationsFromReplicas(nb);
        buildCombination(nb);
    } while (nb < getCombinationNumber());
}

void ExperimentGenerator::buildCombinationsFromReplicas(size_t cmbnumber)
{
    vpz::Conditions& dest(mTmpfile.project().experiment().conditions());
    vpz::ConditionList::const_iterator itDest(dest.conditionlist().begin());
    vpz::ConditionValues::const_iterator itValueDest(
        itDest->second.conditionvalues().begin());

    const vpz::Conditions& orig(mFile.project().experiment().conditions());
    vpz::ConditionList::const_iterator itOrig(orig.conditionlist().begin());
    vpz::ConditionValues::const_iterator itValueOrig(
        itOrig->second.conditionvalues().begin());

    if (dest.conditionlist().size() != orig.conditionlist().size()) {
        throw utils::InternalError(fmt(
                _("Error: %1% %2% %3%\n")) % dest.conditionlist().size() %
            orig.conditionlist().size() % mCondition.size());
    }

    for (size_t jcom = 0; jcom < mCondition.size(); ++jcom) {
        if (not itOrig->second.conditionvalues().empty()) {
            size_t index = mCondition[jcom].pos;
            value::Value& val = itValueOrig->second->get(index);

            if (mStoreComb && (index != 0)) {
                diff_key_t key(itOrig, itValueOrig);
                mCombinationDiff[cmbnumber].insert(
                    std::pair < diff_key_t, int >(key, index));
            }
            itValueDest->second->clear();
            itValueDest->second->add(val);

            itValueDest++;
            itValueOrig++;

            if (itValueDest == itDest->second.conditionvalues().end()) {

                if (itValueOrig != itOrig->second.conditionvalues().end()) {
                    throw utils::InternalError(fmt(
                            _("Error: %1% %2%\n")) %
                        itDest->second.conditionvalues().size() %
                        itOrig->second.conditionvalues().size());
                }

                itDest++;
                itOrig++;
                itValueDest = itDest->second.conditionvalues().begin();
                itValueOrig = itOrig->second.conditionvalues().begin();
            }
        } else {
            itDest++;
            itOrig++;
            itValueDest = itDest->second.conditionvalues().begin();
            itValueOrig = itOrig->second.conditionvalues().begin();
        }
    }

    if (mMutex == 0) {
        for (size_t irep = 0; irep < mReplicasTab.size(); ++irep) {
            if (mCommonSeed) {
                mTmpfile.project().experiment().setSeed(mReplicasTab[irep]);
            } else {
                mTmpfile.project().experiment().setSeed(mRand->getInt());
            }
            writeInstance(cmbnumber, irep);
        }
    } else {
        for (size_t irep = 0; irep < mReplicasTab.size(); ++irep) {
            if (mCommonSeed) {
                mTmpfile.project().experiment().setSeed(mReplicasTab[irep]);
            } else {
                mTmpfile.project().experiment().setSeed(mRand->getInt());
            }
            writeInstanceThread(cmbnumber, irep);
        }
    }

}

void ExperimentGenerator::writeInstance(size_t cmbnumber, size_t replnumber)
{
    std::string expname(
        (fmt(_("%1%-%2%-%3%")) % mFile.project().experiment().name()
         % cmbnumber % replnumber).str());

    mTmpfile.project().experiment().setName(expname);
    vpz::Vpz* newvpz = new vpz::Vpz(mTmpfile);
    newvpz->project().setInstance(cmbnumber);
    newvpz->project().setReplica(replnumber);

    if (mSaveVpz) {
        std::string filename(expname);
        filename += ".vpz";
        std::ofstream output(filename.c_str());
        newvpz->write(output);
    }

    mFileList.push_back(newvpz);
}

void ExperimentGenerator::writeInstanceThread(size_t cmbnumber,
                                              size_t replnumber)
{
    std::string expname(
        (fmt(_("%1%-%2%-%3%")) % mFile.project().experiment().name()
         % cmbnumber % replnumber).str());

    mTmpfile.project().experiment().setName(expname);
    vpz::Vpz* newvpz = new vpz::Vpz(mTmpfile);
    newvpz->project().setInstance(cmbnumber);
    newvpz->project().setReplica(replnumber);

    if (mSaveVpz) {
        std::string filename(expname);
        filename += ".vpz";
        std::ofstream output(filename.c_str());
        newvpz->write(output);
    }

    {
        Glib::Mutex::Lock lock(*mMutex);
        mFileList.push_back(newvpz);
        mProdcond->signal();
    }
}

size_t ExperimentGenerator::getReplicasNumber() const
{
    return mReplicasTab.size();
}

const value::Value&
ExperimentGenerator::getInputFromCombination(unsigned int comb,
                                             const std::string& condition, const std::string& port) const
{
    const vpz::ConditionList& orig(
        mFile.project().experiment().conditions().conditionlist());
    vpz::ConditionList::const_iterator itOrig = orig.find(condition);

    if (itOrig == orig.end()) {
        throw utils::ArgError(fmt(_("Condition %1% does not exist."))
                              % condition);
    }
    const vpz::ConditionValues& origValues(itOrig->second.conditionvalues());
    vpz::ConditionValues::const_iterator itOrigValues = origValues.find(port);
    if (itOrigValues == origValues.end()) {
        throw utils::ArgError(fmt(_("Port %1% does not exist.")) % port);
    }
    diff_key_t key(itOrig, itOrigValues);
    if (comb > mCombinationDiff.size() - 1) {
        throw utils::ArgError(fmt(_("Combination number %1% does not exist."))
                              % comb);
    }
    diff_t::const_iterator itDiff = mCombinationDiff[comb].find(key);

    if (itDiff != mCombinationDiff[comb].end()) {
        return itOrigValues->second->get(itDiff->second);
    } else {
        return itOrigValues->second->get(0);
    }
}

}}  // namespace vle manager
