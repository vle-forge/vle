/** 
 * @file PathOptionGroup.hpp
 * @brief 
 * @author The vle Development Team
 * @date 2007-09-07
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
