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

#ifdef BOOST_WINDOWS
#define VLE_PACKAGE_COMMAND_CONFIGURE "cmake.exe -G 'MinGW Makefiles' " \
    "-DWITH_DOC=OFF "                                                   \
    "-DWITH_TEST=ON "                                                   \
    "-DWITH_WARNINGS=OFF "                                               \
    "-DCMAKE_INSTALL_PREFIX='%1%' "                                     \
    "-DCMAKE_BUILD_TYPE=RelWithDebInfo .."

#define VLE_PACKAGE_COMMAND_TEST "cmake.exe --build '%1%' --target test"
#define VLE_PACKAGE_COMMAND_BUILD "cmake.exe --build '%1%' --target all"
#define VLE_PACKAGE_COMMAND_INSTALL "cmake.exe --build '%1%' --target install"
#define VLE_PACKAGE_COMMAND_CLEAN "cmake.exe --build '%1%' --target clean"
#define VLE_PACKAGE_COMMAND_PACKAGE "cmake.exe --build '%1%' "  \
    "--target package_source"
#define VLE_PACKAGE_COMMAND_UNZIP "unzip.exe"
#else
#define VLE_PACKAGE_COMMAND_CONFIGURE "cmake -DCMAKE_INSTALL_PREFIX='%1%' " \
    "-DWITH_DOC=OFF "                                                       \
    "-DWITH_TEST=ON "                                                       \
    "-DWITH_WARNINGS=ON "                                                   \
    "-DCMAKE_BUILD_TYPE=RelWithDebInfo .."
#define VLE_PACKAGE_COMMAND_TEST "cmake --build '%1%' --target test"
#define VLE_PACKAGE_COMMAND_BUILD "cmake --build '%1%' --target all"
#define VLE_PACKAGE_COMMAND_INSTALL "cmake --build '%1%' --target install"
#define VLE_PACKAGE_COMMAND_CLEAN "cmake --build '%1%' --target clean"
#define VLE_PACKAGE_COMMAND_PACKAGE "cmake --build '%1%' --target package_source"
#define VLE_PACKAGE_COMMAND_UNZIP "unzip"
#endif

namespace po = boost::program_options;

namespace vle { namespace utils {

class Preferences::Pimpl
{
public:
    Pimpl(const std::string& filename)
        : mFilename(filename),
          mGvlePackagesOptions("gvle.packages"),
          mGvleEditorOptions("gvle.editor"),
          mGvleGraphicsOptions("gvle.graphics"),
          mVlePackageOptions("vle.packages"),
          mVleRemoteOptions("vle.remote")
    {
         mGvleEditorOptions.add_options()
            ("gvle.packages.auto-build", po::value < bool >
             (NULL)->default_value(true),
             _("Auto build"));

        mGvleEditorOptions.add_options()
            ("gvle.editor.auto-indent", po::value < bool >
             (NULL)->default_value(true),
             _("Auto indent"))
            ("gvle.editor.font", po::value < std::string >
             (NULL)->default_value("Monospace 10"),
             _("Font"))
            ("gvle.editor.highlight-line", po::value < bool >
             (NULL)->default_value(true),
             _("Highlight line"))
            ("gvle.editor.highlight-brackets", po::value < bool >
             (NULL)->default_value(true),
             _("Highlight line"))
            ("gvle.editor.highlight-syntax", po::value < bool >
             (NULL)->default_value(true),
             _("Highlight syntax"))
            ("gvle.editor.indent-on-tab", po::value < bool >
             (NULL)->default_value(true),
             ("Indent on tabulation"))
            ("gvle.editor.indent-size", po::value < uint32_t >
             (NULL)->default_value(4),
             _("Indentation size"))
            ("gvle.editor.show-line-numbers", po::value < bool >
             (NULL)->default_value(true),
             _("Show line numbers"))
            ("gvle.editor.show-right-margin", po::value < bool >
             (NULL)->default_value(true),
             _("Show right margin"))
            ("gvle.editor.smart-home-end", po::value < bool >
             (NULL)->default_value(true),
             _("Smart Home end"));

        mGvleGraphicsOptions.add_options()
            ("gvle.graphics.background-color", po::value < std::string >
             (NULL)->default_value("#ffffffffffff"))
            ("gvle.graphics.foreground-color", po::value < std::string >
             (NULL)->default_value("#000000000000"))
            ("gvle.graphics.atomic-color", po::value < std::string >
             (NULL)->default_value("#0000ffff0000"))
            ("gvle.graphics.coupled-color", po::value < std::string >
             (NULL)->default_value("#00000000ffff"))
            ("gvle.graphics.selected-color", po::value < std::string >
             (NULL)->default_value("#ffff00000000"))
            ("gvle.graphics.connection-color", po::value < std::string >
             (NULL)->default_value("#000000000000"))
            ("gvle.graphics.font", po::value < std::string >
             (NULL)->default_value("Monospace 10"))
            ("gvle.graphics.font-size", po::value < double >
             (NULL)->default_value(10.0))
            ("gvle.graphics.line-width", po::value < double >
             (NULL)->default_value(3.0));

        mVlePackageOptions.add_options()
            ("vle.packages.configure", po::value < std::string >
             (NULL)->default_value(VLE_PACKAGE_COMMAND_CONFIGURE))
            ("vle.packages.test", po::value < std::string >
             (NULL)->default_value(VLE_PACKAGE_COMMAND_TEST))
            ("vle.packages.build", po::value < std::string >
             (NULL)->default_value(VLE_PACKAGE_COMMAND_BUILD))
            ("vle.packages.install", po::value < std::string >
             (NULL)->default_value(VLE_PACKAGE_COMMAND_INSTALL))
            ("vle.packages.clean", po::value < std::string >
             (NULL)->default_value(VLE_PACKAGE_COMMAND_CLEAN))
            ("vle.packages.package", po::value < std::string >
             (NULL)->default_value(VLE_PACKAGE_COMMAND_PACKAGE))
            ("vle.packages.unzip", po::value < std::string >
             (NULL)->default_value(VLE_PACKAGE_COMMAND_UNZIP));

        mVleDaemonOptions.add_options()
            ("vle.daemon.hosts", po::value < std::string >
             (NULL)->default_value("localhost"))
            ("vle.daemon.ports", po::value < std::string >
             (NULL)->default_value("8001"))
            ("vle.daemon.processes", po::value < std::string >
             (NULL)->default_value("1"));

        mVleRemoteOptions.add_options()
            ("vle.remote.url", po::value < std::string >
             (NULL)->default_value((fmt(
                         "http://www.vle-project.org/pub/%1%.%2%") %
                     VLE_MAJOR_VERSION % VLE_MINOR_VERSION).str()))
            ("vle.remote.proxy_ip", po::value < std::string >
             (NULL)->default_value(""))
            ("vle.remote.proxy_port", po::value < std::string >
             (NULL)->default_value(""));

        mConfigFileOptions.add(mVlePackageOptions).
            add(mGvleEditorOptions).add(mGvleGraphicsOptions).
            add(mVleDaemonOptions).add(mVleRemoteOptions);

        open();
    }

