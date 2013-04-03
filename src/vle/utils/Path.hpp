/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2013 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2013 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2013 INRA http://www.inra.fr
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
typedef std::vector < std::string > PathList;

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
public:
    /**
     * Rebuild path lists of plug-ins directories (package mode,
     * global mode etc.).
     */
    void updateDirs();

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
    std::string getPackagesDir() const;

    /**
     * Return the $VLE_HOME/packages/current_package dirname.
     * @return A string.
     */
    std::string getPackageSourceDir() const; // TODO returns source directory : current path + package
    std::string getPackageBinaryDir() const; // TODO returns binary directory : $VLE_HOME/pkgs-1.1 + package
    std::string getPackageLibDir() const;
    std::string getPackageSrcDir() const;
    std::string getPackageDataDir() const;
    std::string getPackageDocDir() const;
    std::string getPackageExpDir() const;
    std::string getPackageBuildDir() const; // TODO check current path + package + buildvle
    std::string getPackageOutputDir() const;
    std::string getPackagePluginDir() const;
    std::string getPackagePluginSimulatorDir() const;
    std::string getPackagePluginOutputDir() const;
    std::string getPackagePluginGvleGlobalDir() const;
    std::string getPackagePluginGvleModelingDir() const;
    std::string getPackagePluginGvleOutputDir() const;

    std::string getPackageFile(const std::string& name) const;
    std::string getPackageLibFile(const std::string& name) const;
    std::string getPackageSrcFile(const std::string& name) const;
    std::string getPackageDataFile(const std::string& name) const;
    std::string getPackageDocFile(const std::string& name) const;
    std::string getPackageExpFile(const std::string& name) const;
    std::string getPackageOutputFile(const std::string& name) const;
    std::string getPackagePluginFile(const std::string& name) const;
    std::string getPackagePluginSimulatorFile(const std::string& name) const;
    std::string getPackagePluginOutputFile(const std::string& name) const;
    std::string getPackagePluginGvleModelingFile(const std::string& name) const;
    std::string getPackagePluginGvleOutputFile(const std::string& name) const;

    std::string getExternalPackageDir(const std::string& name) const;
    std::string getExternalPackageLibDir(const std::string& name) const;
    std::string getExternalPackageSrcDir(const std::string& name) const;
    std::string getExternalPackageDataDir(const std::string& name) const;
    std::string getExternalPackageDocDir(const std::string& name) const;
    std::string getExternalPackageExpDir(const std::string& name) const;
    std::string getExternalPackageBuildDir(const std::string& name) const;
    std::string getExternalPackageOutputDir(const std::string& name) const;
    std::string getExternalPackagePluginDir(const std::string& name) const;
    std::string getExternalPackagePluginSimulatorDir(const std::string& name) const;
    std::string getExternalPackagePluginOutputDir(const std::string& name) const;
    std::string getExternalPackagePluginGvleGlobalDir(const std::string& name) const;
    std::string getExternalPackagePluginGvleModelingDir(const std::string& name) const;
    std::string getExternalPackagePluginGvleOutputDir(const std::string& name) const;

    std::string getExternalPackageFile(const std::string& name,
                                       const std::string& file) const;
    std::string getExternalPackageLibFile(const std::string& name,
                                          const std::string& file) const;
    std::string getExternalPackageSrcFile(const std::string& name,
                                          const std::string& file) const;
    std::string getExternalPackageDataFile(const std::string& name,
                                           const std::string& file) const;
    std::string getExternalPackageDocFile(const std::string& name,
                                          const std::string& file) const;
    std::string getExternalPackageExpFile(const std::string& name,
                                          const std::string& file) const;
    std::string getExternalPackageOutputFile(const std::string& package,
                                             const std::string& file) const;
    std::string getExternalPackagePluginFile(const std::string& package,
                                             const std::string& file) const;
    std::string getExternalPackagePluginSimulatorFile(const std::string& package,
                                                      const std::string& file) const;
    std::string getExternalPackagePluginOutputFile(const std::string& package,
                                                   const std::string& file) const;
    std::string getExternalPackagePluginGvleModelingFile(const std::string& package,
                                                         const std::string& file) const;
    std::string getExternalPackagePluginGvleOutputFile(const std::string& package,
                                                       const std::string& file) const;

    PathList getInstalledPackages();
    PathList getInstalledExperiments();
    PathList getInstalledLibraries();


    /**
     * Browser the parameter path and retreive the package if
     * it exists.
     * @code
     * // return toto if /home/foo/bar/ corresponds to VLE_HOME.
     * getPackageFromPath("/home/foo/bar/pkgs-x.y/toto/lib/tutu.so");
     * // return empty string.
     * getPackageFromPath("/home/foo/tutu.so");
     * // return foo if /home/bar/ corresponds to VLE_HOME.
     * getPackageFromPath("/home/bar/pkgs-x.y/tutu.so");
     * @endcode
     * @param path The path to get the package.
     * @return The package founded into the string `path' or an empty
     * string if not found.
     */
    static std::string getPackageFromPath(const std::string& path);


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

    /**
     * Copy the file or directory name from the
     * $PREFIX/share/vle-x.y.z/template/name into the output directory.
     * @param name The name of the template to copy.
     * @param to The destination of the template.
     */
    void copyTemplate(const std::string& name, const std::string& to) const;

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

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage singleton
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * Return a Path object instantiate in singleton method.
     * @return A reference to the singleton object.
     */
    inline static Path& path()
    { if (mPath == 0) mPath = new Path; return *mPath; }

    /**
     * Initialize the Path singleton.
     */
    inline static void init()
    { path(); }

    /**
     * Delete Path object instantiate from function path().
     */
    inline static void kill()
    { delete mPath; mPath = 0; }

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
     * Get the current path of the process.
     *
     * @return The current path of the process.
     */
    static std::string getCurrentPath();

    /**
     * Check if the current path exist (file or directory).
     * @param filename The name of the file to check.
     * @return True if the file is a regular file.
     */
    static bool exist(const std::string& filename);

    /**
     * Check if the current filename corresponds to a regular file.
     * @param filename The name of the file to check.
     * @return True if the file is a regular file.
     */
    static bool existFile(const std::string& filename);

    /**
     * Check if the current filename corresponds to a directory.
     * @param filename The name of the directory to check.
     * @return True if the file is a directory.
     */
    static bool existDirectory(const std::string& filename);

    /**
     * Return the filename of the current path.
     * @code
     * std::string s = "/tmp/test";
     * std::string file = filename(s); // dir == test
     *
     * std::string x = "/tmp/test.toto";
     * std::string file = filename(s); // dir == test.toto
     * @endcode
     *
     * @param filename The path.
     *
     * @return The filename in the path.
     */
    static std::string filename(const std::string& filename);

    /**
     * Return the basename of the current path.
     * @code
     * std::string s = "/tmp/test";
     * std::string file = basename(s); // dir == test
     *
     * std::string x = "/tmp/test.toto";
     * std::string file = basename(s); // dir == test
     * @endcode
     * @param filename The path.
     * @return The basename.
     */
    static std::string basename(const std::string& filename);

    /**
     * Return the dirname of the current path.
     * @code
     * std::string s = "/tmp/test";
     * std::string dir = dirname(s); // dir == /tmp
     * @endcode
     * @param filename The path.
     * @return The dirname.
     */
    static std::string dirname(const std::string& filename);

    /**
     * Return the extension of the current path.
     * @param filename The path.
     * @return The extension.
     */
    static std::string extension(const std::string& filename);

    /**
     * return the parent directory of a file or directory
     * @param path the absolute filepath
     * @return a string with the new path
     */
    std::string getParentPath(const std::string& path);

    /**
     * build a portable temporary filename like:
     * @code
     * $TMP/<filename> on Unix/Linux
     * $ c:\windows\tmp\<filename> on Windows
     * @endcode
     * @param filename string to add on temporary directory
     * @return a portable temporary filename
     */
    static std::string buildTemp( const std::string& filename);

    /**
     * Get and open a temporary file.
     *
     * Get from the operating system a temporary directory, for example on
     * Unix @c /tmp or @c c:\\windows\\temp on Win32 and build a file name
     * from the template @c template.
     *
     * @code
     * std::ofstream file;
     * std::string result = utils::getTempFile("test-vle", &file);
     * file << "Hello\n";
     *
     * std::cout << result; // For example, on Unix: /tmp/test-vle123251
     * @endcode
     *
     * @param[in] prefix The prefix of the filename you want.
     * @param[out] file A pointer to an existing std::ofstream not opened.
     *
     * @return The path name of the file.
     */
    static std::string getTempFile(const std::string& prefix,
                                   std::ofstream* file);

    /**
     * Write specified buffer to temporary file and return the
     * filename. The file is created in the temporary directory.
     * @param prefix if you want to prefix the temporary file.
     * @param buffer buffer to write.
     * @return filename of the new temporary file.
     */
    static std::string writeToTemp(const std::string& prefix,
                                   const std::string& buffer);

    static std::string buildFilename(const std::string& dir,
                                     const std::string& file);
    static std::string buildFilename(const std::string& dir1,
                                     const std::string& dir2,
                                     const std::string& file);
    static std::string buildFilename(const std::string& dir1,
                                     const std::string& dir2,
                                     const std::string& dir3,
                                     const std::string& file);
    static std::string buildFilename(const std::string& dir1,
                                     const std::string& dir2,
                                     const std::string& dir3,
                                     const std::string& dir4,
                                     const std::string& file);
    static std::string buildFilename(const std::string& dir1,
                                     const std::string& dir2,
                                     const std::string& dir3,
                                     const std::string& dir4,
                                     const std::string& dir5,
                                     const std::string& file);

    static std::string buildDirname(const std::string& dir1,
                                    const std::string& dir2);

    static std::string buildDirname(const std::string& dir1,
                                    const std::string& dir2,
                                    const std::string& dir3);
    static std::string buildDirname(const std::string& dir1,
                                    const std::string& dir2,
                                    const std::string& dir3,
                                    const std::string& dir4);
    static std::string buildDirname(const std::string& dir1,
                                    const std::string& dir2,
                                    const std::string& dir3,
                                    const std::string& dir4,
                                    const std::string& dir5);


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

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Functor
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * Predicate functor to test the existence of a directory.
     */
    struct IsDirectory
        : public std::unary_function < std::string, bool >
    {
        /**
         * Check if the directory exits.
         * @param the string to check.
         * @return true if the directory exists, false otherwise.
         */
        bool operator()(const std::string& dirname) const
        {
            return utils::Path::existDirectory(dirname);
        }
    };

    PathList    m_simulator;
    PathList    m_stream;
    PathList    m_output;
    PathList    m_modeling;

    std::string m_prefix; /*!< the $prefix of installation */
    std::string m_home; /*!< the $VLE_HOME */
    std::string m_currentPackagePath; /*< the current binary package path */
    std::string m_currentPackageSourcePath; /*< the current source path */

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
     * A default constructor. This function clear the default
     * directories, initialize the home, prefix, plug-ins and package
     * directories.
     *
     * @throw utils::InternalError if Initialisation of the class failed.
     */
    Path();

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

    /**
     * Assign to the plug-ins directories lists paths from prefix and
     * home directory.
     */
    void initGlobalPluginDirs();

    /**
     * Assign the current package path and simulator packages lists
     * paths from prefix or vle user dir.
     */
    void initPackagePluginDirs();

    static Path* mPath; /**< The static variable Path for singleton
                           design pattern. */
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
