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

#include <fstream>



namespace vle { namespace utils {

    /**
     * A logging class to push information onto a stream or a file. We define
     * two macros to simplify the calls of this function. In ndebug mode, only
     * TRACE and DTRACE ie. ALWAYS message are streamed all other macros are
     * empty.
     *
     * @code
     * Trace(utils::compose("Send event at time %1", time));
     * DTrace("Clear file");
     *
     * Trace2(utils::compose("Send event at time %1", time));
     * DTrace2("Clear file");
     * @endcode
     */
    class Trace
    {
    public:
	enum Level { ALWAYS, IMPORTANT, INFORMATION, DEBUG };

	/**
	 * @brief Singleton method to return Trace object.
	 * @return a Trace object instantiate in singleton method.
	 */
        static Trace& trace()
        { if (not m_trace) m_trace = new Trace; return *m_trace; }

	/**
	 * @brief Delete Trace object instiate from function trace().
	 */
	static void kill()
        { delete m_trace; m_trace = 0; }

	/**
	 * @brief Get the current log filename.
	 * @return current log filename.
	 */
	inline const std::string& getLogFile() const
        { return m_filename; }

	/**
	 * @brief Set a new filename to the current log file.
	 * @param filename the new filename.
	 */
        void setLogFile(const std::string& filename);

	/**
	 * @brief A template method to push information into the file.
	 * @param c a object to push into the stream.
         * @param level the level of this push.
	 */
	template < class C >
        void push(const C& c, Level level)
        {
            if (m_file and level >= 0 and level <= m_minlevel) {
                (*m_file) << "---" << c << "\n" << std::flush;
                if (level != ALWAYS) m_warnings++;
            }
        }

	/**
         * @brief Return the default log file position. $HOME/.vle/vle.log under
         * Unix system, $HOME/vle/vle.log under windows.
	 * @return the default log filename.
	 */
        static std::string getDefaultLogFilename();

	/**
         * @brief Return a specific log file position. $HOME/.vle/[filename].log
         * under Unix system, $HOME/vle/[filename].log under windows.
	 * @param filename the filename witout extension.
	 * @return the default log filename.
	 */
        static std::string getLogFilename(const std::string& filename);

	/**
	 * @brief Return the current level ([0 minlevel]).
	 * @return current level.
	 */
	inline Trace::Level getLevel() const
        { return m_minlevel; }

	/**
	 * @brief Set the current level.
	 * @param level the new level to set, if level < 0 or > minlevel then
	 * minlevel is affected.
	 */
	inline void setLevel(Trace::Level level)
        {
            m_minlevel = (level < 0 or level > utils::Trace::DEBUG) ?
                utils::Trace::DEBUG : level;
        }

        /** 
         * @brief Return true if the specified level is between [ALWAYS, level].
         * @param level the specified level to test.
         * @return true if the specified level is between ALWAYS and level,
         * otherwise, false.
         */
        inline bool isInLevel(Level level)
        { return ALWAYS <= level and level <= m_minlevel; }

        /** 
         * @brief Return true if warning are flushed in the stream. Warnigs are
         * defined like not ALWAYS.
         * @return true if one or more warning are flushed.
         */
        inline bool haveWarning() const
        { return m_warnings > 0; }

        /** 
         * @brief Return the number of warnings.
         * @return the number of warnings.
         */
        inline unsigned int warnings() const
        { return m_warnings; }

    private:
        Trace();

        ~Trace();

        std::ofstream*  m_file;
	std::string     m_filename;
	Level           m_minlevel;
        static Trace*   m_trace;
        unsigned int    m_warnings;
    };

}} // namespace vle utils

#define TraceAlways(x) { \
    vle::utils::Trace::trace().push((x), vle::utils::Trace::ALWAYS); }
#define TraceImportant(x) { \
    vle::utils::Trace::trace().push((x), vle::utils::Trace::IMPORTANT); }
#define TraceInformation(x) { \
    vle::utils::Trace::trace().push((x), vle::utils::Trace::INFORMATION); }
#define TraceDebug(x) { \
    vle::utils::Trace::trace().push((x), vle::utils::Trace::DEBUG); }

#ifndef NDEBUG
#define DTraceAlways(x) { \
    vle::utils::Trace::trace().push((x), vle::utils::Trace::ALWAYS); }
#define DTraceImportant(x) { \
    vle::utils::Trace::trace().push((x), vle::utils::Trace::IMPORTANT); }
#define DTraceInformation(x) { \
    vle::utils::Trace::trace().push((x), vle::utils::Trace::INFORMATION); }
#define DTraceDebug(x) { \
    vle::utils::Trace::trace().push((x), vle::utils::Trace::DEBUG); }
#else
#define DTraceAlways(x) {}
#define DTraceImportant(x) {}
#define DTraceInformation(x) {}
#define DTraceDebug(x) {}
#endif

#endif
