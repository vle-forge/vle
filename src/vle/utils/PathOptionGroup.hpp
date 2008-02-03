/**
 * @file vle/utils/PathOptionGroup.hpp
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


#ifndef VLE_UTILS_PATH_OPTION_GROUP_HPP
#define VLE_UTILS_PATH_OPTION_GROUP_HPP

#include <glibmm/optioncontext.h>



namespace vle { namespace utils {

    class PathOptionGroup : public Glib::OptionGroup
    {
    public:
        PathOptionGroup();

        virtual ~PathOptionGroup();

        /** 
         * @brief Assign all the paths to the utils::Path singleton.
         */
        void assignToPath();

    private:
        Glib::OptionGroup::vecustrings m_all;
        Glib::OptionGroup::vecustrings m_simulator;
        Glib::OptionGroup::vecustrings m_stream;
        Glib::OptionGroup::vecustrings m_model;
    };

}} // namespace vle utils

#endif
