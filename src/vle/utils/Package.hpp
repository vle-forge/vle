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


#ifndef VLE_UTILS_PACKAGE_HPP
#define VLE_UTILS_PACKAGE_HPP

#include <vle/DllDefines.hpp>
#include <vle/utils/PackageTable.hpp>
#include <string>

namespace vle { namespace utils {

/**
 * A class to manage VLE packages, ie. to build a new package, to
 * configure, build, clean or to make tarball.
 *
 * This class is a singleton. To use it:
 * @code
 * std::string name = utils::Package::package().name();
 * std::string path = utils::Package::package().path();
 *
 * utils::Package::package().select("firemanqss");
 * utils::Package::Package().configure(std::cout, std::cerr);
 * @endcode
 */
class VLE_API Package
{
public:
    ~Package();

    /**
     * Build the package.
     */
    void create();

    /*   threaded commands   */

    /**
     * Configure the package by running the 'cmake' command.
     */
    void configure();

    /**
     * Launch unit test by running the 'make test' command.
     */
    void test();

    /**
     * Build the package by running the 'make all' command.
     */
    void build();

    /**
     * Install the package by running the 'make install' command from
     * src directory.
     */
    void install();

    /**
     * Clean the package by running the 'make clean' command and
     * remove the CMakeCache.txt file.
     */
    void clean();

    /**
     * Pack the package by runngin the 'make package' and 'make
     * package_source' command.
     */
    void pack();

    /**
     * Return true if the current process is finished.
     * @return True if finish, false otherwise.
     */
    bool isFinish() const;

    /**
     * Return true if the latest execution of a process returns true
     * of false.
     *
     * @return True if latest process success, false otherwise.
     */
    bool isSuccess() const;

    /**
     * Wait the end of the current process and flush all data from the
     * process to the std::ostream object.
     *
     * @param out Standard output stream.
     * @param err Standard error stream.
     */
    bool wait(std::ostream& out, std::ostream& err);

    /**
     * Get the current string from the pipe between the processes.
     *
     * @param [out] out To get standard output stream.
     * @param [out] err To get standard error stream.
     *
     * @return true if get is a success, false otherwise.
     */
    bool get(std::string *out, std::string *err);

    /**
     * Change the current directory to the output directory. If we
     * are in package mode, the output directory will be create. If a file
     * named output directory exists, nothing is done.
     */
    void changeToOutputDirectory();

    /**
     * Remove the specified package from the user directory.
     *
     * @param package The package to remove (recursively).
     */
    void removePackage(const std::string& package);

    /**
     * Remove the binary directory of the package:
     * - $VLE_HOME/pkgs/package/build
     * - $VLE_HOME/pkgs/package/lib
     * - $VLE_HOME/pkgs/package/plugins
     * - $VLE_HOME/pkgs/package/doc/html
     *
     * @param package The \c package to remove binary directories.
     */
    static void removePackageBinary(const std::string& package);


    /**
     * Test if the specified package exists in the user directory.
     *
     * @param package The package to test.
     *
     * @return true if the package already exist, false otherwise.
     */
    bool existsPackage(const std::string& package);

    /**
     * Add an empty file into the current Package.
     *
     * @param path The path, for example: "data" to build a new file
     * "PKG/data/name".
     * @param name The name of the file.
     */
    void addFile(const std::string& path, const std::string& name);

    /**
     * Return true if a file package/path/to/file exists in the
     * package package.
     *
     * @param path The path, for example: "data/input1.dat" to test a file
     * "PKG/data/input1.dat".
     *
     * @return true if file exists, false otherwise.
     */
    bool existsFile(const std::string& path);

    /**
     * Add an empty directgory into the current Package.
     *
     * @param path The path, for example: "data" to build a new directory
     * "PKG/data/name".
     * @param name The name of the directory.
     */
    void addDirectory(const std::string& path, const std::string& name);

    /**
     * Return true if a directory package/path/to/file exists in the
     * package package.
     *
     * @param path The path, for example: "data/input1.dat" to test a file
     * "PKG/data/input1.dat".
     *
     * @return true if directory exists, false otherwise.
     */
    bool existsDirectory(const std::string& path);

    /**
     * Remove file or directory and (recursively) the specified path.
     *
     * @param path The name of the file or the directory.
     */
    void remove(const std::string& path);

    /**
     * Rename the path oldname to the new name newname.
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
     * Copy the file.
     *
     * @param source The file source.
     * @param target The destination path.
     */
    void copy(const std::string& source, std::string& target);

    /**
     * Get the name of the current selected Package. Get the current
     * selected Package. If the name is empty, no package is selected.
     * @return The current selected Package.
     */
    const std::string& name() const;

    /**
     * Is a package selected.
     * @return Return true if a Package is selected, false otherwise.
     */
    bool selected() const;

    /**
     * Select a new Package.
     * @param name The new Package.
     */
    void select(const std::string& name);

    /**
     * Refresh the command line option from the `vle.conf' file.
     */
    void refresh();

    /**
     * Get an identifiant for the specified package. If the name is
     * not defined in the table list, it will be add.
     *
     * @param name The package to get an Id.
     * @return An id.
     */
    PackageTable::index getId(const std::string& package);

    /*   manage singleton   */

    /**
     * Return a instance of Package using the singleton system.
     * @return A reference to the singleton object.
     */
    inline static Package& package()
    { if (m_package == 0) m_package = new Package; return *m_package; }

    /**
     * Initialise the Package singleton.
     */
    inline static void init()
    { package(); }

    /**
     * Delete the Package singleton.
     */
    inline static void kill()
    { delete m_package; m_package = 0; }

private:
    Package();
    Package(const Package&);
    Package& operator=(const Package&);

    static Package* m_package; ///< singleton attribute.

    struct Pimpl;
    Pimpl *m_pimpl;
};

}} // namespace vle utils

#endif
