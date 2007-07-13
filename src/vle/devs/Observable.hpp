/** 
 * @file Observable.hpp
 * @brief 
 * @author The vle Development Team
 * @date 2007-07-13
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

#ifndef VLE_DEVS_OBSERVABLE_HPP
#define VLE_DEVS_OBSERVABLE_HPP

#include <vector>
#include <string>

namespace vle { namespace devs {

    class Simulator;
    
    class Observable
    {
    public:
        Observable(Simulator* model, const std::string& portname) :
            m_model(model),
            m_portname(portname)
        { }

        inline Simulator* simulator() const
        { return m_model; }

        inline const std::string& portname() const
        { return m_portname; }

    private:
        Observable() :
            m_model(0)
        { }
        
        Simulator*      m_model;
	std::string     m_portname;
    };
    
    typedef std::vector < Observable > ObservableList;


}} // namespace vle devs

#endif
