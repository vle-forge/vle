/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <vle/utils/Context.hpp>
#include <vle/utils/ContextPrivate.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Filesystem.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/vle.hpp>

namespace vle {
namespace utils {

struct vle_log_stderr : Context::LogFunctor
{
    virtual void write(const Context& ctx,
                       int priority,
                       const char* file,
                       int line,
                       const char* fn,
                       const char* format,
                       va_list args) noexcept override
    {
        (void)ctx;
        (void)priority;
        (void)file;
        (void)line;

        fprintf(stderr, "%s: ", fn);
        vfprintf(stderr, format, args);
    }
};

ContextPtr
make_context(const Path& prefix)
{
    return std::make_shared<Context>(prefix);
}

ContextPtr
make_context(std::string locale, const Path& prefix)
{
    return std::make_shared<Context>(locale, prefix);
}

Context::Context(const Path& /* prefix */)
  : m_pimpl(std::make_unique<PrivateContextImpl>())
{

    m_pimpl->log_fn = std::make_unique<vle_log_stderr>();
#ifndef VLE_DISABLE_DEBUG
    m_pimpl->log_priority = VLE_LOG_DEBUG;
#else
    m_pimpl->log_priority = VLE_LOG_ERR;
#endif

    initHomeDir();
    initPrefixDir();
    initVleHomeDirectory();

    reset_settings();

    Path conf = getConfigurationFile();
    if (not conf.is_file())
        write_settings();
    else
        load_settings();

    vInfo(this,
          "Context initialized [prefix=%s] [home=%s]\n",
          m_pimpl->m_prefix.string().c_str(),
          m_pimpl->m_home.string().c_str());

#if defined(VLE_HAVE_NLS)
    bindtextdomain(VLE_LOCALE_NAME, getLocaleDir().string().c_str());
    textdomain(VLE_LOCALE_NAME);
#endif
}

Context::Context(std::string locale, const Path& /* prefix */)
  : m_pimpl(std::make_unique<PrivateContextImpl>())
{
    m_pimpl->log_fn = std::make_unique<vle_log_stderr>();
#ifndef VLE_DISABLE_DEBUG
    m_pimpl->log_priority = VLE_LOG_DEBUG;
#else
    m_pimpl->log_priority = VLE_LOG_ERR;
#endif

    initHomeDir();
    initPrefixDir();
    initVleHomeDirectory();

    reset_settings();

    Path conf = getConfigurationFile();
    if (not conf.is_file())
        write_settings();
    else
        load_settings();

    if (locale == "C")
        setlocale(LC_ALL, "C");
    else if (not setlocale(LC_ALL, locale.c_str()))
        setlocale(LC_ALL, "C");

    vInfo(this,
          "Context initialized [prefix=%s] [home=%s] [locale=%s]\n",
          m_pimpl->m_prefix.string().c_str(),
          m_pimpl->m_home.string().c_str(),
          locale.c_str());

#if defined(VLE_HAVE_NLS)
    bindtextdomain(VLE_LOCALE_NAME, getLocaleDir().string().c_str());
    textdomain(VLE_LOCALE_NAME);
#endif
}

ContextPtr
Context::clone()
{
    ContextPtr nctx = std::make_shared<Context>();
    nctx->m_pimpl->m_prefix = m_pimpl->m_prefix;
    nctx->m_pimpl->m_home = m_pimpl->m_home;
    for (auto s : nctx->m_pimpl->settings) {
        nctx->m_pimpl->settings.insert(s);
    }
    nctx->m_pimpl->modules = m_pimpl->modules;
    nctx->m_pimpl->log_priority = m_pimpl->log_priority;
    return nctx;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Path
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

const Path&
Context::getPrefixDir() const
{
    return m_pimpl->m_prefix;
}

const Path&
Context::getHomeDir() const
{
    return m_pimpl->m_home;
}

Path
Context::getLocaleDir() const
{
    Path p(m_pimpl->m_prefix);
    p /= "share";
    p /= "locale";

    return p;
}

Path
Context::getHomeFile(const std::string& name) const
{
    Path p(m_pimpl->m_home);
    p /= name;

    return p;
}

Path
Context::getConfigurationFile() const
{
    auto version = vle::version_abi();

    return getHomeFile(utils::format(
      "vle-%d.%d.conf", std::get<0>(version), std::get<1>(version)));
}

Path
Context::getLogFile() const
{
    auto version = vle::version_abi();

    return getHomeFile(utils::format(
      "vle-%d.%d.log", std::get<0>(version), std::get<1>(version)));
}

Path
Context::getLogFile(const std::string& prefix) const
{
    auto version = vle::version_abi();

    auto lf = utils::format("%s-%d.%d.log",
                            prefix.c_str(),
                            std::get<0>(version),
                            std::get<1>(version));

    return getHomeFile(lf);
}

std::vector<Path>
Context::getBinaryPackagesDir() const
{
    std::vector<Path> ret(2);

    ret[0] = m_pimpl->m_home;
    ret[0] /= utils::format("pkgs-%s", vle::string_version_abi().c_str());

    ret[1] = m_pimpl->m_prefix;
    ret[1] /= "lib";
    ret[1] /= utils::format("vle-%s", vle::string_version_abi().c_str());
    ret[1] /= "pkgs";

    return ret;
}

std::vector<Path>
Context::getBinaryPackages() const
{
    std::vector<Path> result;
    const auto& paths = getBinaryPackagesDir();

    for (const auto& elem : paths) {
        if (not elem.is_directory()) {
            vErr(this,
                 _("Package error: '%s' is not a directory\n"),
                 elem.string().c_str());
            continue;
        }

        for (DirectoryIterator it(elem), end; it != end; ++it)
            if (it->is_directory())
                result.emplace_back(it->path());
    }

    return result;
}

Path
Context::getLocalPackageFilename() const
{
    Path p = getHomeDir();
    p /= "local.pkg";

    return p;
}

Path
Context::getRemotePackageFilename() const
{
    Path p = getHomeDir();
    p /= "remote.pkg";

    return p;
}

Path
Context::getTemplateDir() const
{
    Path p(m_pimpl->m_prefix);
    p /= "share";
    p /= utils::format("vle-%s", vle::string_version_abi().c_str());
    p /= "template";

    return p;
}

Path
Context::getTemplate(const std::string& name) const
{
    Path p(m_pimpl->m_prefix);
    p /= "share";
    p /= utils::format("vle-%s", vle::string_version_abi().c_str());
    p /= "template";
    p /= name;

    return p;
}

void
Context::initVleHomeDirectory()
{
    auto version = vle::version_abi();

    Path homedir = getHomeDir();
    homedir /=
      utils::format("pkgs-%d.%d", std::get<0>(version), std::get<1>(version));

    if (not homedir.is_directory()) {
        if (homedir.is_file())
            throw FileError(
              (fmt(_("VLE_HOME must be a directory (%1%)")) % homedir.string())
                .str());

        if (not Path::create_directories(homedir))
            throw FileError(
              (fmt(_("Failed to build VLE_HOME directory (%1%)")) %
               homedir.string())
                .str());
    }
}

void
Context::readHomeDir()
{
    const char* path_str = std::getenv("VLE_HOME");
    std::string path("");
    if (path_str) {
        path.assign(path_str);
    }
    if (not path.empty()) {
        if (Path::is_directory(path)) {
            m_pimpl->m_home = path;
        } else {
            throw FileError(
              (fmt(_("Path: VLE_HOME '%1%' does not exist")) % path).str());
        }
    } else {
        m_pimpl->m_home.clear();
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Log
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void
Context::set_log_function(std::unique_ptr<LogFunctor> fn) noexcept
{
    m_pimpl->log_fn = std::move(fn);

    vInfo(this, "custom logging function registered\n");
}

void
Context::set_log_priority(int priority) noexcept
{
    m_pimpl->log_priority = priority % 8;
}

int
Context::get_log_priority() const noexcept
{
    return m_pimpl->log_priority;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * ContextPrivate.hpp source code to use the pimpl pointer.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void
Context::log(int priority,
             const char* file,
             int line,
             const char* fn,
             const char* format,
             ...) const noexcept
{
    va_list args;

    va_start(args, format);
    m_pimpl->log_fn->write(*this, priority, file, line, fn, format, args);
    va_end(args);
}
}
} // namespace vle utils
