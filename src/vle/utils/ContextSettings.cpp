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

#include <fstream>
#include <vle/utils/Context.hpp>
#include <vle/utils/ContextPrivate.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/vle.hpp>

#ifdef _WIN32
#define VLE_PACKAGE_COMMAND_CONFIGURE                                         \
    "cmake.exe -G 'MinGW Makefiles' "                                         \
    "-DWITH_DOC=OFF "                                                         \
    "-DWITH_TEST=ON "                                                         \
    "-DWITH_WARNINGS=OFF "                                                    \
    "-DCMAKE_INSTALL_PREFIX='%1%' "                                           \
    "-DCMAKE_BUILD_TYPE=RelWithDebInfo .."
#define VLE_PACKAGE_COMMAND_TEST "cmake.exe --build '%1%' --target test"
#define VLE_PACKAGE_COMMAND_BUILD "cmake.exe --build '%1%' --target all"
#define VLE_PACKAGE_COMMAND_INSTALL "cmake.exe --build '%1%' --target install"
#define VLE_PACKAGE_COMMAND_CLEAN "cmake.exe --build '%1%' --target clean"
#define VLE_PACKAGE_COMMAND_PACKAGE                                           \
    "cmake.exe --build '%1%' "                                                \
    "--target package_source"
#define VLE_COMMAND_TAR "cmake.exe -E tar jcf '%1%' '%2%'"
#define VLE_COMMAND_UNTAR "cmake.exe -E tar jxf '%1%' ."
#define VLE_COMMAND_URL_GET "curl.exe '%1%' -o '%2%'"
#define VLE_COMMAND_DIR_COPY "cmake.exe -E copy_directory '%1%' '%2%'"
#define VLE_COMMAND_DIR_REMOVE "cmake.exe -E remove_directory '%1%'"
#define VLE_COMMAND_VLE_SIMULATION "vle.exe --write-output '%1%' '%2%'"
#else
#define VLE_PACKAGE_COMMAND_CONFIGURE                                         \
    "cmake -DCMAKE_INSTALL_PREFIX='%1%' "                                     \
    "-DWITH_DOC=OFF "                                                         \
    "-DWITH_TEST=ON "                                                         \
    "-DWITH_WARNINGS=ON "                                                     \
    "-DCMAKE_BUILD_TYPE=RelWithDebInfo .."
#define VLE_PACKAGE_COMMAND_TEST "cmake --build '%1%' --target test"
#define VLE_PACKAGE_COMMAND_BUILD "cmake --build '%1%' --target all"
#define VLE_PACKAGE_COMMAND_INSTALL "cmake --build '%1%' --target install"
#define VLE_PACKAGE_COMMAND_CLEAN "cmake --build '%1%' --target clean"
#define VLE_PACKAGE_COMMAND_PACKAGE                                           \
    "cmake --build '%1%' --target package_source"
#define VLE_COMMAND_TAR "cmake -E tar jcf '%1%' '%2%'"
#define VLE_COMMAND_UNTAR "cmake -E tar jxf '%1%'"
#define VLE_COMMAND_URL_GET "curl --progress-bar '%1%' -o '%2%'"
#define VLE_COMMAND_DIR_COPY "cmake -E copy_directory '%1%' '%2%'"
#define VLE_COMMAND_DIR_REMOVE "cmake -E remove_directory '%1%'"
#define VLE_COMMAND_VLE_SIMULATION                                            \
    "vle-" VLE_ABI_VERSION " --write-output '%1%' '%2%'"
#endif

