/**
 * @file src/vle/manager/ExperimentGenerator.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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
#include <vle/utils/XML.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/value/Set.hpp>
#include <iostream>
#include <fstream>



namespace vle { namespace manager {

ExperimentGenerator::ExperimentGenerator(const vpz::Vpz& file,
                                         std::ostream& out,
                                         RandPtr rnd) :
    mFile(file),
    mTmpfile(file),
    mOut(out),
    mSaveVpz(false),
    mMutex(0),
    mProdcond(0),
    mRand(rnd)
{
    vpz::Conditions& dest(mTmpfile.project().experiment().conditions());
    vpz::ConditionList::const_iterator itDest(dest.conditionlist().begin());
    vpz::ConditionValues::const_iterator
        itValueDest(itDest->second.conditionvalues().begin());

    const vpz::Conditions& orig(mFile.project().experiment().conditions());
    vpz::ConditionList::const_iterator itOrig(orig.conditionlist().begin());
    vpz::ConditionValues::const_iterator
        itValueOrig(itOrig->second.conditionvalues().begin());

    mTmpfile.project().experiment().conditions().rebuildValueSet();
}

void ExperimentGenerator::build(OutputSimulationMatrix* matrix)
{
    mMatrix = matrix;

    buildReplicasList();
    buildConditionsList();

    OutputSimulationMatrix::extent_gen extent;

    mOut << boost::format("Generator: build (%1% rep. x %2% cmb.)\n") %
        mReplicasTab.size() % getCombinationNumber();
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
        mOut << boost::format("Generator: build (%1% rep. x %2% cmb.)\n") %
            mReplicasTab.size() % getCombinationNumber();
        mMatrix->resize(extent[mReplicasTab.size()][getCombinationNumber()]);
    }

    buildCombinations();
}

void ExperimentGenerator::buildReplicasList()
{
    Assert(utils::ArgError, mFile.project().experiment().replicas().number() > 0,
           "The replicas's tag is not defined in the vpz file");

    mReplicasTab.resize(mFile.project().experiment().replicas().number());
    for (std::vector < guint32 >::iterator it = mReplicasTab.begin();
         it != mReplicasTab.end(); ++it) {
        *it = mRand->get_int();
    }
}

void ExperimentGenerator::buildConditionsList()
{
    const vpz::Conditions& cnds(mFile.project().experiment().conditions());
    vpz::ConditionList::const_iterator it;

    for (it = cnds.conditionlist().begin();
         it != cnds.conditionlist().end(); ++it) {
        const vpz::Condition& cnd(it->second);
        vpz::ConditionValues::const_iterator jt;

        for (jt = cnd.conditionvalues().begin();
             jt != cnd.conditionvalues().end(); ++jt) {
            mCondition.push_back(cond_t());

            mCondition[mCondition.size() - 1].sz = jt->second->size();
            mCondition[mCondition.size() - 1].pos = 0;
        }
    }
}

void ExperimentGenerator::buildCombinations()
{
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
    vpz::ConditionValues::const_iterator
        itValueDest(itDest->second.conditionvalues().begin());

    const vpz::Conditions& orig(mFile.project().experiment().conditions());
    vpz::ConditionList::const_iterator itOrig(orig.conditionlist().begin());
    vpz::ConditionValues::const_iterator
        itValueOrig(itOrig->second.conditionvalues().begin());

    Assert(utils::InternalError,
           dest.conditionlist().size() == orig.conditionlist().size(),
           boost::format("Error: %1% %2% %3%\n") % dest.conditionlist().size() %
           orig.conditionlist().size() % mCondition.size());

    for (size_t jcom = 0; jcom < mCondition.size(); ++jcom) {
        size_t index = mCondition[jcom].pos;
        value::Value val = itValueOrig->second->getValue(index);
        itValueDest->second->clear();
        itValueDest->second->addValue(val);

        itValueDest++;
        itValueOrig++;

        if (itValueDest == itDest->second.conditionvalues().end()) {
            Assert(utils::InternalError, itValueOrig ==
                   itOrig->second.conditionvalues().end(),
                   boost::format("Error: %1% %2%\n") %
                   itDest->second.conditionvalues().size() %
                   itOrig->second.conditionvalues().size());
            itDest++;
            itOrig++;
            itValueDest = itDest->second.conditionvalues().begin();
            itValueOrig = itOrig->second.conditionvalues().begin();
        }
    }

    if (mMutex == 0) {
        for (size_t irep = 0; irep < mReplicasTab.size(); ++irep) {
            mTmpfile.project().experiment().setSeed(mReplicasTab[irep]);
            writeInstance(cmbnumber, irep);
        }
    } else {
        for (size_t irep = 0; irep < mReplicasTab.size(); ++irep) {
            mTmpfile.project().experiment().setSeed(mReplicasTab[irep]);
            writeInstanceThread(cmbnumber, irep);
        }
    }
}

void ExperimentGenerator::writeInstance(size_t cmbnumber, size_t replnumber)
{
    std::string expname(
        (boost::format("%1%-%2%-%3%") % mFile.project().experiment().
         name() % cmbnumber % replnumber).str());

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


void ExperimentGenerator::writeInstanceThread(size_t cmbnumber, size_t replnumber)
{
    std::string expname(
        (boost::format("%1%-%2%-%3%") % mFile.project().experiment().
         name() % cmbnumber % replnumber).str());

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

}} // namespace vle manager
