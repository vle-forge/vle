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
#include <boost/program_options.hpp>
#include <boost/program_options/variables_map.hpp>
#include <fstream>

#ifdef _WIN32
#define VLE_PACKAGE_COMMAND_CONFIGURE                                   \
    "cmake.exe -G 'MinGW Makefiles' "                                   \
    "-DWITH_DOC=OFF "                                                   \
    "-DWITH_TEST=ON "                                                   \
    "-DWITH_WARNINGS=OFF "                                              \
    "-DCMAKE_INSTALL_PREFIX='%1%' "                                     \
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
#else
#define VLE_PACKAGE_COMMAND_CONFIGURE                                   \
    "cmake -DCMAKE_INSTALL_PREFIX='%1%' "                               \
    "-DWITH_DOC=OFF "                                                   \
    "-DWITH_TEST=ON "                                                   \
    "-DWITH_WARNINGS=ON "                                               \
    "-DCMAKE_BUILD_TYPE=RelWithDebInfo .."
#define VLE_PACKAGE_COMMAND_TEST "cmake --build '%1%' --target test"
#define VLE_PACKAGE_COMMAND_BUILD "cmake --build '%1%' --target all"
#define VLE_PACKAGE_COMMAND_INSTALL "cmake --build '%1%' --target install"
#define VLE_PACKAGE_COMMAND_CLEAN "cmake --build '%1%' --target clean"
#define VLE_PACKAGE_COMMAND_PACKAGE "cmake --build '%1%' --target package_source"
#define VLE_COMMAND_TAR "cmake -E tar jcf '%1%' '%2%'"
#define VLE_COMMAND_UNTAR "cmake -E tar jxf '%1%'"
#define VLE_COMMAND_URL_GET "curl --progress-bar '%1%' -o '%2%'"
#endif

namespace po = boost::program_options;

