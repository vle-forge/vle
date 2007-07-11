/** 
 * @file Observables.hpp
 * @brief 
 * @author The vle Development Team
 * @date ven, 15 jun 2007 17:46:11 +0200
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

#ifndef VLE_VPZ_OBSERVABLES_HPP
#define VLE_VPZ_OBSERVABLES_HPP

#include <vle/vpz/Observable.hpp>
#include <map>
#include <string>

namespace vle { namespace vpz {

    class Observables : public Base, public std::map < std::string, Observable >
    {
    public:
        Observables()
        { }

        virtual ~Observables()
        { }

        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return OBSERVABLES; }

        void add(const Observables& obs);

        Observable& add(const Observable& obs);

        Observable& get(const std::string& name);

        const Observable& get(const std::string& name) const;

        inline void del(const std::string& name)
        { erase(name); }

        inline bool exist(const std::string& name)
        { return find(name) != end(); }
        
        void clean_no_permanent();
    };

}} // namespace vle vpz

#endif
