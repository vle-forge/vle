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


#ifndef VLE_UTILS_PATH_HPP
#define VLE_UTILS_PATH_HPP

#include <string>
#include <vector>
#include <iosfwd>
#include <vle/DllDefines.hpp>

namespace vle { namespace utils {

/**
 * Define a list of directories.
 */
using PathList = std::vector<std::string>;

/**
 * Portable way to get and manage VLE's paths.
 *
 * Linux and Unix versions use compiled paths, Windows version uses
 * registry to store path. And environment variable VLE_HOME is used to
 * change the user home directory for simulators, streams, packages and
 * other plug-ins. If this variable is not defined,
 *
 * This class is a singleton. To use it:
 * @code
 * std::string file(utils::Path::path().getHomeDir("toto");
 * // Return:
 * //  - $HOME/.vle/toto if VLE_HOME is not defined.
 * //  - %HOMEDRIVE%%HOMEPATH%\vle on Windows if VLE_HOME is not defined.
 * //  - $VLE_HOME/toto if VLE_HOME is defined.
 * @endcode
 */
class VLE_API Path
{
private:
    Path();
    ~Path() noexcept;

public:
    Path(const Path& other) = delete;
    Path& operator=(const Path& other) = delete;
    Path(Path&& other) = delete;
    Path& operator=(Path&& other) = delete;

    /**
     * A singleton function that guaranteed to be destroyed. Instantiated
     * on first use.
     *
     * @return Return the only one refernce to the Path object.
     */
    static Path& path()
    {
        static Path p;
        return p;
    }

    /**
     * Return the prefix of the compilation on Unix or installation
     * directory taken from registry on Windows.
     * @return A string path.
     */
    const std::string& getPrefixDir() const
    { return m_prefix; }

    /**
     * Return the VLE home directory. Default, returns $HOME/.vle on
     * Unix, %HOMEDRIVE%%HOMEPATH%/vle on Windows. If user defines a
     * VLE_HOME environment variable, this function returns the contents of
     * this variable.
     * @return A string path.
     */
    const std::string& getHomeDir() const
    { return m_home; }

    /**
     * Get a file from the getHomeDir() directory.
     * @param name The filename to concat.
     */
    std::string getHomeFile(const std::string& name) const;

    /**
     * Get the locale directory (/usr/share/locale).
     */
    std::string getLocaleDir() const;

    /*
     * pixmap path
     */

    std::string getPixmapDir() const;
    std::string getPixmapFile(const std::string& file) const;

    /*
     * glade path
     */

    std::string getGladeDir() const;
    std::string getGladeFile(const std::string& file) const;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage package
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * Return the $VLE_HOME/packages dirname.
     * @return A string.
     */

    std::string getBinaryPackagesDir() const;
    std::string getCurrentDir() const;

    PathList getBinaryPackages();
    PathList getBinaryLibraries();

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage template
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * Get the $PREFIX/share/vle-x.y.z/template directory name.
     * @return A string.
     */
    std::string getTemplateDir() const;

    /**
     * Return the $PREFIX/share/vle-x.y.z/template/name directory
     * name.
     * @path The name of the template.
     * @return A string.
     */
    std::string getTemplate(const std::string& name) const;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage plug-ins lists
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * Get the list of simulator's directories.
     * @return A list of directory name.
     */
    const PathList& getSimulatorDirs() const
    { return m_simulator; }

    /**
     * Get the list of stream's directories.
     * @return A list of directory name.
     */
    const PathList& getStreamDirs() const
    { return m_stream; }

    /**
     * Get the list of output directories.
     * @return A list of directory name.
     */
    const PathList& getOutputDirs() const
    { return m_output; }

    /**
     * Get the list of modeling directories.
     * @return A list of directory name.
     */
    const PathList& getModelingDirs() const
    { return m_modeling; }

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

    static void compress(const std::string& filepath,
                         const std::string& compressedfilepath);

    static void decompress(const std::string& compressedfilepath,
                           const std::string& directorypath);

    /**
     * Find the absolute path to a program
     * @param the program name
     * @param the absolute path to a program
     */
    static std::string findProgram(const std::string& exe);

    /**
     * @brief fill a vector of string with the list of binary packages
     * @param pkglist, the vector to fill
     */
    static void fillBinaryPackagesList(
        std::vector<std::string>& pkglist);

private:
    void addSimulatorDir(const std::string& dirname);

    void addStreamDir(const std::string& dirname);

    void addOutputDir(const std::string& dirname);

    void addModelingDir(const std::string& dirname);

    std::string buildPackageDir(const std::string& name) const;
    std::string buildPackageFile(const std::string& name) const;
    std::string buildPackageFile(const std::string& dir,
                                 const std::string& name) const;
    std::string buildPackageFile(const std::string& dir1,
                                 const std::string& dir2,
                                 const std::string& name) const;

    PathList m_simulator;
    PathList m_stream;
    PathList m_output;
    PathList m_modeling;

    std::string m_prefix; /*!< the $prefix of installation */
    std::string m_home; /*!< the $VLE_HOME */

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

/**
 * Stream operator to show the content of the Path class.
 * @param out The output stream.
 * @param p The Path to show.
 * @return The output stream.
 */
VLE_API std::ostream& operator<<(std::ostream& out, const Path& p);

}} // namespace vle utils

#endif
