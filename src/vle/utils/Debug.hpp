/**
 * @file vle/utils/Debug.hpp
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


#ifndef UTILS_DEBUG_HPP
#define UTILS_DEBUG_HPP

#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>
#include <boost/format.hpp>



namespace vle { namespace utils {

/**
 * A assert macro to throw exception when assertion failed. This macro
 * show compilation information like current file, line, function when
 * NDEBUG is not defile. If it define, only message is show.
 *
 * To use:
 * @code
 * Assert(file_exception, f->isopen(), "error opening file");
 * @endcode
 *
 * @param type exception class to throw
 * @param test represent test to perform
 * @param debugstring a message send to exception
 * @throw an exception define by type
 */
#ifndef NDEBUG
#define Assert(type, test, debugstring) { \
    if (! (test)) { \
        if (vle::utils::Trace::trace().isInLevel( \
            vle::utils::Trace::IMPORTANT)) { \
            std::string _vleerr__( \
                boost::str(boost::format( \
                "%5%\nAssertion '%1%' failed in file %2%: line " \
                "%3%\n'%4%'\n%6%\n") % \
                           #test % __FILE__ % __LINE__ % __PRETTY_FUNCTION__ % \
                           (debugstring) % vle::utils::print_trace_report())); \
                throw type(_vleerr__); \
        } else { \
            std::string _vleerr__( \
                boost::str(boost::format("%1%") % (debugstring))); \
            throw type(_vleerr__); \
        } \
    } \
}
#else // NDEBUG
#define Assert(type, test, debugstring) { \
    if (! (test)) { \
        std::string _vleerr__( \
            boost::str(boost::format("%1%") % (debugstring))); \
        throw type(_vleerr__); \
    } \
}
#endif // NDEBUG

#ifndef NDEBUG
#define AssertS(type, test) { \
    if (! (test)) { \
        if (vle::utils::Trace::trace().isInLevel( \
            vle::utils::Trace::IMPORTANT)) { \
            std::string _vleerr__( \
                boost::str(boost::format( \
                "Assertion '%1%' failed in file %2%: line %3%\n'%4%'\n%5%\n") % \
                           #test % __FILE__ % __LINE__ % __PRETTY_FUNCTION__ % \
                           vle::utils::print_trace_report())); \
                throw type(_vleerr__); \
        } else { \
	    throw type(""); \
        } \
    } \
}
#else // NDEBUG
#define AssertS(type, test) { \
    if (! (test)) { \
        throw type(""); \
    } \
}
#endif // NDEBUG

/**
 * A function to throw an specified Exception. This marco show compilation
 * information like current file, line, function when NDEBUG is not defined.
 *
 * To use:
 * @code
 * if (not x) {
 *     delete value;
 *     Throw(Exception::File, "file not found");
 * }
 * @endcode
 *
 * @throw the specified argument.
 */
#ifndef NDEBUG
#define Throw(type, debugstring) { \
    if (vle::utils::Trace::trace().isInLevel( \
            vle::utils::Trace::IMPORTANT)) { \
        std::string _vleerr__( \
            boost::str(boost::format( \
            "%4%\nThrow exception failed in file %1%: line %2%\n'%3%'\n%5%\n") % \
                       __FILE__ % __LINE__ % __PRETTY_FUNCTION__ % \
                       (debugstring) % vle::utils::print_trace_report())); \
            throw type(_vleerr__); \
    } else { \
        std::string _vleerr__( \
            boost::str(boost::format("%1%") % (debugstring))); \
	throw type(_vleerr__); \
    } \
}
#else // NDEBUG
#define Throw(type, debugstring) { \
    std::string _vleerr__( \
        boost::str(boost::format("%1%") % (debugstring))); \
    throw type(_vleerr__); \
}
#endif

}} // namespace vle utils

#endif
