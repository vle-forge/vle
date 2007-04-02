/** 
 * @file manager/ExperimentGenerator.hpp
 * @brief A class to translate Experiement file into Instance of Experiment.
 * @author The vle Development Team
 * @date lun, 23 jan 2006 14:01:42 +0100
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

#ifndef VLE_MANAGER_EXPERIMENTGENERATOR_HPP
#define VLE_MANAGER_EXPERIMENTGENERATOR_HPP

#include <vle/value/Value.hpp>
#include <vle/vpz/Vpz.hpp>
#include <string>
#include <vector>
#include <list>



namespace vle { namespace manager {

    /** 
     * @brief A class to translate Experiment file into Instance of Experiment.
     */
    class ExperimentGenerator
    {
    public:
        /**
         * Just get a constant reference to VPZ. Use get_instances_files() to
         * generate all VPZ instance file.
         *
         */
        ExperimentGenerator(const vpz::Vpz& file);

        virtual ~ExperimentGenerator() { }

        /**
         * @brief Build all instances function.
         */
        void build();

        /**
         * @brief Get the list of filename build after build.
         * @return the list of filename.
         */
        const std::list < std::string >& get_instances_files() const
        { return mFileList; }

        /** 
         * @brief Save the generated VPZ instance file.
         * 
         * @param save true to save the files, otherwise, this file are remove.
         * The default option is to remove file.
         */
        void saveVPZinstance(bool save = false)
        { mSaveVpz = save; }

    private:
        /** 
         * @brief Build a list of replicas based on information of VPZ file.
         */
        void build_replicas_list();

        /** 
         * @brief Build a list of conditions based on information of VPZ file.
         */
        void build_conditions_list();

        /** 
         * @brief Build just one condition based on information of VPZ file.
         */
        virtual void build_combination(size_t& nb) = 0;

        void build_combinations();

        void build_combinations_from_replicas(size_t cmbnumber);

        void write_instance(size_t cmbnumber, size_t replnumber);

        /** 
         * @brief Get the number of combination from vpz file.
         * 
         * @return A value greater than 0.
         */
        virtual size_t get_combination_number() const = 0;

        /** 
         * @brief Get the number of replicas from vpz file.
         * 
         * @return A value greater than 0.
         */
        size_t get_replicas_number() const;

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    protected:
        /** 
         * @brief Define a condition used to generate combination list.
         */
        struct cond_t {
            cond_t() : sz(0), pos(0) { }

            size_t  sz;
            size_t  pos;
        };

        const vpz::Vpz&             mFile;
        vpz::Vpz                    mTmpfile;
        std::vector < guint32 >     mReplicasTab;
        std::vector < cond_t >      mCondition;
        std::list < std::string >   mFileList;
        bool                        mSaveVpz;
    };

}} // namespace vle manager

#endif