namespace vle { namespace utils {

class Preferences::Pimpl
{
public:
    Pimpl(bool readOnly, std::string filename)
        : mGvlePackagesOptions("gvle.packages")
        , mGvleEditorOptions("gvle.editor")
        , mGvleGraphicsOptions("gvle.graphics")
        , mVlePackageOptions("vle.packages")
        , mVleRemoteOptions("vle.remote")
        , mVleCommandOptions("vle.command")
        , mFilename(std::move(filename))
        , mreadOnly(readOnly)
    {
        mGvleEditorOptions.add_options()
            ("gvle.packages.auto-build", po::value <bool>
             (nullptr)->default_value(true),
             _("Auto build"));

        mGvleEditorOptions.add_options()
            ("gvle.editor.auto-indent", po::value < bool >
             (nullptr)->default_value(true),
             _("Auto indent"))
            ("gvle.editor.font", po::value < std::string >
             (nullptr)->default_value("Monospace 10"),
             _("Font"))
            ("gvle.editor.highlight-line", po::value < bool >
             (nullptr)->default_value(true),
             _("Highlight line"))
            ("gvle.editor.highlight-brackets", po::value < bool >
             (nullptr)->default_value(true),
             _("Highlight line"))
            ("gvle.editor.highlight-syntax", po::value < bool >
             (nullptr)->default_value(true),
             _("Highlight syntax"))
            ("gvle.editor.indent-on-tab", po::value < bool >
             (nullptr)->default_value(true),
             ("Indent on tabulation"))
            ("gvle.editor.indent-size", po::value < uint32_t >
             (nullptr)->default_value(4),
             _("Indentation size"))
            ("gvle.editor.show-line-numbers", po::value < bool >
             (nullptr)->default_value(true),
             _("Show line numbers"))
            ("gvle.editor.show-right-margin", po::value < bool >
             (nullptr)->default_value(true),
             _("Show right margin"))
            ("gvle.editor.smart-home-end", po::value < bool >
             (nullptr)->default_value(true),
             _("Smart Home end"));

        mGvleGraphicsOptions.add_options()
            ("gvle.graphics.background-color", po::value < std::string >
             (nullptr)->default_value("#ffffffffffff"))
            ("gvle.graphics.foreground-color", po::value < std::string >
             (nullptr)->default_value("#000000000000"))
            ("gvle.graphics.atomic-color", po::value < std::string >
             (nullptr)->default_value("#0000ffff0000"))
            ("gvle.graphics.coupled-color", po::value < std::string >
             (nullptr)->default_value("#00000000ffff"))
            ("gvle.graphics.selected-color", po::value < std::string >
             (nullptr)->default_value("#ffff00000000"))
            ("gvle.graphics.connection-color", po::value < std::string >
             (nullptr)->default_value("#000000000000"))
            ("gvle.graphics.font", po::value < std::string >
             (nullptr)->default_value("Monospace 10"))
            ("gvle.graphics.font-size", po::value < double >
             (nullptr)->default_value(10.0))
            ("gvle.graphics.line-width", po::value < double >
             (nullptr)->default_value(3.0));

        mVlePackageOptions.add_options()
            ("vle.packages.configure", po::value < std::string >
             (nullptr)->default_value(VLE_PACKAGE_COMMAND_CONFIGURE))
            ("vle.packages.test", po::value < std::string >
             (nullptr)->default_value(VLE_PACKAGE_COMMAND_TEST))
            ("vle.packages.build", po::value < std::string >
             (nullptr)->default_value(VLE_PACKAGE_COMMAND_BUILD))
            ("vle.packages.install", po::value < std::string >
             (nullptr)->default_value(VLE_PACKAGE_COMMAND_INSTALL))
            ("vle.packages.clean", po::value < std::string >
             (nullptr)->default_value(VLE_PACKAGE_COMMAND_CLEAN))
            ("vle.packages.package", po::value < std::string >
             (nullptr)->default_value(VLE_PACKAGE_COMMAND_PACKAGE));

        mVleDaemonOptions.add_options()
            ("vle.daemon.hosts", po::value < std::string >
             (nullptr)->default_value("localhost"))
            ("vle.daemon.ports", po::value < std::string >
             (nullptr)->default_value("8001"))
            ("vle.daemon.processes", po::value < std::string >
             (nullptr)->default_value("1"));

        mVleRemoteOptions.add_options()
            ("vle.remote.url", po::value < std::string >
             (nullptr)->default_value(
                 "http://www.vle-project.org/pub/" VLE_ABI_VERSION))
            ("vle.remote.proxy_ip", po::value < std::string >
             (nullptr)->default_value(""))
            ("vle.remote.proxy_port", po::value < std::string >
             (nullptr)->default_value(""));

        mVleCommandOptions.add_options()
            ("vle.command.tar", po::value<std::string>
             (nullptr)->default_value(VLE_COMMAND_TAR))
            ("vle.command.untar", po::value<std::string>
             (nullptr)->default_value(VLE_COMMAND_UNTAR))
            ("vle.command.url.get", po::value<std::string>
             (nullptr)->default_value(VLE_COMMAND_URL_GET));

        mConfigFileOptions.add(mVlePackageOptions).
            add(mGvleEditorOptions).add(mGvleGraphicsOptions).
            add(mVleDaemonOptions).add(mVleRemoteOptions).
            add(mVleCommandOptions);

        open();
    }

    ~Pimpl() noexcept
    {
        try {
            if (not mreadOnly)
                save();
        } catch (...) {
        }
    }

    void open()
    {
        std::ifstream file(mFilename.c_str());

        if (not file.is_open()) {
            TraceAlways(_("Preferences: fail to open '%s'"),
                        mFilename.c_str());
            return;
        }

        po::store(po::parse_config_file(file, mConfigFileOptions, false),
                  mSettings);
        po::notify(mSettings);
    }

    void save()
    {
        std::ofstream file(mFilename.c_str());

        if (not file.is_open()) {
            TraceAlways(_("Preferences: fail to open '%s'"),
                        mFilename.c_str());
            return;
        }

        for (const auto& elem : mSettings) {
            file << elem.first << "=";

            if (boost::any_cast <std::string>(&elem.second.value()))
                file << boost::any_cast<std::string>(elem.second.value());
            else if (boost::any_cast<bool>(&elem.second.value()))
                file << boost::any_cast<bool>(elem.second.value());
            else if (boost::any_cast<double>(&elem.second.value()))
                file << boost::any_cast<double>(elem.second.value());
            else if (boost::any_cast<uint32_t>(&elem.second.value()))
                file << boost::any_cast<uint32_t>(elem.second.value());
            else
                TraceAlways(_("Preferences: unknown type '%s'"),
                            elem.first.c_str());

            file << "\n";
        }
    }

