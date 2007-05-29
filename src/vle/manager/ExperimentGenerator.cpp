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
}

void ExperimentGenerator::build()
{
    build_replicas_list();
    build_conditions_list();
    build_combinations();
}

void ExperimentGenerator::build_replicas_list()
{
    mReplicasTab = mFile.project().experiment().replicas().getList();
    std::cerr << "Replicas: " << mReplicasTab.size() << std::endl;
}

void ExperimentGenerator::build_conditions_list()
{
    std::cerr << "Combinations: " << get_combination_number() << std::endl;

    const vpz::Conditions::ConditionList& conds(
        mFile.project().experiment().conditions().conditions());

    for (vpz::Conditions::const_iterator it = conds.begin();
         it != conds.end(); ++it) {

        const vpz::Condition::ValueList& values(it->second.values());
        for (vpz::Condition::const_iterator jt = values.begin();
             jt != values.end(); ++jt) {

            mCondition.push_back(cond_t());
            mCondition[mCondition.size() - 1].sz = jt->second->size();
            mCondition[mCondition.size() - 1].pos = 0;
        }
    }
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
    for (size_t irep = 0; irep < mReplicasTab.size(); ++irep) {
        mTmpfile.project().experiment().setSeed(mReplicasTab[irep]);

        vpz::Conditions::ConditionList& dest(
            mTmpfile.project().experiment().conditions().conditions());
        vpz::Conditions::iterator itDest(dest.begin());
        vpz::Condition::iterator itValueDest(itDest->second.values().begin());

        const vpz::Conditions::ConditionList& orig(
            mFile.project().experiment().conditions().conditions());
        vpz::Conditions::const_iterator itOrig(orig.begin());
        vpz::Condition::const_iterator
            itValueOrig(itOrig->second.values().begin());

        Assert(utils::InternalError,
               dest.size() == orig.size(),
               boost::format("Error: %1% %2% %3%\n") % dest.size() %
               orig.size() % mCondition.size());

        for (size_t jcom = 0; jcom < mCondition.size(); ++jcom) {
            itValueDest->second->clear();
            itValueDest->second->addValue(
                itValueOrig->second->getValue(mCondition[jcom].pos));
            //(*itDest).addValue((*itOrig).nValue(mCondition[jcom].pos));

            itValueDest++;
            itValueOrig++;

            if (itValueDest == itDest->second.values().end()) {
                Assert(utils::InternalError, itValueOrig ==
                       itOrig->second.values().end(),
                       boost::format("Error: %1% %2%\n") %
                       itDest->second.values().size() %
                       itOrig->second.values().size());
                itDest++;
                itOrig++;
                itValueDest = itDest->second.values().begin();
                itValueOrig = itOrig->second.values().begin();
            }
        }
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

    std::cerr << "Writing file: " << filename << " " << mSaveVpz << std::endl;
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
