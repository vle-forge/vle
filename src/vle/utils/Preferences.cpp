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


#include <vle/utils/Preferences.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/version.hpp>
#include <boost/variant.hpp>
#include <fstream>
#include <map>

#ifdef _WIN32
#define VLE_PACKAGE_COMMAND_CONFIGURE           \
    "cmake.exe -G 'MinGW Makefiles' "           \
    "-DWITH_DOC=OFF "                           \
    "-DWITH_TEST=ON "                           \
    "-DWITH_WARNINGS=OFF "                      \
    "-DCMAKE_INSTALL_PREFIX='%1%' "             \
    "-DCMAKE_BUILD_TYPE=RelWithDebInfo .."
#define VLE_PACKAGE_COMMAND_TEST "cmake.exe --build '%1%' --target test"
#define VLE_PACKAGE_COMMAND_BUILD "cmake.exe --build '%1%' --target all"
#define VLE_PACKAGE_COMMAND_INSTALL "cmake.exe --build '%1%' --target install"
#define VLE_PACKAGE_COMMAND_CLEAN "cmake.exe --build '%1%' --target clean"
#define VLE_PACKAGE_COMMAND_PACKAGE "cmake.exe --build '%1%' "  \
    "--target package_source"
#define VLE_COMMAND_TAR "cmake.exe -E tar jcf '%1%' '%2%'"
#define VLE_COMMAND_UNTAR "cmake.exe -E tar jxf '%1%' .'"
#define VLE_COMMAND_URL_GET "curl.exe '%1%' -o '%2%'"
#define VLE_COMMAND_DIR_COPY "cmake.exe -E copy_directory '%1%' '%2%'"
#define VLE_COMMAND_DIR_REMOVE "cmake.exe -E remove_directory '%1%'"
#else
#define VLE_PACKAGE_COMMAND_CONFIGURE           \
    "cmake -DCMAKE_INSTALL_PREFIX='%1%' "       \
    "-DWITH_DOC=OFF "                           \
    "-DWITH_TEST=ON "                           \
    "-DWITH_WARNINGS=ON "                       \
    "-DCMAKE_BUILD_TYPE=RelWithDebInfo .."
#define VLE_PACKAGE_COMMAND_TEST "cmake --build '%1%' --target test"
#define VLE_PACKAGE_COMMAND_BUILD "cmake --build '%1%' --target all"
#define VLE_PACKAGE_COMMAND_INSTALL "cmake --build '%1%' --target install"
#define VLE_PACKAGE_COMMAND_CLEAN "cmake --build '%1%' --target clean"
#define VLE_PACKAGE_COMMAND_PACKAGE "cmake --build '%1%' --target package_source"
#define VLE_COMMAND_TAR "cmake -E tar jcf '%1%' '%2%'"
#define VLE_COMMAND_UNTAR "cmake -E tar jxf '%1%'"
#define VLE_COMMAND_URL_GET "curl --progress-bar '%1%' -o '%2%'"
#define VLE_COMMAND_DIR_COPY "cmake -E copy_directory '%1%' '%2%'"
#define VLE_COMMAND_DIR_REMOVE "cmake -E remove_directory '%1%'"
#endif

namespace vle { namespace utils {

using PreferenceType = boost::variant<bool,
                                      std::string,
                                      long,
                                      double>;

using PreferenceMap = std::map<std::string, PreferenceType>;

struct Preferences::Pimpl
{
    PreferenceMap ppMap;
    std::string mFilename; /**< The filename of the resources file in \c
                              `VLE_HOME/vle.conf'. */

    bool mreadOnly; /**< if TRUE, the file is not saved at exit. */

    Pimpl(bool readOnly, std::string filename)
        : ppMap{
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
        { "vle.packages.configure", std::string(VLE_PACKAGE_COMMAND_CONFIGURE) },
        { "vle.packages.test", std::string(VLE_PACKAGE_COMMAND_TEST) },
        { "vle.packages.build", std::string(VLE_PACKAGE_COMMAND_BUILD) },
        { "vle.packages.install", std::string(VLE_PACKAGE_COMMAND_INSTALL) },
        { "vle.packages.clean", std::string(VLE_PACKAGE_COMMAND_CLEAN) },
        { "vle.packages.package", std::string(VLE_PACKAGE_COMMAND_PACKAGE) },
        { "vle.remote.url",
                std::string("http://www.vle-project.org/pub/" VLE_ABI_VERSION) },
        { "vle.command.tar", std::string(VLE_COMMAND_TAR) },
        { "vle.command.untar", std::string(VLE_COMMAND_UNTAR) },
        { "vle.command.url.get", std::string(VLE_COMMAND_URL_GET) },
        { "vle.command.dir.copy", std::string(VLE_COMMAND_DIR_COPY) },
        { "vle.command.dir.remove", std::string(VLE_COMMAND_DIR_REMOVE) }}
        , mFilename(std::move(filename))
        , mreadOnly(readOnly)
    {
        std::ifstream ifs(mFilename);
        if (not ifs.is_open()) {
            TraceAlways(_("Preferences: fail open '%s'. Use default value."),
                        mFilename.c_str());
            return;
        }

        int l = 0;
        std::string line;
        while (std::getline(ifs, line)) {
            auto equalchar = line.find_first_of('=', 0);
            if (equalchar == std::string::npos or
                equalchar == 0 or
                equalchar == line.size())
                throw utils::InternalError(
                    (fmt(_("Preferences: fail while reading '%s' line '%d'"))
                     % mFilename % l).str());

            std::string key = line.substr(0, equalchar);
            std::string value = line.substr(equalchar + 1, std::string::npos);

            auto it = ppMap.find(key);
            if (it == ppMap.end()) {
                TraceAlways(_("Preferences: unknown key '%s'"), key.c_str());
            } else {
                if (boost::get<bool>(&it->second)){
                    if (value == "1" or value  == "true" or
                        value == "1" or value == "TRUE" or value == "True")
                        it->second = true;
                    else if (value == "false" or value == "0"
                             or value == "FALSE" or value == "False")
                        it->second = false;
                    else
                        TraceAlways(_("Preferences: fail reading boolean value"
                                      " at line %d in %s"), l, value.c_str());
                } else if (boost::get<std::string>(&it->second)) {
                    it->second = value;
                } else if (boost::get<long>(&it->second)) {
                    try {
                        long r = std::stol(value);
                        it->second = r;
                    } catch (const std::exception& /* e */) {
                        TraceAlways(_("Preferences: fail reading integer value"
                                      " at line %d in %s"), l, value.c_str());
                    }
                } else if (boost::get<long>(&it->second)) {
                    try {
                        double r = std::stod(value);
                        it->second = r;
                    } catch (const std::exception& /* e */) {
                        TraceAlways(_("Preferences: fail reading double value"
                                      " at line %d in %s"), l, value.c_str());
                    }
                }
            }
            l++;
        }
    }