    ~Pimpl()
    {
        save();
    }

    void open()
    {
        std::ifstream file(mFilename.c_str());

        if (file.is_open()) {
            po::store(po::parse_config_file(file, mConfigFileOptions, false),
                      mSettings);
            po::notify(mSettings);
        }
    }

    void save() throw()
    {
        std::ofstream file(mFilename.c_str());

        if (file.is_open()) {
            for (po::variables_map::const_iterator it = mSettings.begin();
                 it != mSettings.end(); ++it) {
                file << it->first << "=";
                if (boost::any_cast < std::string >(&it->second.value())) {
                    file << boost::any_cast
                        < std::string >(it->second.value());
                } else if (boost::any_cast < bool >(&it->second.value())) {
                    file << boost::any_cast
                        < bool >(it->second.value());
                } else if (boost::any_cast < double >(&it->second.value())) {
                    file << boost::any_cast
                        < double >(it->second.value());
                } else if (boost::any_cast < uint32_t
                           >(&it->second.value())) {
                    file << boost::any_cast
                        < uint32_t>(it->second.value());
                } else {
                    TraceAlways(_("Preferences: unknown"));
                    assert(false);
                }
                file << "\n";
            }
        }
    }

    bool set(const std::string& key, const boost::any& value)
    {
        po::variables_map::iterator it = mSettings.find(key);

        if (it != mSettings.end()) {
            it->second.value() = value;
            return true;
        }

        return false;
    }

    boost::any get(const std::string& key)
    {
        po::variables_map::iterator it = mSettings.find(key);

        if (it != mSettings.end()) {
            return it->second.value();
        } else {
            return boost::any();
        }
    }

    void get(std::vector < std::string >* sections) const
    {
        sections->resize(mSettings.size());

        std::transform(mSettings.begin(),
                       mSettings.end(),
                       sections->begin(),
                       CopyVariableName());
    }

private:
    std::string mFilename; /**< The filename of the resources file in \c
                             `VLE_HOME/vle.conf'. */

    po::variables_map mSettings; /**< The variables_map which stores all the
                                   parameters in a std::map < std::string,
                                   po::variable_value >. */

    po::options_description mGvlePackagesOptions;
    po::options_description mConfigFileOptions;
    po::options_description mGvleEditorOptions;
    po::options_description mGvleGraphicsOptions;
    po::options_description mVlePackageOptions;
    po::options_description mVleDaemonOptions;
    po::options_description mVleRemoteOptions;

    /**
     * A functor to get the variable name in the @c
     * boost::program_options::variables_map.
     */
    struct CopyVariableName
    {
        inline const std::string&
            operator()(const po::variables_map::value_type& v) const
            {
                return v.first;
            }
    };
};

Preferences::Preferences(const std::string& file)
    : mPimpl(new Preferences::Pimpl(utils::Path::path().getHomeFile(file)))
{
}

Preferences::~Preferences()
{
    delete mPimpl;
}

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

void Preferences::get(std::vector < std::string >* sections) const
{
    mPimpl->get(sections);
}

bool Preferences::get(const std::string& key, std::string* value) const
{
    boost::any result = mPimpl->get(key);

    try {
        (*value) = boost::any_cast < std::string >(result);
        return true;
    } catch (const boost::bad_any_cast& /*e*/) {
        TraceAlways((fmt(_("Preferences: %1% is not a string")) % key));
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
        TraceAlways((fmt(_("Preferences: %1% is not a real")) % key));
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
        TraceAlways((fmt(_("Preferences: %1% is not an integer")) % key));
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
        TraceAlways((fmt(_("Preferences: %1% is not a bool")) % key));
        return false;
    }
}

}} //namespace vle utils
