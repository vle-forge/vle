/**
 * @file vle/utils/Trace.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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


#include <vle/utils/Trace.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/DateTime.hpp>
#include <vle/utils/Debug.hpp>


namespace vle { namespace utils {

Trace* Trace::m_trace = 0;

Trace::Trace()
    : m_minlevel(utils::Trace::ALWAYS), m_warnings(0)
{
    m_filename = getDefaultLogFilename();
    m_file = new std::ofstream(m_filename.c_str());

    if (not m_file->is_open()) {
        delete m_file;
        m_file = 0;
    } else {
        (*m_file) << _("Start log at ") << utils::DateTime::currentDate()
            << "\n\n" << std::flush;
    }
}

Trace::~Trace()
{
    delete m_file;
}

void Trace::setLogFile(const std::string& filename)
{
    std::ofstream* tmp = new std::ofstream(filename.c_str());

    if (not tmp->is_open()) {
        delete tmp;
    } else {
        if (m_file) {
            m_file->close();
            delete m_file;
        }
        m_filename.assign(filename);
        m_file = tmp;
        (*m_file) << _("Start log at ") << utils::DateTime::currentDate()
            << "\n\n" << std::flush;
    }
}

std::string Trace::getDefaultLogFilename()
{
    return getLogFilename("vle.log");
}

std::string Trace::getLogFilename(const std::string& filename)
{
    return Path::buildFilename(utils::Path::path().getHomeDir(), filename);
}

std::ostream& Trace::output()
{
    if (not m_file) {
        throw utils::InternalError(_("Trace stream is empty"));
    }

    if (not m_file->is_open()) {
        throw utils::InternalError(_("Trace stream is not opened"));
    }

    return *m_file;
}

}} // namespace vle utils
