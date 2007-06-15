/** 
 * @file vle/vpz/tr1.hpp
 * @brief Translator test.
 * @author The vle Development Team
 * @date jeu, 14 jun 2007 18:48:27 +0200
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

#include <vle/vpz/test/tr1.hpp>
#include <iostream>

namespace vle { namespace vpz { namespace testunit {

void tr1::translate(const std::string& buffer)
{
    m_dynamics.add(Dynamic("dyn1"));
    m_conditions.add(Condition("cond1"));
    m_measures.addTextStreamOutput("output1");
    m_measures.addEventMeasure("measure1", "output1");
    m_measures.addObservableToMeasure("measure1", "obs1");
}

}}} // namespace vle vpz testunit
