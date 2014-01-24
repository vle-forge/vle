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


#include <vle/vle.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Trace.hpp>
#include <glibmm/thread.h>
#include <libxml/parser.h>

namespace vle {

static void __vle_init()
{
    xmlInitParser(); /**< Initialize the libxml2 library. */

    if (not Glib::thread_supported()) {
        Glib::thread_init();
    }

    utils::Path::init();
    utils::Trace::init();
}

Init::Init()
{
    __vle_init();

    setlocale(LC_ALL, "");

#ifdef VLE_HAVE_NLS
    bindtextdomain(VLE_LOCALE_NAME, utils::Path::path().getLocaleDir().c_str());
    textdomain(VLE_LOCALE_NAME);
#endif
}

Init::Init(const char *localname)
{
    __vle_init();

    if (!setlocale(LC_ALL, localname == NULL ? "" : localname))
        setlocale(LC_ALL, "C");

#ifdef VLE_HAVE_NLS
    bindtextdomain(VLE_LOCALE_NAME, utils::Path::path().getLocaleDir().c_str());
    textdomain(VLE_LOCALE_NAME);
#endif
}

Init::~Init()
{
    utils::Path::kill();
    utils::Trace::kill();

    xmlCleanupParser();
}

} // namespace vle
