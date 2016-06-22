/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
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


#ifndef VLE_UTILS_CONTEXT_HPP
#define VLE_UTILS_CONTEXT_HPP

#include <vle/utils/Filesystem.hpp>
#include <vle/DllDefines.hpp>
#include <memory>
#include <string>
#include <vector>
#include <iosfwd>

namespace vle { namespace utils {

/**
 * Define a list of directories.
 */
using PathList = std::vector<vle::utils::FSpath>;

class Context;
using ContextPtr = std::shared_ptr<Context>;

/**
 * Build a std::shared_ptr<Context> using "C" as default locale.
 *
 * \e param prefix Default (\e prefix.empty() or \e
 * !prefix.is_directory()), it reads the \e VLE_HOME environment variable
 * and if it fails, try to build prefix from the \e HOME user.
 *
 * \e param locale Default "C" for ASCII, empty for current locale
 * otherwise, you can specify what you want.  \e return An initialized
 * std::shared_ptr<Context>.
 */
VLE_API ContextPtr make_context(const FSpath& prefix = {},
                                std::string locale = {"C"});

/**
 * \e Context manage all users and installation paths of VLE.
 *
 * Linux and Unix versions use compiled paths, Windows version uses
 * registry to store path. And environment variable VLE_HOME is used to
 * change the user home directory for simulators, streams, packages and
 * other plug-ins. If this variable is not defined,
 *
 * This class is a singleton. To use it:
 * @code
 * auto ctx { vle::utils::make_context();
 * std::cout << ctx->getHomeDir();
 * // Return:
 * //  - $HOME/.vle if VLE_HOME is not defined.
 * //  - %HOMEDRIVE%%HOMEPATH%\vle on Windows if VLE_HOME is not defined.
 * //  - $VLE_HOME/toto if VLE_HOME is defined.
 * @endcode
 */
class VLE_API Context
{
public:
    Context(const FSpath& prefix = {}, std::string locale = {"C"});
    ~Context() = default;

    Context(const Context& other) = delete;
    Context& operator=(const Context& other) = delete;
    Context(Context&& other) = delete;
    Context& operator=(Context&& other) = delete;

    /**
     * Return the prefix of the compilation on Unix or installation
     * directory taken from registry on Windows.
     * @return A string path.
     */
    const FSpath& getPrefixDir() const
    { return m_prefix; }

    /**
     * Return the VLE home directory. Default, returns $HOME/.vle on
     * Unix, %HOMEDRIVE%%HOMEPATH%/vle on Windows. If user defines a
     * VLE_HOME environment variable, this function returns the contents of
     * this variable.
     * @return A string path.
     */
    const FSpath& getHomeDir() const
    { return m_home; }

    /**
     * Returns the \e $VLE_HOME/vle-x.y.conf file path.
     *
     * \return A complete path to access the vle configuration file.
     */
    FSpath getConfigurationFile() const;

    /**
     * Return the \e $HOMEVLE/vle-x.y.log file path.
     *
     * \return A complete path to access the vle log file.
     */
    FSpath getLogFile() const;

    /**
     * Get a file from the getHomeDir() directory.
     * @param name The filename to concat.
     */
    FSpath getHomeFile(const std::string& name) const;

    /**
     * Get the locale directory (/usr/share/locale).
     */
    FSpath getLocaleDir() const;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage package
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * Return the $VLE_HOME/packages dirname.
     * @return A string.
     */

    FSpath getBinaryPackagesDir() const;

    /** Returns the list of dirname available in the binary package directory.
     */
    std::vector<std::string> getBinaryPackages() const;

    /**
     * Return the path @e "$VLE_HOME/local.pkg".
     *
     * @return The path @e "$VLE_HOME/local.pkg."
     */
    FSpath getLocalPackageFilename() const;

    /**
     * Return the path @e "$VLE_HOME/remote.pkg".
     *
     * @return The path @e "$VLE_HOME/remote.pkg."
     */
    FSpath getRemotePackageFilename() const;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage template
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * Get the $PREFIX/share/vle-x.y.z/template directory name.
     * @return A string.
     */
    FSpath getTemplateDir() const;

    /**
     * Return the $PREFIX/share/vle-x.y.z/template/name directory
     * name.
     * @path The name of the template.
     * @return A string.
     */
    FSpath getTemplate(const std::string& name) const;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage plug-ins lists
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * Build the VLE_HOME directories:
     * VLE_HOME/lib, doc, src, build, exp.
     */
    void initVleHomeDirectory();

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Usefull functions
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * Find the absolute path to a program
     * @param the program name
     * @param the absolute path to a program
     */
    FSpath findProgram(const std::string& exe);

    /**
     * @brief fill a vector of string with the list of binary packages
     * @param pkglist, the vector to fill
     */
    void fillBinaryPackagesList(std::vector<std::string>& pkglist) const;

private:
    FSpath buildPackageDir(const std::string& name) const;
    FSpath buildPackageFile(const std::string& name) const;
    FSpath buildPackageFile(const std::string& dir,
                                 const FSpath& name) const;
    FSpath buildPackageFile(const std::string& dir1,
                                 const FSpath& dir2,
                                 const FSpath& name) const;

    PathList m_simulator;
    PathList m_stream;
    PathList m_output;
    PathList m_modeling;

    FSpath m_prefix; /*!< the $prefix of installation */
    FSpath m_home; /*!< the $VLE_HOME */

    /**
     * Build the paths from environment variables.
     * @param variable The name of the variable to read.
     * @param out The output parameter to store valid path.
     * @return true if success, false if variable does not exist.
     */
    bool readEnv(const std::string& variable, PathList& out);

    /**
     * Assign to the m_home attribute the content of the VLE_HOME
     * variable if it exist.
     */
    void readHomeDir();

    /**
     * Assign to the m_home attribut the content of the VLE_HOME
     * variable. If this variable does not exist, m_home is initialized with
     * the $HOME/.vle on Unix and %HOMEDRIVE%%HOMEPATH%\vle on Win.
     */
    void initHomeDir();

    /**
     * Assign to the m_prefix attribut the prefix of VLE
     * installation. On Unix, we use the install path, (ie. DESTDIR or
     * CMAKE_INSTALL_PREFIX directories) and the install path on Win readed
     * from the registry.
     */
    void initPrefixDir();

    /**
     * Clear all plug-ins lists.
     */
    void clearPluginDirs();
};

/**
 * Stream operator to show the content of the Path::PathList.
 * @param out The output stream.
 * @param paths The list of path.
 * @return The output stream.
 */
VLE_API std::ostream& operator<<(std::ostream& out, const PathList& paths);

}} // namespace vle utils

#endif
