/** 
 * @file vpz/Classes.hpp
 * @brief 
 * @author The vle Development Team
 * @date lun, 27 fév 2006 16:38:39 +0100
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

#ifndef VLE_VPZ_CLASSES_HPP
#define VLE_VPZ_CLASSES_HPP

#include <vle/vpz/Base.hpp>
#include <vle/vpz/Class.hpp>
#include <map>

namespace vle { namespace vpz {

    class Classes : public Base, public std::map < std::string, Class >
    {
    public:
        Classes() { }

        virtual ~Classes() { }

        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return CLASSES; }

        Class& add(const Class& c);

        void del(const std::string& name);

        const Class& get(const std::string& name) const;

        Class& get(const std::string& name);
    };

}} // namespace vle vpz

#endif
