/**
 * @file src/vle/vpz/Class.cpp
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
    out << "<class name=\"" << m_name << "\" >\n";
    m_model->writeXML(out);
    out << "</class>\n";
}

}} // namespace vle vpz
