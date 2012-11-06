/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2012 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
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


#include <vle/vle.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Trace.hpp>
#include <glibmm/thread.h>
#include <curl/curl.h>
#include <libxml/parser.h>

namespace vle {

Init::Init()
{
    CURLcode error;

    xmlInitParser(); /**< Initialize the libxml2 library. */

#ifdef _WIN32
    error = curl_global_init(CURL_GLOBAL_WIN32); /**< Initialize the
                                                  * libcurl library
                                                  * for WIN32. */
#else
    error = curl_global_init(CURL_GLOBAL_NOTHING); /**< Initialize the
                                                    * libcurl library
                                                    * for other OS. */
#endif

    if (error != CURLE_OK) {
        const char *msg = curl_easy_strerror(error);

        throw utils::InternalError(fmt(_("Fail to initialize curl: %1%")) % msg);
    }

    if (not Glib::thread_supported()) {
        Glib::thread_init();
    }

    utils::Path::init();
    utils::Trace::init();

#ifdef VLE_HAVE_NLS
    setlocale(LC_ALL, "");
    bindtextdomain(VLE_LOCALE_NAME, utils::Path::path().getLocaleDir().c_str());
    textdomain(VLE_LOCALE_NAME);
#endif
}

Init::~Init()
{
    utils::Path::kill();
    utils::Trace::kill();

    curl_global_cleanup();

    xmlCleanupParser();
}

} // namespace vle
