/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#include <string>
#include <vle/DllDefines.hpp>
#include <vle/utils/Context.hpp>
#include <vle/utils/PackageTable.hpp>

namespace vle {
namespace utils {

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

enum VLE_PACKAGE_TYPE
{
    PKG_BINARY,
    PKG_SOURCE
};

VLE_API std::ostream& operator<<(std::ostream& out,
                                 const VLE_PACKAGE_TYPE& type);

class VLE_API Package
{
public:
    Package(ContextPtr context);
    Package(ContextPtr context, const std::string& pkgname);

    Package(const Package&) = delete;
    Package& operator=(const Package&) = delete;
    Package(const Package&&) = delete;
    Package& operator=(const Package&&) = delete;

    ~Package();

    const std::string& name() const;

    /**
     * Selects the package
     */
    void select(const std::string& name);

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
     * Clean the package by running removing the "buildvle"
     * directory of the source package
     */
    void clean();

    /**
     * Remove the binary package
     */
    void rclean();

    /**
     * Pack the package by running the 'make package' and 'make
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
    bool get(std::string* out, std::string* err);

    /**
     * Test if the specified package exists in the user directory.
     *
     * @param package The package to test.
     *
     * @return true if the package already exist, false otherwise.
     */
    bool existsSource() const;

    /**
     * Test if the specified package exists as a binary package.
     *
     * @param package The package to test.
     *
     * @return true if the package already exist, false otherwise.
     */
    bool existsBinary() const;

    std::string getParentDir(VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getDir(VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getLibDir(VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getSrcDir(VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getDataDir(VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getDocDir(VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getExpDir(VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getBuildDir(VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getOutputDir(VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getPluginDir(VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getPluginSimulatorDir(
      VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getPluginOutputDir(VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getPluginGvleGlobalDir(
      VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getPluginGvleModelingDir(
      VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getPluginGvleOutputDir(
      VLE_PACKAGE_TYPE type = PKG_BINARY) const;

    std::string getFile(const std::string& file,
                        VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getLibFile(const std::string& file,
                           VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getSrcFile(const std::string& file,
                           VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getDataFile(const std::string& file,
                            VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getDocFile(const std::string& file,
                           VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getExpFile(const std::string& file,
                           VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getOutputFile(const std::string& file,
                              VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getPluginFile(const std::string& file,
                              VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getPluginSimulatorFile(
      const std::string& file,
      VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getPluginOutputFile(const std::string& file,
                                    VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getPluginGvleModelingFile(
      const std::string& file,
      VLE_PACKAGE_TYPE type = PKG_BINARY) const;
    std::string getPluginGvleOutputFile(
      const std::string& file,
      VLE_PACKAGE_TYPE type = PKG_BINARY) const;

    PathList getExperiments(VLE_PACKAGE_TYPE type = PKG_BINARY) const;

    PathList getPluginsSimulator() const;
    PathList getPluginsOutput() const;
    PathList getPluginsGvleGlobal() const;
    PathList getPluginsGvleModeling() const;
    PathList getPluginsGvleOutput() const;

    std::string getMetadataExpDir(VLE_PACKAGE_TYPE type = PKG_BINARY) const;

    std::string getMetadataExpFile(const std::string& expName,
                                   VLE_PACKAGE_TYPE type = PKG_BINARY) const;

    /**
     * Return true if a file package/path/to/file exists in the
     * package package.
     *
     *
     * @param path The path, for example: "data/input1.dat" to test a file
     * "PKG/data/input1.dat".
     * @param type The type of package
     *
     * @return true if file exists, false otherwise.
     */
    bool existsFile(const std::string& path,
                    VLE_PACKAGE_TYPE type = PKG_BINARY);

    /**
     * Add an empty directgory into the current Package.
     *
     * @param path The path, for example: "data" to build a new directory
     * "PKG/data/name".
     * @param name The name of the directory.
     * @param type The type of package
     */
    void addDirectory(const std::string& path,
                      const std::string& name,
                      VLE_PACKAGE_TYPE type = PKG_BINARY);

    /**
     * Return true if a directory package/path/to/file exists in the
     * package package.
     *
     * @param type The type of package
     * @param path The path, for example: "data/input1.dat" to test a file
     * "PKG/data/input1.dat".
     *
     * @return true if directory exists, false otherwise.
     */
    bool existsDirectory(const std::string& path,
                         VLE_PACKAGE_TYPE type = PKG_BINARY);

    /**
     * Remove file or directory and (recursively) the specified path.
     *
     * @param toremove The name of the file or the directory.
     * @param type The type of package
     */
    void remove(const std::string& toremove,
                VLE_PACKAGE_TYPE type = PKG_BINARY);

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
     * @param type The type of package
     *
     * @return The string representation of the path of the new file.
     *
     * @throw utils::ArgError if the file already exists or if the old file
     * does not exist.
     */
    std::string rename(const std::string& oldname,
                       const std::string& newname,
                       VLE_PACKAGE_TYPE type = PKG_BINARY);

    /**
     * Copy the file.
     *
     * @param source The file source.
     * @param target The destination path.
     */
    void copy(const std::string& source, std::string& target);

    /**
     * Refresh the command line option from the `vle.conf' file.
     */
    void refreshCommands();

    /**
     * Refresh the command line option from the `vle.conf' file.
     */
    void refreshPath();

    /**
     * @brief fills a vector of string with the binary content
     * @param pkgcontent, the vector of string to fill
     */
    void fillBinaryContent(std::vector<std::string>& pkgcontent);

    friend std::ostream& operator<<(std::ostream& out, const Package& pkg);

private:
    PathList listLibraries(const std::string& path) const;

    struct Pimpl;
    std::unique_ptr<Pimpl> m_pimpl;
};

VLE_API std::ostream& operator<<(std::ostream& out, const Package& pkg);
}
} // namespace vle utils

#endif
