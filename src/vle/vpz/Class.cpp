/*
 * @file vle/vpz/Class.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
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


#include <vle/vpz/Class.hpp>
#include <algorithm>

namespace vle { namespace vpz {

Class::Class(const Class& cls) :
    Base(cls),
    m_name(cls.m_name)
{
    if (cls.m_model == 0) {
        m_model = 0;
    } else {
        m_model = cls.m_model->clone();
        std::for_each(cls.m_atomicmodels.begin(), cls.m_atomicmodels.end(),
                      CopyAtomicModel(m_atomicmodels, *m_model));
    }
}

void Class::write(std::ostream& out) const
{
    out << "<class name=\"" << m_name.c_str() << "\" >\n";
    m_atomicmodels.writeModel(m_model, out);
    out << "</class>\n";
}

}} // namespace vle vpz
