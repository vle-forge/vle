/**
 * @file utils/Debug.hpp
 * @author The VLE Development Team.
 * @brief Macro and filestream to produce debug.
 */

/*
 * Copyright (c) 2004, 2005 The vle Development Team
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

#ifndef UTILS_DEBUG_HPP
#define UTILS_DEBUG_HPP

#include <iostream>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
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
        std::string _vleerr__( \
            boost::str(boost::format( \
                "%5%\nAssertion '%1%' failed in file %2%: line " \
                                      "%3%\n'%4%'\n%6%\n") % \
                #test % __FILE__ % __LINE__ % __PRETTY_FUNCTION__ % \
		(debugstring) % vle::utils::print_trace_report())); \
        throw type(_vleerr__); }}
#else // NDEBUG
#define Assert(type, test, debugstring) { \
    if (! (test)) { \
        throw type(boost::str(boost::format( \
                "%1%\n") % (debugstring))); }}
#endif // NDEBUG

/**
 * A assert macro to throw exception when assertion failed. This macro
 * show compilation information like current file, line, function when
 * NDEBUG is not defined.
 *
 * To use:
 * @code
 * AssertS(file_exception, f->isopen());
 * @endcode
 *
 * @param type exception class to throw
 * @param test represent test to perform
 * @throw an exception define by type
 */
#ifndef NDEBUG
#define AssertS(type, test) { \
    if (! (test)) { \
        std::string _vleerr__( \
            boost::str(boost::format( \
                "Assertion '%1%' failed in file %2%: line %3%\n'%4%'\n%5%\n") % \
                             #test % __FILE__ % __LINE__ % __PRETTY_FUNCTION__ % \
                             vle::utils::print_trace_report())); \
        throw type(_vleerr__); }}
#else // NDEBUG
#define AssertS(type, test) { \
    if (! (test)) { \
        throw type(""); }}
#endif // NDEBUG

/**
 * A assert macro to throw Internal exception when assertion failed. This
 * macro show compilation information like current file, line, function when
 * NDEBUG is not defined.
 *
 * To use:
 * @code
 * AssertI(ptr_tab != NULL);
 * @endcode
 *
 * @param test represent test to perform
 * @throw an Internal exception
 */
#ifndef NDEBUG
#define AssertI(test) { \
    if (! (test)) { \
        std::string _vleerr__( \
            boost::str(boost::format( \
                "Assertion '%1%' failed in file %2%: line %3%\n'%4%'\n%5%\n") % \
                       #test % __FILE__ % __LINE__ % __PRETTY_FUNCTION__ % \
                       vle::utils::print_trace_report())); \
        throw vle::utils::InternalError(_vleerr__); }}
#else // NDEBUG
#define AssertI(test) { \
    if (! (test)) { \
        throw vle::utils::InternalError(""); }}
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
    std::string _vleerr__( \
        boost::str(boost::format( \
        "%4%\nThrow exception failed in file %1%: line %2%\n'%3%'\n%5%\n") % \
                   __FILE__ % __LINE__ % __PRETTY_FUNCTION__ % \
		(debugstring) % vle::utils::print_trace_report())); \
        throw type(_vleerr__); }
#else // NDEBUG
#define Throw(type, debugstring) { \
    throw type(boost::str(boost::format( \
        "%1%") % (debugstring))); }
#endif

/**
 * A function to throw an Internal Exception. This marco show compilation
 * information like current file, line, function when NDEBUG is not defined.
 *
 * To use:
 * @code
 * ThrowInternal();
 * @endcode
 *
 * @throw Exception::Internal
 */
#ifndef NDEBUG
#define ThrowInternal() { \
    std::string _vleerr__( \
        boost::str(boost::format( \
                "Throw Internal error in file %1% : line %2%\n'%3%'\n%4%\n") % \
            __FILE__ % __LINE__ % __PRETTY_FUNCTION__ % \
                         vle::utils::print_trace_report())); \
    throw vle::utils::InternalError(_vleerr__); }
#else // NDEBUG
#define ThrowInternal() { \
    throw vle::utils::InternalError(""); }
#endif // NDEBUG

}} // namespace vle utils

#endif