    bool set(const std::string& key, const boost::any& value)
    {
        auto it = mSettings.find(key);

        if (it != mSettings.end()) {
            it->second.value() = value;
            return true;
        }

        return false;
    }

    boost::any get(const std::string& key)
    {
        auto it = mSettings.find(key);

        if (it != mSettings.end())
            return it->second.value();

        return boost::any();
    }

    std::vector<std::string>
    get() const
    {
        std::vector<std::string> ret(mSettings.size());

        std::transform(std::begin(mSettings), std::end(mSettings),
                       std::begin(ret),
                       [](const po::variables_map::value_type& pair)
                       {
                           return pair.first;
                       });

        return ret;
    }

private:
    po::variables_map mSettings; /**< The variables_map which stores all
                                    the parameters in a std::map <
                                    std::string, po::variable_value >. */
    po::options_description mGvlePackagesOptions;
    po::options_description mConfigFileOptions;
    po::options_description mGvleEditorOptions;
    po::options_description mGvleGraphicsOptions;
    po::options_description mVlePackageOptions;
    po::options_description mVleDaemonOptions;
    po::options_description mVleRemoteOptions;
    po::options_description mVleCommandOptions;

    std::string mFilename; /**< The filename of the resources file in \c
                              `VLE_HOME/vle.conf'. */

    bool mreadOnly; /**< if TRUE, the file is not saved at exit. */
};

Preferences::Preferences(const std::string& file)
    : mPimpl(
        std::unique_ptr<Preferences::Pimpl>(
            new Preferences::Pimpl(
                false, utils::Path::path().getHomeFile(file))))
{
}

Preferences::Preferences(bool readOnly, const std::string& file)
    : mPimpl(
        std::unique_ptr<Preferences::Pimpl>(
            new Preferences::Pimpl(
                readOnly, utils::Path::path().getHomeFile(file))))
{
}

Preferences::~Preferences() noexcept = default;

bool Preferences::set(const std::string& key, const std::string& value)
{
    return mPimpl->set(key, value);
}

bool Preferences::set(const std::string& key, double value)
{
    return mPimpl->set(key, value);
}

bool Preferences::set(const std::string& key, uint32_t value)
{
    return mPimpl->set(key, value);
}

bool Preferences::set(const std::string& key, bool value)
{
    return mPimpl->set(key, value);
}

std::vector<std::string> Preferences::get() const
{
    return mPimpl->get();
}

bool Preferences::get(const std::string& key, std::string* value) const
{
    boost::any result = mPimpl->get(key);

    try {
        (*value) = boost::any_cast < std::string >(result);
        return true;
    } catch (const boost::bad_any_cast& /*e*/) {
        TraceAlways(_("Preferences: %s is not a string"), key.c_str());
        return false;
    }
}

bool Preferences::get(const std::string& key, double* value) const
{
    boost::any result = mPimpl->get(key);

    try {
        (*value) = boost::any_cast < double >(result);
        return true;
    } catch (const boost::bad_any_cast& /*e*/) {
        TraceAlways(_("Preferences: %s is not a real"), key.c_str());
        return false;
    }
}

bool Preferences::get(const std::string& key, uint32_t* value) const
{
    boost::any result = mPimpl->get(key);

    try {
        (*value) = boost::any_cast < uint32_t >(result);
        return true;
    } catch (const boost::bad_any_cast& /*e*/) {
        TraceAlways(_("Preferences: %s is not a integer"), key.c_str());
        return false;
    }
}

bool Preferences::get(const std::string& key, bool* value) const
{
    boost::any result = mPimpl->get(key);

    try {
        (*value) = boost::any_cast < bool >(result);
        return true;
    } catch (const boost::bad_any_cast& /*e*/) {
        TraceAlways(_("Preferences: %s is not a boolean"), key.c_str());
        return false;
    }
}

}} //namespace vle utils
