/**
 * @file vle/utils/Trace.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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


#ifndef UTILS_TRACE_HPP
#define UTILS_TRACE_HPP

#include <fstream>
#include <vle/utils/DllDefines.hpp>

namespace vle { namespace utils {

    /**
     * A logging class to push information onto a stream or a file. We define
     * two macros to simplify the calls of this function. In NDEBUG mode, only
     * TRACE and DTRACE ie. ALWAYS message are streamed all other macros are
     * empty.
     *
     * @code
     * Trace(utils::fmt("Send event at time %1%", time));
     * DTrace("Clear file");
     * @endcode
     */
    class VLE_UTILS_EXPORT Trace
    {
    public:
	enum Level { ALWAYS, MODEL, EXTENSION, DEVS };

	/**
	 * @brief Singleton method to return Trace object.
	 * @return a Trace object instantiate in singleton method.
	 */
        static Trace& trace()
        { if (not m_trace) m_trace = new Trace; return *m_trace; }

        /**
         * @brief Initialize the Trace singleton.
         */
        static void init()
        { trace(); }

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
                if (m_file) {
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
            m_minlevel = (level < 0 or level > utils::Trace::DEVS) ?
                utils::Trace::DEVS : level;
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

        /**
         * @brief Get the current stream.
         * @return The current stream where push data.
         * @throw utils::InternalError if current stream is empty.
         */
        std::ostream& output();

    private:
        Trace();

        ~Trace();

        std::ofstream* m_file;
	std::string    m_filename;
	Level          m_minlevel;
        static Trace*  m_trace;
        unsigned int   m_warnings;
    };

}} // namespace vle utils

#define TraceAlways(x) { \
    if (vle::utils::Trace::trace().isInLevel(vle::utils::Trace::ALWAYS)) { \
        vle::utils::Trace::trace().push(x, vle::utils::Trace::ALWAYS); \
    } \
}
#define TraceModel(x) { \
    if (vle::utils::Trace::trace().isInLevel(vle::utils::Trace::MODEL)) { \
        vle::utils::Trace::trace().push(x, vle::utils::Trace::MODEL); \
    } \
}
#define TraceExtension(x) { \
    if (vle::utils::Trace::trace().isInLevel(vle::utils::Trace::EXTENSION)) { \
        vle::utils::Trace::trace().push(x, vle::utils::Trace::EXTENSION); \
    } \
}
#define TraceDevs(x) { \
    if (vle::utils::Trace::trace().isInLevel(vle::utils::Trace::DEVS)) { \
        vle::utils::Trace::trace().push(x, vle::utils::Trace::DEVS); \
    } \
}

#ifndef NDEBUG
#define DTraceAlways(x) { \
    if (vle::utils::Trace::trace().isInLevel(vle::utils::Trace::ALWAYS)) { \
        vle::utils::Trace::trace().push(x, vle::utils::Trace::ALWAYS); \
    } \
}
#define DTraceModel(x) { \
    if (vle::utils::Trace::trace().isInLevel(vle::utils::Trace::MODEL)) { \
        vle::utils::Trace::trace().push(x, vle::utils::Trace::MODEL); \
    } \
}
#define DTraceExtension(x) { \
    if (vle::utils::Trace::trace().isInLevel(vle::utils::Trace::EXTENSION)) { \
        vle::utils::Trace::trace().push(x, vle::utils::Trace::EXTENSION); \
    } \
}
#define DTraceDevs(x) { \
    if (vle::utils::Trace::trace().isInLevel(vle::utils::Trace::DEVS)) { \
        vle::utils::Trace::trace().push(x, vle::utils::Trace::DEVS); \
    } \
}
#else
#define DTraceAlways(x) {}
#define DTraceModel(x) {}
#define DTraceExtension(x) {}
#define DTraceDevs(x) {}
#endif

#endif
