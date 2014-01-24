/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/vpz/Class.hpp>
#include <vle/vpz/AtomicModel.hpp>

namespace vle { namespace vpz {

Class::Class(const Class& cls) :
    Base(cls),
    m_name(cls.m_name)
{
    if (cls.m_model == 0) {
        m_model = 0;
    } else {
        m_model = cls.m_model->clone();
    }
}

Class::~Class()
{
    delete m_model;
}

void Class::write(std::ostream& out) const
{
    out << "<class name=\"" << m_name.c_str() << "\" >\n";
    m_model->write(out);
    out << "</class>\n";
}

void Class::updateDynamics(const std::string& oldname,
                           const std::string& newname)
{
    m_model->updateDynamics(oldname,newname);
}

void Class::purgeDynamics(const std::set < std::string >& dynamicslist)
{
    m_model->purgeDynamics(dynamicslist);
}

void Class::updateObservable(const std::string& oldname,
                             const std::string& newname)
{
    m_model->updateObservable(oldname, newname);
}

void Class::purgeObservable(const std::set < std::string >& observablelist)
{
    m_model->purgeObservable(observablelist);
}

void Class::updateConditions(const std::string& oldname,
                             const std::string& newname)
{
    m_model->updateConditions(oldname, newname);
}

void Class::purgeConditions(const std::set < std::string >& conditionlist)
{
    m_model->purgeConditions(conditionlist);
}

void Class::getAtomicModelList(std::vector < AtomicModel* >& list) const
{
    list.clear();

    m_model->getAtomicModelList(m_model, list);
}

}} // namespace vle vpz
