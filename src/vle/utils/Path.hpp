/**
 * @file vle/utils/Path.hpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef VLE_UTILS_PATH_HPP
#define VLE_UTILS_PATH_HPP

#include <string>
#include <list>
#include <glibmm/fileutils.h>


namespace vle { namespace utils {

    /**
     * @brief Portable way, i.e. Linux/Unix/Windows to get VLE paths. Linux
     * and Unix versions use compiled paths, Windows version uses registry to
     * store path.
     * And environment variable VLE_HOME is used to change the user home
     * directory for simulators, streams, packages and other plug-ins. If this
     * variable is not defined,
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
    class Path
    {
    public:
        /**
         * @brief Define a list of directories.
         */
        typedef std::list < std::string > PathList;

        /**
         * @brief Return the prefix of the compilation on Unix or installation
         * directory taken from registry on Windows.
         * @return A string path.
         */
        const std::string& getPrefixDir() const
        { return m_prefix; }

        /**
         * @brief Return the VLE home directory. Default, returns $HOME/.vle on
         * Unix, %HOMEDRIVE%%HOMEPATH%/vle on Windows. If user defines a
         * VLE_HOME environment variable, this function returns the contents of
         * this variable.
         * @return A string path.
         */
        const std::string& getHomeDir() const
        { return m_home; }

        std::string getHomeFile(const std::string& name) const;

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

        /*
         * simulation paths
         */

        std::string getSimulatorDir() const;
        std::string getHomeSimulatorDir() const;

        /*
         * examples
         */

        std::string getExamplesDir() const;
        std::string getExampleFile(const std::string& file) const;

        /*
         * stream paths
         */

        std::string getStreamDir() const;
        std::string getHomeStreamDir() const;

        /*
         * output paths
         */

        std::string getOutputDir() const;
        std::string getHomeOutputDir() const;
        std::string getOutputFile(const std::string& file) const;
        std::string getHomeOutputFile(const std::string& file) const;

        std::string getOutputPixmapDir() const;
        std::string getHomeOutputPixmapDir() const;
        std::string getOutputPixmapFile(const std::string& file) const;
        std::string getHomeOutputPixmapFile(const std::string& file) const;

        std::string getOutputGladeDir() const;
        std::string getHomeOutputGladeDir() const;
        std::string getOutputGladeFile(const std::string& file) const;
        std::string getHomeOutputGladeFile(const std::string& file) const;

        std::string getOutputDocDir() const;
        std::string getHomeOutputDocDir() const;
        std::string getOutputDocFile(const std::string& file) const;
        std::string getHomeOutputDocFile(const std::string& file) const;

        /*
         * condition paths
         */

        std::string getConditionDir() const;
        std::string getHomeConditionDir() const;
        std::string getConditionFile(const std::string& file) const;
        std::string getHomeConditionFile(const std::string& file) const;

        std::string getConditionPixmapDir() const;
        std::string getHomeConditionPixmapDir() const;
        std::string getConditionPixmapFile(const std::string& file) const;
        std::string getHomeConditionPixmapFile(const std::string& file) const;

        std::string getConditionGladeDir() const;
        std::string getHomeConditionGladeDir() const;
        std::string getConditionGladeFile(const std::string& file) const;
        std::string getHomeConditionGladeFile(const std::string& file) const;

        std::string getConditionDocDir() const;
        std::string getHomeConditionDocDir() const;
        std::string getConditionDocFile(const std::string& file) const;
        std::string getHomeConditionDocFile(const std::string& file) const;

        /*
         * packages path
         */

        std::string getPackagesDir() const;
        std::string getPackageDir(const std::string& name) const;
        std::string getPackageSimulatorsDir(const std::string& name) const;
        std::string getPackageSourcesDir(const std::string& name) const;
        std::string getPackageDatasDir(const std::string& name) const;
        std::string getPackageExperimentDir(const std::string& name) const;

        /**
         * @brief Get the list of simulator's directories.
         * @return A list of directory name.
         */
        const PathList& getSimulatorDirs() const
        { return m_simulator; }

        /**
         * @brief Get the list of stream's directories.
         * @return A list of directory name.
         */
        const PathList& getStreamDirs() const
        { return m_stream; }

        /**
         * @brief Get the list of output directories.
         * @return A list of directory name.
         */
        const PathList& getOutputDirs() const
        { return m_output; }

        /**
         * @brief Get the list of condition directories.
         * @return A list of directory name.
         */
        const PathList& getConditionDirs() const
        { return m_condition; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Manage singleton
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Return a Path object instantiate in singleton method.
         * @return A reference to the singleton object.
         */
        inline static Path& path()
        { if (mPath == 0) mPath = new Path; return *mPath; }

        /**
         * @brief Initialize the Path singleton.
         */
        inline static void init()
        { path(); }

        /**
         * @brief Delete Path object instantiate from function path().
         */
        inline static void kill()
        { delete mPath; mPath = 0; }

    private:
        void addSimulatorDir(const std::string& dirname);

        void addStreamDir(const std::string& dirname);

        void addOutputDir(const std::string& dirname);

        void addConditionDir(const std::string& dirname);

        std::string buildHomePath(const std::string& name) const;
        std::string buildHomePath(const std::string& directory,
                                  const std::string& name) const;
        std::string buildHomePath(const std::string& dir1,
                                  const std::string& dir2,
                                  const std::string& name) const;

        std::string buildLibrariesPath(const std::string& name) const;
        std::string buildLibrariesPath(const std::string& directory,
                                       const std::string& name) const;
        std::string buildLibrariesPath(const std::string& dir1,
                                       const std::string& dir2,
                                       const std::string& name) const;

        std::string buildSharePath(const std::string& name) const;
        std::string buildSharePath(const std::string& directory,
                                   const std::string& name) const;
        std::string buildSharePath(const std::string& dir1,
                                   const std::string& dir2,
                                   const std::string& name) const;

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Functor
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Predicate functor to test the existence of a directory.
         */
        struct IsDirectory
            : public std::unary_function < std::string, bool >
        {
            /**
             * @brief Check if the directory exits.
             * @param the string to check.
             * @return true if the directory exists, false otherwise.
             */
            bool operator()(const std::string& dirname) const
            {
                return Glib::file_test(dirname, Glib::FILE_TEST_EXISTS |
                                       Glib::FILE_TEST_IS_DIR);
            }
        };

        /**
         * @brief Check if the directory exists.
         * @param dirname the directory to check.
         * @return true if str is a directory.
         */
        static bool isDirectory(const std::string& dirname)
        {
            return Glib::file_test(dirname, Glib::FILE_TEST_EXISTS |
                                   Glib::FILE_TEST_IS_DIR);
        }

        PathList    m_simulator;
        PathList    m_stream;
        PathList    m_output;
        PathList    m_condition;

        std::string m_prefix; /*!< the $prefix of installation */
        std::string m_home; /*!< the $VLE_HOME */

        /**
         * @brief A default constructor that call the initPath member.
         * @throw utils::InternalError if Initialisation of the class failed.
         */
        Path();

        /**
         * @brief Build the paths. This function exist in two part of the file,
         * on for Unix/Linux another for Windows for registry access.
         * @return true if success, false otherwise.
         */
        bool initPath();

        /**
         * @brief Build the paths from environment variables.
         * @param variable The name of the variable to read.
         * @param out The output parameter to store valid path.
         * @return true if success, false if variable does not exist.
         */
        bool readEnv(const std::string& variable, PathList& out);

        /**
         * @brief Assign to the m_home attribut the content of the VLE_HOME
         * variable. If this variable does not exist, m_home is initialized with
         * the $HOME/.vle on Unix and %HOMEDRIVE%%HOMEPATH%\vle on Win.
         */
        void initHomeDir();

        /**
         * @brief Assign to the m_home attribute the content of the VLE_HOME
         * variable if it exist.
         */
        void readHomeDir();

        static Path* mPath; /*!< The static variable Path for singleton design
                              pattern. */
    };

    /**
     * @brief Stream operator to show the content of the Path::PathList.
     * @param out The output stream.
     * @param paths The list of path.
     * @return The output stream.
     */
    std::ostream& operator<<(std::ostream& out, const Path::PathList& paths);

    /**
     * @brief Stream operator to show the content of the Path class.
     * @param out The output stream.
     * @param p The Path to show.
     * @return The output stream.
     */
    std::ostream& operator<<(std::ostream& out, const Path& p);

}} // namespace vle utils

#endif
