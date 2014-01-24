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


#ifndef VLE_UTILS_TRACE_HPP
#define VLE_UTILS_TRACE_HPP 1

#include <vle/DllDefines.hpp>
#include <boost/format/format_fwd.hpp>
#include <string>

namespace vle { namespace utils {

enum TraceLevelOptions
{
    TRACE_LEVEL_ALWAYS,         /**< Corresponds to vle --verbose=0-3
                                   or vle without any argument. */
    TRACE_LEVEL_MODEL,          /**< Corresponds to vle
                                 * --verbose=1-3. */
    TRACE_LEVEL_EXTENSION,      /**< Corresponds to vle
                                 * --verbose=2-3. */
    TRACE_LEVEL_DEVS            /**< Corresponds to vle
                                 * --verbose=3. */
};

enum TraceStreamType
{
    TRACE_STREAM_STANDARD_ERROR, /**< Uses the std::cerr to
                                       * log. Default value */
    TRACE_STREAM_STANDARD_OUTPUT, /**< Uses the std::cout to log. */
    TRACE_STREAM_FILE      /**< Use a specific file to log */
};

/**
 * A logging class to send information into a file. We define two types of
 * macros to simplify the calls of this function. In NDEBUG mode (cflags
 * parameter), only TraceAlways, TraceModel, TraceExtension and TraceDevs work
 * otherwise. The macros DTraceAlways, DTraceModel, DTraceExtension and
 * DTraceDevs work only withtout NDEBUG.
 *
 * @code
 * #include <vle/utils/Trace.hpp>
 *
 * int main(int argc, char* argv)
 * {
 *     utils::Trace::init();
 *
 *     TraceAlways(utils::fmt("Send event at time %1%", time));
 *     DTraceAlways("Clear file");
 *
 *     utils::Trace::setLogFile("check.log");
 *     TraceAlway(utils::fmt("Yeah!"));
 *
 *     utils::Trace::kill();
 * }
 * @endcode
 */
class VLE_API Trace
{
public:
    /**
     * Initialize the Trace singleton.
     */
    static void init();

    /**
     * Delete Trace object instantiate from function Trace::init().
     */
    static void kill();

    /**
     * Get the current log filename.
     *
     * @return current log filename or empty if not open.
     */
    static std::string getLogFile();

    /**
     * Set a new filename to the current log file.
     *
     * @param filename the new filename.
     */
    static void setLogFile(const std::string& filename);

    /**
     * Set the std::cout stream to the current log.
     *
     */
    static void setStandardOutput();

    /**
     * Set the std::cerr stream to the current log.
     *
     */
    static void setStandardError();

    /**
     * Send a message to the log file.
     *
     * @param str The string to send.
     * @param level The Level of the message.
     */
    static void send(const std::string& str,
                     TraceLevelOptions level = TRACE_LEVEL_ALWAYS);

    /**
     * Send a message to the log file using a boost::format object.
     *
     * @param str The boost::format message to send.
     * @param level The Level of the message.
     */
    static void send(const boost::format& str,
                     TraceLevelOptions level = TRACE_LEVEL_ALWAYS);

    /**
     * Return the default log file position. $VLE_HOME/vle.log under Unix
     * system, %VLE_HOME%/vle.log under Win32.
     *
     * @return the default log filename.
     */
    static std::string getDefaultLogFilename();

    /**
     * Return a specific log file position. $VLE_HOME/.vle/[filename].log
     * under Unix system, $VLE_HOME/[filename].log under windows.
     *
     * @param filename the filename without extension.
     *
     * @return the default log filename.
     */
    static std::string getLogFilename(const std::string& filename);

    /**
     * Return the current level.
     *
     * @return current level.
     */
    static TraceLevelOptions getLevel();

    /**
     * Return the current log type (standard output, error output or file).
     *
     *
     * @return the type.
     */
    static TraceStreamType getType();

    /**
     * Set the current level.
     *
     * @param level the new level to set.
     */
    static void setLevel(TraceLevelOptions level);

    /**
     * Return true if the specified level is between [ALWAYS, current
     * level].
     *
     * @param level the specified level to test.
     *
     * @return true if the specified level is between ALWAYS and level,
     * otherwise, false.
     */
    static bool isInLevel(TraceLevelOptions level);

