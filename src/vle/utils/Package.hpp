/*
 * @file vle/utils/Package.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#ifndef VLE_UTILS_PACKAGE_HPP
#define VLE_UTILS_PACKAGE_HPP

#include <vle/utils/DllDefines.hpp>
#include <vle/utils/PackageTable.hpp>
#include <glibmm/spawn.h>
#include <glibmm/thread.h>
#include <string>
#include <list>

namespace vle { namespace utils {

    /**
     * @brief A class to manage VLE packages, ie. to build a new package, to
     * configure, build, clean or to make tarball.
     *
     * This class is a singleton. To use it:
     * @code
     * std::string name = utils::Package::package().name();
     * std::string path = utils::Package::package().path();
     *
     * utils::Package::package().select("firemanqss");
     * utils::Package::Package().configure(std::cout, std::cerr);
     * @encode
     */
    class VLE_UTILS_EXPORT Package
    {
    public:
        /**
         * @brief Build the package.
         */
        void create();

                          /*   threaded commands   */

        /**
         * @brief Configure the package by running the 'cmake' command.
         */
        void configure();

        /**
         * @brief Launch unit test by running the 'make test' command.
         */
        void test();

        /**
         * @brief Build the package by running the 'make all' command.
         */
        void build();

        /**
         * @brief Install the package by running the 'make install' command from
         * src directory.
         */
        void install();

        /**
         * @brief Clean the package by running the 'make clean' command and
         * remove the CMakeCache.txt file.
         */
        void clean();

        /**
         * @brief Pack the package by runngin the 'make package' and 'make
         * package_source' command.
         */
        void pack();

        /**
         * @brief Try to unzip the provided file into the \c package directory.
         *
         * @param package The name of the directory.
         * @param file The filename to unzip.
         */
        void unzip(const std::string& package, const std::string& file);

        /**
         * @brief Return true if the current process is finished.
         * @return True if finish, false otherwise.
         */
        inline bool isFinish() const { return m_stop; }

        /**
         * @brief Return true if the latest execution of a process returns true
         * of false.
         *
         * @return True if latest process success, false otherwise.
         */
        bool isSuccess() const { return m_success; }

        /**
         * @brief Wait the end of the current process and flush all data from
         * the process to the std::ostream object.
         *
         * @param out Standard output stream.
         * @param err Standard error stream.
         */
        void wait(std::ostream& out, std::ostream& err);

        void getOutput(std::string& str) const;
        void getError(std::string& str) const;
        void getOutputAndClear(std::string& str);
        void getErrorAndClear(std::string& str);


        /**
         * @brief Change the current directory to the output directory. If we
         * are in package mode, the output directory will be create. If a file
         * named output directory exists, nothing is done.
         */
        void changeToOutputDirectory();

        /**
         * @brief Remove the specified package from the user directory.
         *
         * @param package The package to remove (recursively).
         */
        void removePackage(const std::string& package);


        /**
         * @brief Test if the specified package exists in the user directory.
         *
         * @param package The package to test.
         *
         * @return true if the package already exist, false otherwise.
         */
        bool existsPackage(const std::string& package);

        /**
         * @brief Add an empty file into the current Package.
         *
         * @param path The path, for example: "data" to build a new file
         * "PKG/data/name".
         * @param name The name of the file.
         */
        void addFile(const std::string& path, const std::string& name);

        /**
         * @brief Return true if a file package/path/to/file exists in the
         * package package.
         *
         * @param path The path, for example: "data/input1.dat" to test a file
         * "PKG/data/input1.dat".
         *
         * @return true if file exists, false otherwise.
         */
        bool existsFile(const std::string& path);

        /**
         * @brief Add an empty directgory into the current Package.
         *
         * @param path The path, for example: "data" to build a new directory
         * "PKG/data/name".
         * @param name The name of the directory.
         */
        void addDirectory(const std::string& path, const std::string& name);

        /**
         * @brief Return true if a directory package/path/to/file exists in the
         * package package.
         *
         * @param path The path, for example: "data/input1.dat" to test a file
         * "PKG/data/input1.dat".
         *
         * @return true if directory exists, false otherwise.
         */
        bool existsDirectory(const std::string& path);

        /**
         * @brief Remove file or directory and (recursively) the specified path.
         *
         * @param path The name of the file or the directory.
         */
        void remove(const std::string& path);

        /**
         * @brief Rename the path oldname to the new name newname.
         * @code
         * utils::Package::package().rename("output/exp_view1.dat", "toto.dat");
         *     // rename PKG/output/exp_view1.dat in PKG/output/toto.dat")
         *     // and return the string "PKG/output/toto.dat".
         * @endcode
         *
         * @param oldname The path, in PKG, of the file to rename.
         * @param newname The new name.
         *
         * @return The string representation of the path of the new file.
         *
         * @throw utils::ArgError if the file already exists or if the old file
         * does not exist.
         */
        std::string rename(const std::string& oldname,
                           const std::string& newname);

        /**
         * @brief Copy the file.
         *
         * @param source The file source.
         * @param target The destination path.
         */
        void copy(const std::string& source, std::string& target);

        /**
         * @brief Get the name of the current selected Package. Get the current
         * selected Package. If the name is empty, no package is selected.
         * @return The current selected Package.
         */
        const std::string& name() const
        { return m_table.current(); }

        /**
         * @brief Is a package selected.
         * @return Return true if a Package is selected, false otherwise.
         */
        bool selected() const
        { return not m_table.current().empty(); }

        /**
         * @brief Select a new Package.
         * @param name The new Package.
         */
        void select(const std::string& name);

        /**
         * @brief Refresh the command line option from the `vle.conf' file.
         */
        void refresh();

        /**
         * @brief Get an identifiant for the specified package. If the name is
         * not defined in the table list, it will be add.
         * @param name The package to get an Id.
         * @return An id.
         */
        PackageTable::index getId(const std::string& package)
        { return m_table.get(package); }

                           /*   manage singleton   */

        /**
         * @brief Return a instance of Package using the singleton system.
         * @return A reference to the singleton object.
         */
        inline static Package& package()
        { if (m_package == 0) m_package = new Package; return *m_package; }

        /**
         * @brief Initialise the Package singleton.
         */
        inline static void init()
        { package(); }

        /**
         * @brief Delete the Package singleton.
         */
        inline static void kill()
        { delete m_package; m_package = 0; }

    private:
        /**
         * @brief Hide constructor.
         */
        Package();

        static Package* m_package; ///< singleton attribute.
        PackageTable m_table; ///< the list of package available.
        bool m_stop; ///< true if process is stopped.
        bool m_success; ///< true if process is successfull.
        Glib::Thread* m_out; ///< standard output stream reader thread.
        Glib::Thread* m_err; ///< standard error stream reader thread.
        Glib::Thread* m_wait; ///< wait process thread.
        mutable Glib::Mutex m_mutex; ///< mutex to access to strerr and strout.
        Glib::Pid m_pid; ///< pid of the current process.
        std::string m_strout; ///< standard output string.
        std::string m_strerr; ///< standard error string.

        std::string mCommandConfigure;
        std::string mCommandTest;
        std::string mCommandBuild;
        std::string mCommandInstall;
        std::string mCommandClean;
        std::string mCommandPack;
        std::string mCommandUnzip;

        /**
         * @brief Start the process taken from the list of argument in the
         * working directory. Launch the process asynchronously an start two
         * threads to read output and error standard stream and move data into
         * stream.
         * @param workingDir The directory to start the process.
         * @param argv The command line argument.
         * @param envp The environment variable.
         */
        void process(const std::string& workingDir,
                     const std::list < std::string >& argv,
                     const std::list < std::string >& envp =
                     std::list < std::string >());

        /**
         * @brief A threaded method to read the stream and fill the output.
         * @param stream The stream (ie. a file) to read error.
         * @param output A string filled by the stream.
         */
        void readStandardOutputStream(int stream);

        /**
         * @brief A threaded method to read the stream and fill the output.
         * @param stream The stream (ie. a file) to read error.
         * @param output A string filled by the stream.
         */
        void readStandardErrorStream(int stream);

        void waitProcess();

        void appendOutput(const char* str);
        void appendError(const char* str);
        void appendOutput(const std::string& str);
        void appendError(const std::string& str);

        /**
         * @brief Split the string `cmd' into argv argumenent.
         * @param cmd
         * @param argv
         */
        void buildCommandLine(const std::string& cmd,
                              std::list < std::string >& argv);
    };

}} // namespace vle utils

#endif