    ~Pimpl() noexcept
    {
        try {
            if (not mreadOnly) {
                std::ofstream file(mFilename);

                if (not file.is_open()) {
                    TraceAlways(_("Preferences: fail write '%s'"),
                                mFilename.c_str());
                    return;
                }

                file << std::boolalpha;
                for (const auto& elem : ppMap)
                    file << elem.first
                         << '=' << elem.second << '\n';

                if (file.bad()) {
                    TraceAlways(_("Preferences: fail while writing '%s'"),
                                mFilename.c_str());
                    return;
                }
            }
        } catch (...) {
        }
    }
};

Preferences::Preferences(const std::string& file)
    : mPimpl(new Preferences::Pimpl(
                 false, utils::Path::path().getHomeFile(file)))
{
}

Preferences::Preferences(bool readOnly, const std::string& file)
    : mPimpl(new Preferences::Pimpl(readOnly,
                                    utils::Path::path().getHomeFile(file)))
{
}

Preferences::~Preferences() noexcept = default;

bool Preferences::set(const std::string& key, const std::string& value)
{
    auto it = mPimpl->ppMap.find(key);
    if (it == mPimpl->ppMap.end())
        return false;

    if (not boost::get<std::string>(&it->second))
        return false;

    it->second = value;

    return true;
}

bool Preferences::set(const std::string& key, double value)
{
    auto it = mPimpl->ppMap.find(key);
    if (it == mPimpl->ppMap.end())
        return false;

    if (not boost::get<double>(&it->second))
        return false;

    it->second = value;

    return true;
}

bool Preferences::set(const std::string& key, long value)
{
    auto it = mPimpl->ppMap.find(key);
    if (it == mPimpl->ppMap.end())
        return false;

    if (not boost::get<long>(&it->second))
        return false;

    it->second = value;

    return true;
}

bool Preferences::set(const std::string& key, bool value)
{
    auto it = mPimpl->ppMap.find(key);
    if (it == mPimpl->ppMap.end())
        return false;

    if (not boost::get<bool>(&it->second))
        return false;

    it->second = value;

    return true;
}

std::vector<std::string> Preferences::get() const
{
    std::vector<std::string> ret(mPimpl->ppMap.size());

    std::transform(std::begin(mPimpl->ppMap), std::end(mPimpl->ppMap),
                   std::begin(ret),
                   [](const PreferenceMap::value_type& pair)
                   {
                       return pair.first;
                   });

    return ret;
}

bool Preferences::get(const std::string& key, std::string* value) const
{
    auto it = mPimpl->ppMap.find(key);
    if (it == mPimpl->ppMap.end())
        return false;

    auto ptr = boost::get<std::string>(&it->second);
    if (not ptr)
        return false;

    *value = *ptr;

    return true;
}

bool Preferences::get(const std::string& key, double* value) const
{
    auto it = mPimpl->ppMap.find(key);
    if (it == mPimpl->ppMap.end())
        return false;

    auto ptr = boost::get<double>(&it->second);
    if (not ptr)
        return false;

    *value = *ptr;

    return true;
}

bool Preferences::get(const std::string& key, long* value) const
{
    auto it = mPimpl->ppMap.find(key);
    if (it == mPimpl->ppMap.end())
        return false;

    auto ptr = boost::get<long>(&it->second);
    if (not ptr)
        return false;

    *value = *ptr;

    return true;
}

bool Preferences::get(const std::string& key, bool* value) const
{
    auto it = mPimpl->ppMap.find(key);
    if (it == mPimpl->ppMap.end())
        return false;

    auto ptr = boost::get<bool>(&it->second);
    if (not ptr)
        return false;

    *value = *ptr;

    return true;
}

}} //namespace vle utils
