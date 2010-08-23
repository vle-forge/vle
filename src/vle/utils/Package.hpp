/**
 * @file vle/utils/Package.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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
         * @brief Try to unzip the provided file.
         * @param file The filename to unzip.
         */
        void unzip(const std::string& file);

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

        void addFile(const std::string& path, const std::string& name);
        void addDirectory(const std::string& path, const std::string& name);
        void removeFile(const std::string& pathFile);
        void renameFile(const std::string& oldFile, std::string& newName);
        void copyFile(const std::string& sourceFile, std::string& targetName);

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
        Package()
            : m_stop(true), m_success(false), m_out(0), m_err(0), m_wait(0),
            m_pid(0)
        {}

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

        /**
         * @brief Start the process taken from the list of argument in the
         * working directory. Launch the process asynchronously an start two
         * threads to read output and error standard stream and move data into
         * stream.
         * @param workingDir The directory to start the process.
         * @param lst The command line argument.
         */
        void process(const std::string& workingDir,
                     const std::list < std::string >& lst);

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
    };

}} // namespace vle utils

#endif