namespace vle {
namespace utils {

void
Context::reset_settings() noexcept
{
    m_pimpl->settings = {
        { "gvle.packages.auto-build", true },
        { "gvle.editor.auto-indent", true },
        { "gvle.editor.font", std::string("Monospace 10") },
        { "gvle.editor.highlight-line", true },
        { "gvle.editor.highlight-brackets", true },
        { "gvle.editor.highlight-syntax", true },
        { "gvle.editor.indent-on-tab", true },
        { "gvle.editor.indent-size", 4l },
        { "gvle.editor.show-line-numbers", true },
        { "gvle.editor.show-right-margin", true },
        { "gvle.editor.smart-home-end", true },
        { "gvle.graphics.background-color", std::string("#ffffffffffff") },
        { "gvle.graphics.foreground-color", std::string("#000000000000") },
        { "gvle.graphics.atomic-color", std::string("#0000ffff0000") },
        { "gvle.graphics.coupled-color", std::string("#00000000ffff") },
        { "gvle.graphics.selected-color", std::string("#ffff00000000") },
        { "gvle.graphics.connection-color", std::string("#000000000000") },
        { "gvle.graphics.font", std::string("Monospace 10") },
        { "gvle.graphics.font-size", 10.0 },
        { "gvle.graphics.line-width", 3.0 },
        { "vle.simulation.thread", 0l },
        { "vle.simulation.block-size", 8l },
        { "vle.packages.configure",
          std::string(VLE_PACKAGE_COMMAND_CONFIGURE) },
        { "vle.packages.test", std::string(VLE_PACKAGE_COMMAND_TEST) },
        { "vle.packages.build", std::string(VLE_PACKAGE_COMMAND_BUILD) },
        { "vle.packages.install", std::string(VLE_PACKAGE_COMMAND_INSTALL) },
        { "vle.packages.clean", std::string(VLE_PACKAGE_COMMAND_CLEAN) },
        { "vle.packages.package", std::string(VLE_PACKAGE_COMMAND_PACKAGE) },
        { "vle.command.tar", std::string(VLE_COMMAND_TAR) },
        { "vle.command.untar", std::string(VLE_COMMAND_UNTAR) },
        { "vle.command.url.get", std::string(VLE_COMMAND_URL_GET) },
        { "vle.command.dir.copy", std::string(VLE_COMMAND_DIR_COPY) },
        { "vle.command.dir.remove", std::string(VLE_COMMAND_DIR_REMOVE) }
    };

    auto version = vle::version_abi();

    auto url = utils::format("http://www.vle-project.org/pub/%d.%d",
                             std::get<0>(version),
                             std::get<1>(version));

    m_pimpl->settings["vle.remote.url"] = url;

    std::string simulation = "";
#ifdef _WIN32
    simulation = "vle.exe";
    simulation += " --write-output '%1%' '%2%'";
#else
    simulation = utils::format("vle-%s", vle::string_version_abi().c_str());
    simulation += " --write-output '%1%' '%2%'";
#endif
    m_pimpl->settings["vle.command.vle.simulation"] = simulation;
}

bool
Context::load_settings() noexcept
{
    std::ifstream ifs(getConfigurationFile().string());
    if (not ifs.is_open()) {
        vErr(this,
             _("Settings: fail to read configuration file `%s'."
               " Use default settings instead.\n"),
             getConfigurationFile().string().c_str());
        return false;
    }

    int l = 0;
    std::string line;
    while (std::getline(ifs, line)) {
        auto equalchar = line.find_first_of('=', 0);
        if (equalchar == std::string::npos or equalchar == 0 or
            equalchar == line.size()) {
            vErr(this,
                 _("Settings: fail while reading '%s' line '%d'\n"),
                 getConfigurationFile().string().c_str(),
                 l);
            return false;
        }

        std::string key = line.substr(0, equalchar);
        std::string value = line.substr(equalchar + 1, std::string::npos);

        auto it = m_pimpl->settings.find(key);
        if (it == m_pimpl->settings.end()) {
            vInfo(this, _("Settings: unknown key `%s'\n"), key.c_str());
            continue;
        }

        if (boost::get<bool>(&it->second)) {
            if (value == "1" or value == "true" or value == "1" or
                value == "TRUE" or value == "True")
                it->second = true;
            else if (value == "false" or value == "0" or value == "FALSE" or
                     value == "False")
                it->second = false;
            else
                vErr(this,
                     _("Settings: fail reading boolean value"
                       " at line %d in %s\n"),
                     l,
                     value.c_str());
        } else if (boost::get<std::string>(&it->second)) {
            it->second = value;
        } else if (boost::get<long>(&it->second)) {
            try {
                long r = std::stol(value);
                it->second = r;
            } catch (const std::exception& /* e */) {
                vErr(this,
                     _("Settings: fail reading integer value"
                       " at line %d in %s\n"),
                     l,
                     value.c_str());
            }
        } else if (boost::get<long>(&it->second)) {
            try {
                double r = std::stod(value);
                it->second = r;
            } catch (const std::exception& /* e */) {
                vErr(this,
                     _("Settings: fail reading double value"
                       " at line %d in %s\n"),
                     l,
                     value.c_str());
            }
        }
        l++;
    }

    return true;
}

bool
Context::write_settings() const noexcept
{
    std::ofstream ofs(getConfigurationFile().string());
    if (not ofs.is_open()) {
        vErr(this,
             _("Settings: fail to write configuration file `%s'\n"),
             getConfigurationFile().string().c_str());
        return false;
    }

    ofs << std::boolalpha;
    for (const auto& elem : m_pimpl->settings)
        ofs << elem.first << '=' << elem.second << '\n';

    if (ofs.bad()) {
        vErr(this,
             _("Settings: fail while writing configuration file `%s'\n"),
             getConfigurationFile().string().c_str());
        return false;
    }

    return true;
}

bool
Context::set_setting(const std::string& key, const std::string& value) noexcept
{
    auto it = m_pimpl->settings.find(key);
    if (it == m_pimpl->settings.end())
        return false;

    if (not boost::get<std::string>(&it->second))
        return false;

    it->second = value;

    return true;
}

bool
Context::set_setting(const std::string& key, double value) noexcept
{
    auto it = m_pimpl->settings.find(key);
    if (it == m_pimpl->settings.end())
        return false;

    if (not boost::get<double>(&it->second))
        return false;

    it->second = value;

    return true;
}

bool
Context::set_setting(const std::string& key, long value) noexcept
{
    auto it = m_pimpl->settings.find(key);
    if (it == m_pimpl->settings.end())
        return false;

    if (not boost::get<long>(&it->second))
        return false;

    it->second = value;

    return true;
}

bool
Context::set_setting(const std::string& key, bool value) noexcept
{
    auto it = m_pimpl->settings.find(key);
    if (it == m_pimpl->settings.end())
        return false;

    if (not boost::get<bool>(&it->second))
        return false;

    it->second = value;

    return true;
}

bool
Context::get_setting(const std::string& key, std::string* value) const noexcept
{
    auto it = m_pimpl->settings.find(key);
    if (it == m_pimpl->settings.end())
        return false;

    auto ptr = boost::get<std::string>(&it->second);
    if (not ptr)
        return false;

    *value = *ptr;

    return true;
}

bool
Context::get_setting(const std::string& key, double* value) const noexcept
{
    auto it = m_pimpl->settings.find(key);
    if (it == m_pimpl->settings.end())
        return false;

    auto ptr = boost::get<double>(&it->second);
    if (not ptr)
        return false;

    *value = *ptr;

    return true;
}

bool
Context::get_setting(const std::string& key, long* value) const noexcept
{
    auto it = m_pimpl->settings.find(key);
    if (it == m_pimpl->settings.end())
        return false;

    auto ptr = boost::get<long>(&it->second);
    if (not ptr)
        return false;

    *value = *ptr;

    return true;
}

bool
Context::get_setting(const std::string& key, bool* value) const noexcept
{
    auto it = m_pimpl->settings.find(key);
    if (it == m_pimpl->settings.end())
        return false;

    auto ptr = boost::get<bool>(&it->second);
    if (not ptr)
        return false;

    *value = *ptr;

    return true;
}
}
} // namespace vle utils
