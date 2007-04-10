/** 
 * @file manager/LinearExperimentGenerator.hpp
 * @brief A class to translate Experiement file into Instance of Experiment.
 * @author The vle Development Team
 * @date jeu, 04 avr 2007 11:19:02 +0100
 */

/*
 * Copyright (C) 2007 - The vle Development Team
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

#ifndef VLE_MANAGER_LINEAREXPERIMENTGENERATOR_HPP
#define VLE_MANAGER_LINEAREXPERIMENTGENERATOR_HPP

#include <vle/manager/ExperimentGenerator.hpp>



namespace vle { namespace manager {

    /** 
     * @brief A class to translate Experiment file into Instance of Experiment.
     */
    class LinearExperimentGenerator : public ExperimentGenerator
    {
    public:
        /**
         * Just get a constant reference to VPZ. Use get_instances_files() to
         * generate all VPZ instance file.
         *
         */
        LinearExperimentGenerator(const vpz::Vpz& file) :
            ExperimentGenerator(file)
        { }

        virtual ~LinearExperimentGenerator()
        { }

        virtual void build_combination(size_t& nb);

        /** 
         * @brief Get the number of combination from vpz file.
         * 
         * @return A value greater than 0.
         */
        virtual size_t get_combination_number() const;
    };

}} // namespace vle manager

#endif
