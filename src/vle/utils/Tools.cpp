/*
 * @file vle/utils/Tools.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#include <vle/utils/Tools.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Socket.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/version.hpp>
#include <boost/lexical_cast.hpp>
#include <iomanip>

#include <libxml/parser.h> /** to the init and finalize functions which need to
                             initialize and cleanup the libxml2 library. */
#include <glibmm/thread.h>
#include <glib/gstdio.h>

#ifdef G_OS_WIN32
#include <io.h>
#endif

#ifdef VLE_HAVE_EXECINFO_H
#include <execinfo.h>
#endif

#ifdef VLE_HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifdef VLE_HAVE_GCC_ABI_DEMANGLE
#include <cxxabi.h>
#endif

#ifdef G_OS_UNIX
#include <unistd.h>
#endif

namespace vle { namespace utils {

template < typename T >
    bool is(const std::string& str)
    {
        try {
            boost::lexical_cast < T >(str);
            return true;
        } catch (const boost::bad_lexical_cast& /*e*/) {
            return false;
        }
    }

template <>
    bool is < bool >(const std::string& str)
    {
        if (str == "true") {
            return true;
        } else if (str == "false") {
            return false;
        } else {
            try {
                boost::lexical_cast < bool >(str);
                return true;
            } catch (const boost::bad_lexical_cast& /*e*/) {
                return false;
            }
        }
    }

template bool is < bool >(const std::string& str);
template bool is < int8_t >(const std::string& str);
template bool is < int16_t >(const std::string& str);
template bool is < int32_t >(const std::string& str);
template bool is < uint8_t >(const std::string& str);
template bool is < uint16_t >(const std::string& str);
template bool is < uint32_t >(const std::string& str);
template bool is < double >(const std::string& str);
template bool is < float >(const std::string& str);

template < typename T >
    std::string to(const T t)
    {
        std::ostringstream o;
        o << t;
        return o.str();
    }

template <>
    std::string to < bool >(const bool t)
    {
        if (t) {
            return "true";
        } else {
            return "false";
        }
    }

template std::string to < bool >(const bool t);
template std::string to < int8_t >(const int8_t t);
template std::string to < int16_t >(const int16_t t);
template std::string to < int32_t >(const int32_t t);
template std::string to < uint8_t >(const uint8_t t);
template std::string to < uint16_t >(const uint16_t t);
template std::string to < uint32_t >(const uint32_t t);
template std::string to < double >(const double t);
template std::string to < float >(const float t);

template < typename T >
    T to(const std::string& str)
    {
        try {
            return boost::lexical_cast < T >(str);
        } catch (const boost::bad_lexical_cast& /*e*/) {
            throw utils::ArgError(fmt(_("Can not convert `%1%'")) % str);
        }
    }

template bool to < bool >(const std::string& str);
template int8_t to < int8_t >(const std::string& str);
template int16_t to < int16_t >(const std::string& str);
template int32_t to < int32_t >(const std::string& str);
template uint8_t to < uint8_t >(const std::string& str);
template uint16_t to < uint16_t >(const std::string& str);
template uint32_t to < uint32_t >(const std::string& str);
template double to < double >(const std::string& str);
template float to < float >(const std::string& str);

template < typename output > output convert(
    const std::string& value,
    bool locale,
    const std::string& loc)
{
    std::stringstream s;

    if (locale or not loc.empty()) {
        if (not isLocaleAvailable(loc)) {
            s.imbue(std::locale::classic());
            locale = false;
        } else {
            s.imbue(std::locale(loc.c_str()));
            locale = true;
        }
    }

    s << value;

    if (s.bad() or s.fail()) {
        throw ArgError("failed to read the value");
    }

    output result;
    s >> result;

    if (s.bad()) {
        throw ArgError("failed to write the value");
    } else {
        std::streambuf* buf = s.rdbuf();

        if (buf->in_avail() > 0 and locale) {
            s.imbue(std::locale::classic());
            s.str(value);
            s >> result;

            if (s.bad()) {
                throw ArgError("failed to write the value");
            } else {
                return result;
            }
        } else {
            return result;
        }
    }
}

bool isLocaleAvailable(const std::string& locale)
{
    try {
        std::locale tmp(locale.c_str());
        return true;
    } catch (const std::runtime_error& /*e*/) {
        return false;
    }
}

template bool convert < bool >(const std::string& value, bool locale,
                               const std::string& loc);
template int8_t convert < int8_t >(const std::string& value, bool locale,
                                   const std::string& loc);
template int16_t convert < int16_t >(const std::string& value, bool locale,
                                     const std::string& loc);
template int32_t convert < int32_t >(const std::string& value, bool locale,
                                     const std::string& loc);
template uint8_t convert < uint8_t >(const std::string& value, bool locale,
                                     const std::string& loc);
template uint16_t convert < uint16_t >(const std::string& value, bool locale,
                                       const std::string& loc);
template uint32_t convert < uint32_t >(const std::string& value, bool locale,
                                       const std::string& loc);
template double convert < double >(const std::string& value, bool locale,
                                   const std::string& loc);
template float convert < float >(const std::string& value, bool locale,
                                 const std::string& loc);

std::string toScientificString(const double& v, bool locale)
{
    std::ostringstream o;
    if (locale) {
        std::locale selected("");
        o.imbue(selected);
    }

    o << std::setprecision(std::numeric_limits<double>::digits10) << v;

    return o.str();
}

std::string demangle(const std::type_info& in)
{
    std::string result;
#ifdef VLE_HAVE_GCC_ABI_DEMANGLE
    result.assign(utils::demangle(in.name()));
#else
    result.assign(in.name());
#endif
    return result;
}

std::string demangle(const std::string& in)
{
    std::string result;
#ifdef VLE_HAVE_GCC_ABI_DEMANGLE
    char* output = NULL;
    int status;
    output = abi::__cxa_demangle(in.c_str(), NULL, NULL, &status);
    if (status == 0 and output) {
        result.assign(output);
    }

    free(output);
#else
    result.assign(in);
#endif
    return result;
}

void init()
{
    xmlDefaultSAXHandlerInit();

    if (not Glib::thread_supported()) {
        Glib::thread_init();
    }

    utils::Path::init();
    utils::Trace::init();
    utils::net::Base::init();

#ifdef VLE_HAVE_NLS
    setlocale(LC_ALL, "");
    bindtextdomain(VLE_LOCALE_NAME, utils::Path::path().getLocaleDir().c_str());
    textdomain(VLE_LOCALE_NAME);
#endif
}

void finalize()
{
    utils::Path::kill();
    utils::Trace::kill();
    xmlCleanupParser();
}

}} // namespace vle utils
