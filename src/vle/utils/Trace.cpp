/**
 * @file utils/Trace.cpp
 * @author The VLE Development Team.
 * @brief A simple class to manage log files.
 */

/*
 * Copyright (c) 2005 The vle Development Team
 *
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <vle/utils/Trace.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Tools.hpp>



namespace vle { namespace utils {

    Trace* Trace::m_trace = 0;

    Trace::Trace() :
        m_minlevel(utils::Trace::ALWAYS),
        m_warnings(0)
    {
	m_filename = getDefaultLogFilename();
	m_file = new std::ofstream(m_filename.c_str());

	if (not m_file->is_open()) {
            delete m_file;
            m_file = 0;
	} else {
            (*m_file) << "Start log at " << utils::get_current_date() << "\n\n";
            (*m_file) << std::flush;
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
            (*m_file) << "Start log at " << utils::get_current_date() << "\n\n";
            (*m_file) << std::flush;
	}
    }

    std::string Trace::getDefaultLogFilename()
    {
        return getLogFilename("vle.log");
    }

    std::string Trace::getLogFilename(const std::string& filename)
    {
	return Glib::build_filename(utils::Path::path().getHomeDir(), filename);
    }

}} // namespace vle utils
