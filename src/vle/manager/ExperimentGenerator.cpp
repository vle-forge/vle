/** 
 * @file manager/ExperimentGenerator.cpp
 * @brief A class to translate Experiement file into Instance of Experiment.
 * @author The vle Development Team
 * @date lun, 23 jan 2006 14:00:40 +0100
 */

/*
 * Copyright (C) 2004, 05, 06 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <vle/manager/ExperimentGenerator.hpp>
#include <vle/vpz/Conditions.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/value/Set.hpp>
#include <iostream>
#include <fstream>



namespace vle { namespace manager {

ExperimentGenerator::ExperimentGenerator(const vpz::Vpz& file) :
    mFile(file),
    mTmpfile(file),
    mSaveVpz(false)
{
    mTmpfile.project().experiment().conditions().rebuildValueSet();
}

void ExperimentGenerator::build()
{
    build_replicas_list();
    build_conditions_list();
    build_combinations();
}

void ExperimentGenerator::build_replicas_list()
{
    std::cerr << "Replicas: ";
    mReplicasTab = mFile.project().experiment().replicas().getList();
    if (mReplicasTab.empty()) {
        std::cerr << "/!\\ no defined (experiment seed is used): ";
        mReplicasTab.push_back(mFile.project().experiment().seed());
    }
    std::cerr << mReplicasTab.size() << std::endl;
}

void ExperimentGenerator::build_conditions_list()
{
    const vpz::Conditions& cnds(mFile.project().experiment().conditions());
    vpz::Conditions::const_iterator it;

    for (it = cnds.begin(); it != cnds.end(); ++it) {
        const vpz::Condition& cnd(it->second);
        vpz::Condition::const_iterator jt;

        for (jt = cnd.begin(); jt != cnd.end(); ++jt) {
            mCondition.push_back(cond_t());
            mCondition[mCondition.size() - 1].sz = jt->second->size();
            mCondition[mCondition.size() - 1].pos = 0;
        }
    }
    std::cerr << "Combinations: " << get_combination_number() << std::endl;
}

void ExperimentGenerator::build_combinations()
{
    size_t nb = 0;

    do {
        build_combinations_from_replicas(nb);
        build_combination(nb);
    } while (nb < get_combination_number()); 
}

void ExperimentGenerator::build_combinations_from_replicas(size_t cmbnumber)
{
    vpz::Conditions& dest(mTmpfile.project().experiment().conditions());
    vpz::Conditions::iterator itDest(dest.begin());
    vpz::Condition::iterator itValueDest(itDest->second.begin());

    const vpz::Conditions& orig(mFile.project().experiment().conditions());
    vpz::Conditions::const_iterator itOrig(orig.begin());
    vpz::Condition::const_iterator itValueOrig(itOrig->second.begin());

    Assert(utils::InternalError,
           dest.size() == orig.size(),
           boost::format("Error: %1% %2% %3%\n") % dest.size() %
           orig.size() % mCondition.size());

    for (size_t jcom = 0; jcom < mCondition.size(); ++jcom) {
        size_t index = mCondition[jcom].pos;
        value::Value val = itValueOrig->second->getValue(index);
        itValueDest->second->clear();
        itValueDest->second->addValue(val);

        itValueDest++;
        itValueOrig++;

        if (itValueDest == itDest->second.end()) {
            Assert(utils::InternalError, itValueOrig == itOrig->second.end(),
                   boost::format("Error: %1% %2%\n") % itDest->second.size() %
                   itOrig->second.size());
            itDest++;
            itOrig++;
            itValueDest = itDest->second.begin();
            itValueOrig = itOrig->second.begin();
        }
    }

    for (size_t irep = 0; irep < mReplicasTab.size(); ++irep) {
        mTmpfile.project().experiment().setSeed(mReplicasTab[irep]);
        write_instance(cmbnumber, irep);
    }
}

void ExperimentGenerator::write_instance(size_t cmbnumber, size_t replnumber)
{
    std::string expname(
        (boost::format("%1%-%2%-%3%") % mFile.project().experiment().
         name() % cmbnumber % replnumber).str());

    mTmpfile.project().experiment().setName(expname);

    std::string buffer(mTmpfile.writeToString());

    Glib::ustring filename(utils::write_to_temp("vleexp", buffer));

    std::cerr << (boost::format(
            "Writing file: %1% %2%/%3%\n") % filename % cmbnumber % replnumber);
    if (mSaveVpz) {
        expname += ".vpz";
        std::ofstream file(expname.c_str());
        file << buffer;
    }

    mFileList.push_back(filename);
}

size_t ExperimentGenerator::get_replicas_number() const
{
    return mReplicasTab.size();
}

}} // namespace vle manager
