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


#include <vle/utils/Context.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/utils/Filesystem.hpp>
#include <vle/version.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace vle { namespace utils {

ContextPtr make_context(const Path& /* prefix */, std::string locale)
{
    return std::make_shared<Context>(locale);
}

Context::Context(const Path& /* prefix */, std::string locale)
{
    initHomeDir();
    initPrefixDir();
    initVleHomeDirectory();

    if (locale == "C")
        setlocale(LC_ALL, "C");
    else
        if (not setlocale(LC_ALL, locale.c_str()))
            setlocale(LC_ALL, "C");

#if defined(VLE_HAVE_NLS)
    bindtextdomain(VLE_LOCALE_NAME, getLocaleDir().string().c_str());
    textdomain(VLE_LOCALE_NAME);
#endif  
}

Path Context::getLocaleDir() const
{
    Path p(m_prefix);
    p /= "share";
    p /= "locale";

    return p;
}

Path Context::getHomeFile(const std::string& name) const
{
    Path p(m_home);
    p /= name;

    return p;
}

Path Context::getConfigurationFile() const
{
    return getHomeFile("vle-" VLE_ABI_VERSION ".conf");
}

Path Context::getLogFile() const
{
    return getHomeFile("vle-" VLE_ABI_VERSION ".log");
}

Path Context::getBinaryPackagesDir() const
{
    Path p(m_home);
    p /= "pkgs-" VLE_ABI_VERSION;

    return p;
}

std::vector<std::string> Context::getBinaryPackages() const
{
    Path pkgs(getBinaryPackagesDir());

    if (not pkgs.is_directory())
        throw utils::InternalError(
            (fmt(_("Package error: '%1%' is not a directory")) %
             getBinaryPackagesDir().string()).str());

    std::vector <std::string> result;
    for (DirectoryIterator it(pkgs), end; it != end; ++it)
        if (it->is_directory())
            result.push_back(it->path().filename());

    return result;
}


Path Context::getLocalPackageFilename() const
{
    Path p(getBinaryPackagesDir());
    p /= "local.pkg";

    return p;
}

Path Context::getRemotePackageFilename() const
{
    Path p(getBinaryPackagesDir());
    p /= "remote.pkg";

    return p;
}

Path Context::getTemplateDir() const
{
    Path p(m_prefix);
    p /= VLE_SHARE_DIRS;
    p /= "template";

    return p;
}

Path Context::getTemplate(const std::string& name) const
{
    Path p(m_prefix);
    p /= VLE_SHARE_DIRS;
    p /= "template";
    p /= name;

    return p;
}

void Context::initVleHomeDirectory()
{
    Path pkgs = getBinaryPackagesDir();

    if (not pkgs.exists())
        if (not Path::create_directories(getBinaryPackagesDir()))
            throw FileError(
                (fmt(_("Failed to build VLE_HOME directory (%1%)")) %
                 pkgs.string()).str());
}

bool Context::readEnv(const std::string& variable, PathList& out)
{
    const char* path_str = std::getenv(variable.c_str());
    std::string path("");
    if (path_str) {
        path.assign(path_str);
    }
    if (not path.empty()) {
        std::vector<std::string> result;
        boost::algorithm::split(result, path, boost::is_any_of(":"),
                                boost::algorithm::token_compress_on);

        auto it = std::remove_if(result.begin(), result.end(),
                                 [](const std::string& dirname)
                                 {
                                     Path p(dirname);
                                     return p.is_directory();
                                 });

        result.erase(it, result.end());

        std::copy(result.begin(), result.end(), std::back_inserter(out));
        return true;
    }
    return false;
}

void Context::readHomeDir()
{
    const char* path_str = std::getenv("VLE_HOME");
    std::string path("");
    if (path_str) {
        path.assign(path_str);
    }
    if (not path.empty()) {
        if (Path::is_directory(path)) {
            m_home = path;
        } else {
            throw FileError(
                (fmt(_("Path: VLE_HOME '%1%' does not exist")) % path).str());
        }
    } else {
        m_home.clear();
    }
}

void Context::clearPluginDirs()
{
    m_simulator.clear();
    m_stream.clear();
    m_output.clear();
    m_modeling.clear();
}

std::ostream& operator<<(std::ostream& out, const PathList& paths)
{
    for (const auto& elem : paths)
        out << '\t' << elem.string() << '\n';

    return out;
}

void Context::fillBinaryPackagesList(std::vector<std::string>& pkglist) const
{
    std::string header = "Packages from: ";
    header += getBinaryPackagesDir().string();
    pkglist.clear();
    pkglist.push_back(header);

    std::vector<std::string> pkgs = getBinaryPackages();
    std::sort(pkgs.begin(), pkgs.end());
    auto itb = pkgs.cbegin(), ite = pkgs.cend();
    for (; itb!=ite; itb++) {
        pkglist.push_back(*itb);
    }
    return;
}

}} // namespace vle utils
