/** 
 * @file vpz/Base.hpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:16:52 +0100
 */

/*
 * Copyright (C) 2006 - The vle Development Team
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

#ifndef VLE_VPZ_BASE_HPP
#define VLE_VPZ_BASE_HPP

#include <libxml++/libxml++.h>

namespace vle { namespace vpz {

    /** 
     * @brief A base to class to force all inheritance classes to load and
     * write from an XML reference.
     */
    class Base
    {
    public:
        Base() { }

        virtual ~Base() { }

        /** 
         * @brief initialise this object with the XML provide by the elt
         * reference.
         * 
         * @param elt a reference to the parent tag or current tag.
         */
        virtual void init(xmlpp::Element* elt) = 0;
        
        /** 
         * @brief add this object information under the elt reference.
         * 
         * @param elt a reference to the EXPERIMENT tag.
         */
        virtual void write(xmlpp::Element* elt) const = 0;
    };

}} // namespace vle vpz

#endif
