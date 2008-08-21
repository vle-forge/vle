/**
 * @file vle/vpz/Project.cpp
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


#include <vle/vpz/Project.hpp>
#include <vle/utils/Debug.hpp>
#include <config.h>

namespace vle { namespace vpz {

Project::Project() :
    m_version(VLE_VERSION),
    m_instance(-1),
    m_replica(-1)
{
}

void Project::write(std::ostream& out) const
{
    out << "<vle_project"
        << " version=\"" << m_version << "\""
        << " date=\"" << m_date << "\""
        << " author=\"" << m_author << "\"";

    if (m_instance >= 0) {
        out << " instance=\"" << m_instance << "\"";
    }

    if (m_replica >= 0) {
        out << " replica=\"" << m_replica << "\"";
    }

    out << ">\n"
        << m_model
        << m_dynamics
        << m_classes
        << m_experiment
        << "</vle_project>\n";
}

void Project::clear()
{
    m_date.clear();
    m_model.clear();
    m_dynamics.clear();
    m_experiment.clear();
    m_classes.clear();
}

void Project::setAuthor(const std::string& name)
{
    Assert(utils::ArgError, not name.empty(),
            "Project author unknow");
;
    m_author.assign(name);
}

void Project::setDate(const std::string& date)
{
    if (date.empty()) {
        m_date.assign(utils::get_current_date());
    } else {
        m_date.assign(date);
    }
}

}} // namespace vle vpz
