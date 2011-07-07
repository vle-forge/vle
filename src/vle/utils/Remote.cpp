/*
 * @file vle/utils/Remote.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
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


#include <vle/utils/Path.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Remote.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Preferences.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <glibmm/spawn.h>
#include <glibmm/thread.h>
#include <string>
#include <list>
#include <cstdlib>
#include <libxml/nanohttp.h>
#include <libxml/tree.h>

namespace vle { namespace utils {

Remote::Remote()
{
    refresh();
}

const RemoteList& Remote::list() const
{
    return mRemotes;
}

PackageList Remote::show(const std::string& remote)
{
    std::string url = getUrl(remote);
    url += "/packages";

    std::string response = httpGet(remote, url);

    PackageList result;

    boost::algorithm::split(result, response,
                            boost::algorithm::is_any_of(" \n\r"));

    return result;
}

std::string Remote::show(const std::string& remote,
                         const std::string& package)
{
    std::string url = getUrl(remote);
    url += "/";
    url += package;
    url += "/Description.txt";

    return httpGet(remote, url);
}

std::string Remote::get(const std::string& remote, const std::string& package)
{
    std::string url = getUrl(remote);
    url += "/";
    url += package;

    std::string response = httpGet(remote, url);

    return utils::Path::writeToTemp(package, response);
}

const std::string& Remote::getUrl(const std::string& remote) const
{
    throw utils::NotYetImplemented(fmt(
            _("Remote::getUrl not yet implemented")));
//    if (mRemotes.empty()) {
//        Preferences prefs;
//        prefs.load();
//        mRemotes = prefs.getKeyValues("remotes");
//    }

//    RemoteList::const_iterator it = mRemotes.find(remote);
//    if (it == mRemotes.end()) {
//        throw utils::InternalError(fmt(_(
//                    "Remote: unknown remote `%1%'")) % remote);
//    }

//    return it->second;
}

void Remote::refresh()
{
    throw utils::NotYetImplemented(fmt(
            _("Remote::refresh not yet implemented")));
//    Preferences prefs;
//    prefs.load();

//    try {
//        mRemotes = prefs.getKeyValues("remotes");
//    } catch (const std::exception& /*e*/) {
//    }
}

void Remote::config(const std::string& remote, const std::string& url)
{
    throw utils::NotYetImplemented(fmt(
            _("Remote::config not yet implemented")));
//    Preferences prefs;
//    prefs.load();
//    prefs.setAttributes("remotes", remote, url);
//    refresh();
}

std::string Remote::httpGet(const std::string& remote, const std::string& url)
{
    xmlNanoHTTPInit();

    void* ctxt = xmlNanoHTTPMethod(url.c_str(), "GET", NULL, NULL, NULL, 0);

    if (not ctxt) {
        throw utils::InternalError(fmt(
                _("Remote: no response from server `%1%' (%2%)")) % remote %
            url);
    }

    int code = xmlNanoHTTPReturnCode(ctxt);
    if (code != 200) {
        throw utils::InternalError(fmt(_(
                    "Remote: server `%1%' return bad access `%2%' "
                    "(may be an error in URL ?):\n%3%")) % remote % code % url);
    }

    xmlBuffer* output = xmlBufferCreate();
    xmlChar buf[1024];
    int len;

    while ((len = xmlNanoHTTPRead(ctxt, buf, sizeof(buf))) > 0) {
        if(xmlBufferAdd(output, buf, len) != 0) {
            xmlNanoHTTPClose(ctxt);
            xmlBufferFree(output);
            throw utils::InternalError(fmt(_("Adding error")));
        }
    }

    xmlNanoHTTPClose(ctxt);
    int resLength = xmlBufferLength(output);

    std::string response(reinterpret_cast < const char* >
                         (xmlBufferContent(output)),
                         resLength);
    xmlBufferFree(output);
    xmlNanoHTTPCleanup();

    return response;
}

}} // namespace vle utils
