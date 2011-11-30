/*
 * @file vle/manager/ExperimentGenerator.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#ifndef VLE_MANAGER_EXPERIMENTGENERATOR_HPP
#define VLE_MANAGER_EXPERIMENTGENERATOR_HPP

#include <vle/value/Value.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/manager/Types.hpp>
#include <vle/manager/DllDefines.hpp>
#include <string>
#include <vector>
#include <list>
#include <glibmm/thread.h>
#include <utility>

namespace vle { namespace manager {

    /**
     * @brief A class to translate Experiment file into Instance of Experiment.
     */
    class VLE_MANAGER_EXPORT ExperimentGenerator
    {
    public:
        /**
         * @brief Define a structure that stores the differences
         * between the default condition values and a combination
         */
        typedef std::pair < vpz::ConditionList::const_iterator,
                vpz::ConditionValues::const_iterator > diff_key_t;

        struct less_diff_keys
        {
            bool operator()(diff_key_t s1, diff_key_t s2) const
            {
                int comp = s1.first->first.compare(s2.first->first);

                if (comp == 0) {
                    comp = s1.second->first.compare(s2.second->first);
                }
                return comp < 0;
            }

        };

        typedef std::map < diff_key_t, int, less_diff_keys > diff_t;

        typedef std::vector < diff_t > Diffs;

        /**
         * @brief Build a experimental frame base on the const vpz::Vpz give in
         * parameters. It allows to build a combination of value from this
         * vpz::Vpz.
         * @param file A constant reference to a VPZ.
         * @param out Where to send output.
         * @param storecomb stores simulated combinations in order to access to
         * the function getInputFromCombination.
         * @param rnd A reference random number generator.
         */
        ExperimentGenerator(const vpz::Vpz & file, std::ostream & out,
                            bool storecomb);

        virtual ~ExperimentGenerator();

        /**
         * @brief Get the list of filename build after build.
         * @return the list of filename.
         */
        std::list < vpz::Vpz* >& vpzInstances()
        {
            return mFileList;
        }

        void build(OutputSimulationList* matrix);

        void build(Glib::Mutex* mutex, Glib::Cond* prod,
                   OutputSimulationList* matrix);

        /**
         * @brief Save the generated VPZ instance file.
         *
         * @param save true to save the files, otherwise, this file are remove.
         * The default option is to remove file.
         */
        void saveVPZinstance(bool save = false)
        {
            mSaveVpz = save;
        }

        /**
         * @brief Get the input value used for simulating a given combination
         *
         * @param comb number of combination
         * @param condition name of a condition
         * @param port name of a port from the condition
         * @return the value, on port port_name of condition condition_name,
         *  used for simulation of combination comb
         */
        const value::Value&
            getInputFromCombination(unsigned int       comb,
                                    const std::string& condition,
                                    const std::string& port) const;

        const Diffs getCombinations() const
        {
            return mCombinationDiff;
        }

    protected:
        /**
         * @brief Build just one condition based on information of VPZ file.
         */
        virtual void buildCombination(size_t& nb) = 0;

        /**
         * @brief Get the number of combination from vpz file.
         * @return A value greater than 0.
         */
        virtual size_t getCombinationNumber() const = 0;

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        /**
         * @brief Build a list of conditions based on information of VPZ file.
         */
        void buildConditionsList();

        void buildCombinations();

        void buildCombinationsFromReplicas(size_t cmbnumber);

        /**
         * @brief Build an instance of the current specified vpz file and push
         * it into the output list. This function use the Glib::Mutex and
         * Glib::Cond to protect the access to data.
         * @param cmbnumber the index of the combination.
         */
        void writeInstanceThread(size_t cmbnumber);

        /**
         * @brief Build an instance of the current specified vpz file and push
         * it into the output list.
         * @param cmbnumber the index of the combination.
         */
        void writeInstance(size_t cmbnumber);

    protected:
        /**
         * @brief Define a condition used to generate combination list.
         */
        struct cond_t {
            cond_t() :
                sz(0), pos(0)
            {
            }

            size_t sz;
            size_t pos;
        };

        void cleanCondition(vpz::Vpz& file);


        const vpz::Vpz& mFile;
        vpz::Vpz mTmpfile;
        std::ostream& mOut;
        std::vector < cond_t > mCondition;
        std::vector < diff_t > mCombinationDiff;
        std::list < vpz::Vpz* > mFileList;
        bool mSaveVpz;
        bool mStoreComb;
        OutputSimulationList* mMatrix;
        Glib::Mutex* mMutex;
        Glib::Cond* mProdcond;
    };

}} // namespace vle manager

#endif
