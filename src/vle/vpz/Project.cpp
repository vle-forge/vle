/** 
 * @file vpz/Project.cpp
 * @brief 
 * @author The vle Development Team
 * @date lun, 27 fév 2006 18:57:37 +0100
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

#include <vle/vpz/Project.hpp>
#include <vle/utils/Debug.hpp>
#include <glibmm/date.h>

namespace vle { namespace vpz {

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
        << m_experiment
        << m_classes
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
    AssertI(not name.empty());
    m_author.assign(name);
}

void Project::setDate(const std::string& date)
{
    Glib::Date d;
    d.set_parse(date);
    if (d.valid()) {
        m_date = d.format_string("%a, %d %b %Y %H:%M:%S %z");
    } else {
        m_date.assign(date);
    }
}

void Project::setCurrentDate()
{
    Glib::Date d;
    m_date = d.format_string("%a, %d %b %Y %H:%M:%S %z");
}

}} // namespace vle vpz
