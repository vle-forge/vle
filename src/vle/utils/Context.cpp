/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
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
#include <vle/utils/Filesystem.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/vle.hpp>

#include "utils/ContextPrivate.hpp"
#include "utils/i18n.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#ifdef _WIN32
#include "utils/details/UtilsWin.hpp"

#include <Windows.h>
#endif

namespace vle {
namespace utils {

struct vle_log_stderr : Context::LogFunctor
{
    void write(const Context& /*ctx*/,
               int priority,
               const char* format,
               va_list args) noexcept override
    {
        auto* stream = stderr;

        if (priority == 7)
            fputs("debug: ", stream);
        else if (priority == 4)
            fputs("warning: ", stream);
        else if (priority == 3)
            fputs("error: ", stream);
        else if (priority == 2)
            fputs("critical: ", stream);
        else if (priority == 1)
            fputs("alert: ", stream);
        else if (priority == 0)
            fputs("emergency: ", stream);
        else
            stream = stdout;

        vfprintf(stream, format, args);
    }

    void write(const Context& /*ctx*/,
               int priority,
               const std::string& str) noexcept override
    {
        if (priority == 7)
            fprintf(stderr, "debug: %s", str.c_str());
        else if (priority == 4)
            fprintf(stderr, "warning: %s", str.c_str());
        else if (priority == 3)
            fprintf(stderr, "error: %s", str.c_str());
        else if (priority == 2)
            fprintf(stderr, "critical: %s", str.c_str());
        else if (priority == 1)
            fprintf(stderr, "alert: %s", str.c_str());
        else if (priority == 0)
            fprintf(stderr, "emergency: %s", str.c_str());
        else
            fprintf(stdout, "%s", str.c_str());
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

#ifdef _WIN32
    /* Starting from VLE 2.0, Windows port changes is own environment
     * variables PATH, CMAKE_MODULE_PATH and PKG_CONFIG_PATH simplify source
     * code of VLE.
     *
     * https://msdn.microsoft.com/en-us/library/windows/desktop/ms682009(v=vs.85).aspx
     */

    {
        auto path = getPrefixDir();
        path /= L"bin";

        auto str = path.wstring();
        auto var = get_environment_variable_wide(L"PATH");

        if (var.find(str) == var.npos) {
            if (not var.empty()) {
                str += L';';
                str += var;
            }

            set_environment_variable_wide(L"PATH", str);
        }
    }

    {
        auto path = getPrefixDir();
        path /= L"bin";

        auto str = path.wstring();
        auto var = get_environment_variable_wide(L"CMAKE_MODULE_PATH");

        if (var.find(str) == var.npos) {
            if (not var.empty()) {
                str += L';';
                str += var;
            }

            set_environment_variable_wide(L"CMAKE_MODULE_PATH", str);
        }
    }

    {
        auto path = getPrefixDir();
        path /= L"lib";
        path /= L"pkgconfig";

        set_environment_variable_wide(L"PKG_CONFIG_PATH", path.wstring());
    }

    {
        auto path = getPrefixDir();
        set_environment_variable_wide(L"VLE_BASEPATH", path.wstring());
    }

    {
        this->debug(_("Environment\n"));
        LPWCH env = ::GetEnvironmentStringsW();
        for (LPWSTR vars = static_cast<PWSTR>(env); *vars;
             vars += wcslen(vars) + 1) {
            try {
                std::wstring str(vars);

                if (not str.empty() or str[0] != L'=' or
                    str.find(L'=') != str.npos)
                    this->debug(_("%s\n"), from_wide_to_utf8(str).c_str());
            } catch (const std::exception& /*e*/) {
            }
        }

        FreeEnvironmentStringsW(env);
    }
#endif

    // To determine which version of VLE is running the sub-processor (CMake
    // for example), we dynamically define a environment variable VLE_VERSION
    // equal to the current ABI version.

#ifdef _WIN32
    set_environment_variable("VLE_VERSION", string_version_abi());
#else
    ::setenv("VLE_VERSION", string_version_abi().c_str(), 1);
#endif

#if defined(VLE_HAVE_NLS)
    bindtextdomain(E_NAME, getLocaleDir().string().c_str());
    textdomain(E_NAME);
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

#ifdef _WIN32
    /* Starting from VLE 2.0, Windows port changes is own environment
     * variables PATH, CMAKE_MODULE_PATH and PKG_CONFIG_PATH simplify source
     * code of VLE.
     *
     * https://msdn.microsoft.com/en-us/library/windows/desktop/ms682009(v=vs.85).aspx
     */

    {
        auto path = getPrefixDir();
        path /= L"bin";

        auto str = path.wstring();
        auto var = get_environment_variable_wide(L"PATH");

        if (var.find(str) == var.npos) {
            if (not var.empty()) {
                str += L';';
                str += var;
            }

            set_environment_variable_wide(L"PATH", str);
        }
    }

    {
        auto path = getPrefixDir();
        path /= L"bin";

        auto str = path.wstring();
        auto var = get_environment_variable_wide(L"CMAKE_MODULE_PATH");

        if (var.find(str) == var.npos) {
            if (not var.empty()) {
                str += L';';
                str += var;
            }

            set_environment_variable_wide(L"CMAKE_MODULE_PATH", str);
        }
    }

    {
        auto path = getPrefixDir();
        path /= L"lib";
        path /= L"pkgconfig";

        set_environment_variable_wide(L"PKG_CONFIG_PATH", path.wstring());
    }

    {
        auto path = getPrefixDir();
        set_environment_variable_wide(L"VLE_BASEPATH", path.wstring());
    }

    {
        this->debug(_("Environment\n"));
        LPWCH env = ::GetEnvironmentStringsW();
        for (LPWSTR vars = static_cast<PWSTR>(env); *vars;
             vars += wcslen(vars) + 1) {
            try {
                std::wstring str(vars);

                if (not str.empty() or str[0] != L'=' or
                    str.find(L'=') != str.npos)
                    this->debug(_("%s\n"), from_wide_to_utf8(str).c_str());
            } catch (const std::exception& /*e*/) {
            }
        }

        FreeEnvironmentStringsW(env);
    }
#endif

#if defined(VLE_HAVE_NLS)
    bindtextdomain(E_NAME, getLocaleDir().string().c_str());
    textdomain(E_NAME);
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
    Path path = getPrefixDir();
    path /= "share";
    path /= "locale";

    return path;
}

Path
Context::getHomeFile(const std::string& name) const
{
    Path path = getHomeDir();
    path /= name;

    return path;
}

Path
Context::getConfigurationFile() const
{
    auto path = getHomeDir();
    path /= "vle.conf";

    return path;
}

Path
Context::getLogFile() const
{
    auto path = getHomeDir();
    path /= "vle.log";

    return path;
}

Path
Context::getLogFile(const std::string& prefix) const
{
    auto path = prefix + ".log";

    return getHomeFile(path);
}

std::vector<Path>
Context::getBinaryPackagesDir() const
{
    std::vector<Path> ret(2);

    ret[0] = m_pimpl->m_home;
    ret[0] /= "pkgs";

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
            this->error(_("Package error: '%s' is not a directory\n"),
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
    Path homedir = getHomeDir();
    homedir /= "pkgs";

    if (not homedir.is_directory()) {
        if (homedir.is_file())
            throw FileError(_("VLE_HOME must be a directory (%s)"),
                            homedir.string().c_str());

        if (not Path::create_directories(homedir))
            throw FileError(_("Failed to build VLE_HOME directory (%s)"),
                            homedir.string().c_str());
    }
}

void
Context::readHomeDir()
{
#ifdef _WIN32
    auto vlehome = get_environment_variable("VLE_HOME");

    if (not vlehome.empty()) {
        Path path(vlehome);
        path /= format("vle-%d", VERSION_MAJOR * 1000 + VERSION_MINOR);

        if (path.is_directory()) {
            m_pimpl->m_home = path;
            return;
        }

        path.create_directories();
        if (path.is_directory()) {
            m_pimpl->m_home = path;
            return;
        }
    }

    auto homedrive = get_environment_variable("HOMEDRIVE");
    auto homepath = get_environment_variable("HOMEPATH");

    Path home(homedrive);
    home /= homepath;
    home /= "vle";
    home /= format("vle-%d", VERSION_MAJOR * 1000 + VERSION_MINOR);

    if (home.is_directory()) {
        m_pimpl->m_home = home;
        return;
    }

    if (home.create_directories()) {
        m_pimpl->m_home = home;
        return;
    }

    std::string description("Failed to initialize home directory in `");
    description += home.string();
    description += "` or using the VLE_HOME environment variable.";

    show_message_box(description);
#else
    char* cvlehome = std::getenv("VLE_HOME");
    if (cvlehome) {
        Path home(cvlehome);
        home /= format("vle-%d", VERSION_MAJOR * 1000 + VERSION_MINOR);

        if (home.is_directory()) {
            m_pimpl->m_home = home;
            return;
        }

        home.create_directories();
        if (home.is_directory()) {
            m_pimpl->m_home = home;
            return;
        }

        this->info("$VLE_HOME (%s) defined but not usable.\n",
                   home.string().c_str());
    }

    char* chome = std::getenv("HOME");
    if (chome) {
        Path home(chome);
        home /= ".vle";
        home /= format("vle-%d", VERSION_MAJOR * 1000 + VERSION_MINOR);

        if (home.is_directory()) {
            m_pimpl->m_home = home;
            return;
        }

        if (home.create_directories()) {
            m_pimpl->m_home = home;
            return;
        }

        this->info("$HOME/.vle (%s) not usable.\n", home.string().c_str());
    }
#endif

    m_pimpl->m_home = ".";
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

    this->info("custom logging function registered\n");
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
Context::log(int priority, const std::string& str) const
{
    if (m_pimpl->log_priority >= priority)
        m_pimpl->log_fn->write(*this, priority, str);
}

void
Context::log(int priority, const char* format, ...) const
{
    if (m_pimpl->log_priority >= priority) {
        va_list args;
        va_start(args, format);
        m_pimpl->log_fn->write(*this, priority, format, args);
        va_end(args);
    }
}

void
Context::emergency(const std::string& str) const
{
    if (m_pimpl->log_priority >= VLE_LOG_EMERG)
        m_pimpl->log_fn->write(*this, VLE_LOG_EMERG, str);
}

void
Context::emergency(const char* format, ...) const
{
    if (m_pimpl->log_priority >= VLE_LOG_EMERG) {
        va_list args;

        va_start(args, format);
        m_pimpl->log_fn->write(*this, VLE_LOG_EMERG, format, args);
        va_end(args);
    }
}

void
Context::alert(const std::string& str) const
{
    if (m_pimpl->log_priority >= VLE_LOG_ALERT)
        m_pimpl->log_fn->write(*this, VLE_LOG_ALERT, str);
}

void
Context::alert(const char* format, ...) const
{
    if (m_pimpl->log_priority >= VLE_LOG_ALERT) {
        va_list args;
        va_start(args, format);
        m_pimpl->log_fn->write(*this, VLE_LOG_ALERT, format, args);
        va_end(args);
    }
}

void
Context::critical(const std::string& str) const
{
    if (m_pimpl->log_priority >= VLE_LOG_CRIT)
        m_pimpl->log_fn->write(*this, VLE_LOG_CRIT, str);
}

void
Context::critical(const char* format, ...) const
{
    if (m_pimpl->log_priority >= VLE_LOG_CRIT) {
        va_list args;
        va_start(args, format);
        m_pimpl->log_fn->write(*this, VLE_LOG_CRIT, format, args);
        va_end(args);
    }
}

void
Context::error(const std::string& str) const
{
    if (m_pimpl->log_priority >= VLE_LOG_ERR)
        m_pimpl->log_fn->write(*this, VLE_LOG_ERR, str);
}

void
Context::error(const char* format, ...) const
{
    if (m_pimpl->log_priority >= VLE_LOG_ERR) {
        va_list args;
        va_start(args, format);
        m_pimpl->log_fn->write(*this, VLE_LOG_ERR, format, args);
        va_end(args);
    }
}

void
Context::warning(const std::string& str) const
{
    if (m_pimpl->log_priority >= VLE_LOG_WARNING)
        m_pimpl->log_fn->write(*this, VLE_LOG_WARNING, str);
}

void
Context::warning(const char* format, ...) const
{
    if (m_pimpl->log_priority >= VLE_LOG_WARNING) {
        va_list args;
        va_start(args, format);
        m_pimpl->log_fn->write(*this, VLE_LOG_WARNING, format, args);
        va_end(args);
    }
}

void
Context::notice(const std::string& str) const
{
    if (m_pimpl->log_priority >= VLE_LOG_NOTICE)
        m_pimpl->log_fn->write(*this, VLE_LOG_NOTICE, str);
}

void
Context::notice(const char* format, ...) const
{
    if (m_pimpl->log_priority >= VLE_LOG_NOTICE) {
        va_list args;
        va_start(args, format);
        m_pimpl->log_fn->write(*this, VLE_LOG_NOTICE, format, args);
        va_end(args);
    }
}

void
Context::info(const std::string& str) const
{
    if (m_pimpl->log_priority >= VLE_LOG_INFO)
        m_pimpl->log_fn->write(*this, VLE_LOG_INFO, str);
}

void
Context::info(const char* format, ...) const
{
    if (m_pimpl->log_priority >= VLE_LOG_INFO) {
        va_list args;
        va_start(args, format);
        m_pimpl->log_fn->write(*this, VLE_LOG_INFO, format, args);
        va_end(args);
    }
}

void
Context::debug(const std::string& str) const
{
#ifdef VLE_DISABLE_DEBUG
    (void)str;
#else
    if (m_pimpl->log_priority >= VLE_LOG_DEBUG)
        m_pimpl->log_fn->write(*this, VLE_LOG_DEBUG, str);
#endif
}

void
Context::debug(const char* format, ...) const
{
#ifdef VLE_DISABLE_DEBUG
    (void)format;
#else
    if (m_pimpl->log_priority >= VLE_LOG_DEBUG) {
        va_list args;
        va_start(args, format);
        m_pimpl->log_fn->write(*this, VLE_LOG_DEBUG, format, args);
        va_end(args);
    }
#endif
}
}
} // namespace vle utils
