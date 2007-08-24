/**
 * @file utils/Trace.hpp
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

#ifndef UTILS_TRACE_HPP
#define UTILS_TRACE_HPP

#include <glibmm/ustring.h>
#include <fstream>
#include <vle/utils/Tools.hpp>



namespace vle { namespace utils {

    /**
     * A logging class to push information onto a stream or a file. We define
     * two macros to simplify the calls of this function. In ndebug mode, only
     * TRACE and DTRACE ie. ALWAYS message are streamed all other macros are
     * empty.
     *
     *
     * @code
     * Trace(utils::compose("Send event at time %1", time);
     * DTrace("Clear file");
     *
     * Trace2(utils::compose("Send event at time %1", time);
     * DTrace2("Clear file");
     * @endcode
     */
    class Trace
    {
    public:
	enum { ALWAYS = 0, IMPORTANT = 1, INFORMATION = 2, DEBUG = 3 };

	/**
	 * Singleton method to return Trace object.
	 *
	 * @return a Trace object instantiate in singleton method.
	 */
        static Trace& trace()
	    { if (!_trace) _trace = new Trace; return *_trace; }

	/**
	 * Delete Trace object instiate from function trace().
	 *
	 */
	static void kill()
	    { delete _trace; _trace = 0; }

	/**
	 * Get the current log filename.
	 *
	 * @return current log filename.
	 */
	inline const std::string& get_log_file() const
	    { return _filename; }

	/**
	 * Set a new filename to the current log file.
	 *
	 * @param filename the new filename.
	 */
        void set_log_file(const std::string& filename);

	/**
	 * A template method to push information into the file.
	 *
	 * @param c a object to push into the stream.
         * @param level the level of this push.
	 */
	template < class C >
        void push(const C& c, int level)
	    { if (_file and level >= 0 and level <= _minlevel)
		(*_file) << "---" << c << "\n" << std::flush; }

	/**
	 * A template method to push information into the file. Each call put
	 * the date into the file.
	 *
	 * @param c a object to push into the stream.
         * @param level the level of this push.
	 */
	template < class C >
        void push_date(const C& c, int level)
	    { if (_file and level >= 0 and level <= _minlevel)
                (*_file) << utils::get_current_date() << "---" << c << "\n"
                    << std::flush; }

	/**
	 * Return the default log file position. $HOME/.vle/vle.log under Unix
	 * system, $HOME/vle/vle.log under windows.
	 *
	 * @return the default log filename.
	 */
        static std::string get_default_log_filename();

	/**
	 * Return a specific log file position. $HOME/.vle/[filename].log under
	 * Unix system, $HOME/vle/[filename].log under windows.
	 *
	 * @param filename the filename witout extension.
	 *
	 * @return the default log filename.
	 */
        static std::string get_log_filename(const std::string& filename);

	/**
	 * Return the current level ([0 minlevel]).
	 *
	 * @return current level.
	 */
	inline int get_level() const
	    { return _minlevel; }

	/**
	 * Set the current level.
	 *
	 * @param level the new level to set, if level < 0 or > minlevel then
	 * minlevel is affected.
	 */
	inline void set_level(int level)
	    { _minlevel = (level < 0 or level > utils::Trace::DEBUG) ?
		  utils::Trace::DEBUG : level; }

    private:
        Trace();

        ~Trace();

        std::ofstream* _file;
	std::string    _filename;
	int            _minlevel;
        static Trace*  _trace;
    };

}} // namespace vle utils

#define TRACE(x) vle::utils::Trace::trace().push((x), vle::utils::Trace::ALWAYS)
#define DTRACE(x) vle::utils::Trace::trace().push_date((x), vle::utils::Trace::ALWAYS)

#ifndef NDEBUG
#define TRACE1(x) vle::utils::Trace::trace().push((x), vle::utils::Trace::IMPORTANT)
#define DTRACE1(x) vle::utils::Trace::trace().push_date((x), vle::utils::Trace::IMPORTANT)
#define TRACE2(x) vle::utils::Trace::trace().push((x), vle::utils::Trace::INFORMATION)
#define DTRACE2(x) vle::utils::Trace::trace().push_date((x), vle::utils::Trace::INFORMATION)
#define TRACE3(x) vle::utils::Trace::trace().push((x), vle::utils::Trace::DEBUG)
#define DTRACE3(x) vle::utils::Trace::trace().push_date((x), vle::utils::Trace::DEBUG)
#else
#define TRACE1(x)
#define DTRACE1(x)
#define TRACE2(x)
#define DTRACE2(x)
#define TRACE3(x)
#define DTRACE3(x)
#endif

#endif