    /**
     * Return true if warnings are send in the stream.
     *
     * @return true if one or more warning are sent.
     */
    static bool haveWarning();

    /**
     * Return the number of warnings.
     *
     * @return the number of warnings.
     */
    static size_t warnings();

    /**
     * Cast the integer into a TraceLevelOptions option.
     *
     * Try to cast the integer into a TraceLevelOptions. If integer is equal to
     * 0, the function returns TRACE_LEVEL_ALWAYS, if integer is equal to 1,
     * the function returns TRACE_LEVEL_MODEL, if the integer is equal to 2,
     * the function return TRACE_LEVEL_EXTENSION, finally, if the integer is
     * equal to 3, the function returns TRACE_LEVEL_DEVS. All other value, the
     * function return TRACE_LEVEL_ALWAYS.
     *
     * @code
     * utils::Trace::cast(0); // return TRACE_LEVEL_ALWAYS.
     * utils::Trace::cast(1); // return TRACE_LEVEL_MODEL.
     * utils::Trace::cast(2); // return TRACE_LEVEL_EXTENSION.
     * utils::Trace::cast(3); // return TRACE_LEVEL_DEVS.
     *
     * utils::Trace::cast(-1); // return TRACE_LEVEL_ALWAYS.
     * utils::Trace::cast(100); // return TRACE_LEVEL_ALWAYS.
     * @endcode
     *
     * @param level The integer to convert.
     *
     * @return A TraceLevelOptions.
     */
    static TraceLevelOptions cast(int level) throw();

private:
    Trace();
    ~Trace();
    Trace(const Trace& other);
    Trace& operator=(const Trace& other);

    class Pimpl;
    Pimpl* mImpl;
};

}} // namespace vle utils

#define TraceAlways(x) { \
    if (vle::utils::Trace::isInLevel(vle::utils::TRACE_LEVEL_ALWAYS)) { \
        vle::utils::Trace::send(x, vle::utils::TRACE_LEVEL_ALWAYS); \
    } \
}
#define TraceModel(x) { \
    if (vle::utils::Trace::isInLevel(vle::utils::TRACE_LEVEL_MODEL)) { \
        vle::utils::Trace::send(x, vle::utils::TRACE_LEVEL_MODEL); \
    } \
}
#define TraceExtension(x) { \
    if (vle::utils::Trace::isInLevel(vle::utils::TRACE_LEVEL_EXTENSION)) { \
        vle::utils::Trace::send(x, vle::utils::TRACE_LEVEL_EXTENSION); \
    } \
}
#define TraceDevs(x) { \
    if (vle::utils::Trace::isInLevel(vle::utils::TRACE_LEVEL_DEVS)) { \
        vle::utils::Trace::send(x, vle::utils::TRACE_LEVEL_DEVS); \
    } \
}

#ifndef NDEBUG
#define DTraceAlways(x) { \
    if (vle::utils::Trace::isInLevel(vle::utils::TRACE_LEVEL_ALWAYS)) { \
        vle::utils::Trace::send(x, vle::utils::TRACE_LEVEL_ALWAYS); \
    } \
}
#define DTraceModel(x) { \
    if (vle::utils::Trace::isInLevel(vle::utils::TRACE_LEVEL_MODEL)) { \
        vle::utils::Trace::send(x, vle::utils::TRACE_LEVEL_MODEL); \
    } \
}
#define DTraceExtension(x) { \
    if (vle::utils::Trace::isInLevel(vle::utils::TRACE_LEVEL_EXTENSION)) { \
        vle::utils::Trace::send(x, vle::utils::TRACE_LEVEL_EXTENSION); \
    } \
}
#define DTraceDevs(x) { \
    if (vle::utils::Trace::isInLevel(vle::utils::TRACE_LEVEL_DEVS)) { \
        vle::utils::Trace::send(x, vle::utils::TRACE_LEVEL_DEVS); \
    } \
}
#else
#define DTraceAlways(x) {}
#define DTraceModel(x) {}
#define DTraceExtension(x) {}
#define DTraceDevs(x) {}
#endif

#endif
