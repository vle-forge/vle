/**
 * @file Trace.cpp
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
#include <vle/utils/Debug.hpp>



namespace vle { namespace utils {

    Trace* Trace::_trace = 0;

    Trace::Trace() :
	_minlevel(utils::Trace::ALWAYS)
    {
	_filename = get_default_log_filename();
	_file = new std::ofstream(_filename.c_str());

	if (_file->is_open() == false) {
            delete _file;
            _file = 0;
	} else {
            (*_file) << "Start log at " << utils::get_current_date() << "\n\n";
            (*_file) << std::flush;
        }
    }

    Trace::~Trace()
    {
        delete _file;
    }

    void Trace::set_log_file(const std::string& filename)
    {
	std::ofstream* tmp = new std::ofstream(filename.c_str());

	if (tmp->is_open() == false) {
	    delete tmp;
	} else {
	    if (_file) {
		_file->close();
		delete _file;
	    }
	    _filename.assign(filename);
	    _file = tmp;
            (*_file) << "Start log at " << utils::get_current_date() << "\n\n";
            (*_file) << std::flush;
	}
    }

    std::string Trace::get_default_log_filename()
    {
        return get_log_filename("vle.log");
    }

    std::string Trace::get_log_filename(const std::string& filename)
    {
	return Glib::build_filename(utils::Path::path().getUserDir(), filename);
    }


}} // namespace vle utils
