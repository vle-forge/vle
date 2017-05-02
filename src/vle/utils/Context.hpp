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

#ifndef VLE_UTILS_CONTEXT_HPP
#define VLE_UTILS_CONTEXT_HPP

#include <cstdarg>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>
#include <vle/DllDefines.hpp>
#include <vle/utils/Filesystem.hpp>

namespace vle {
namespace utils {

/**
 * Define a list of directories.
 */
using PathList = std::vector<vle::utils::Path>;

struct PrivateContextImpl;
class Context;
/**
 * \c ContextPtr is a shared pointer. \c ContextPtr is use in many piece of
 * code of VLE. Try to use one \c Context per thread to avoid the use of global
 * mutex.
 */
using ContextPtr = std::shared_ptr<Context>;

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
    Context(const Path& prefix = {});
    Context(std::string locale, const Path& prefix = {});
    ~Context() = default;

    Context(const Context& other) = delete;
    Context& operator=(const Context& other) = delete;
    Context(Context&& other) = delete;
    Context& operator=(Context&& other) = delete;

    /**
     * @brief Build a (partial) copy of the context:
     *  - the VLE_HOME, prefix and settings are copied
     *  - the module manager is shared with current context
     *  - the log functor is the default one (vle_log_stderr)
     *  - the locale is not modified
     *
     * @return a (partial) copy of the current context
     */
    ContextPtr clone();

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage settings
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * Assign default values to the settings database.
     * settings database.
     */
    void reset_settings() noexcept;

    /**
     * Read the configuration file (\e %VLEHOME/vle.conf) and fill the
     * settings database.
     */
    bool load_settings() noexcept;

    /**
     * Read the configuration file (\e %VLEHOME/vle.conf).
     */
    bool write_settings() const noexcept;

    /**
     * Insert an @c std::string into the specified key.
     *
     * @code
     * context->set("section.key", "vle is wonderful");
     * // section.key = "vle is wonderful"
     * @endcode
     *
     * @param key The key.
     * @param value The value.
     *
     * @return true if insertion is successful.
     */
    bool set_setting(const std::string& key,
                     const std::string& value) noexcept;

    /**
     * Insert an @c double into the specified key.
     *
     * @code
     * context->set("section.key", 1.123456789);
     * // section.key = 1.123456789
     * @endcode
     *
     * @param key The key.
     * @param value The value.
     *
     * @return true if insertion is successful.
     */
    bool set_setting(const std::string& key, double value) noexcept;

    /**
     * Insert an @c long into the specified key.
     *
     * @code
     * context->set("section.key", 23485);
     * // section.key = 23485
     * @endcode
     *
     * @param key The key.
     * @param value The value.
     *
     * @return true if insertion is successful.
     */
    bool set_setting(const std::string& key, long value) noexcept;

    /**
     * Insert an @c bool into the specified key.
     *
     * @code
     * context->set("section.key", true);
     * // section.key = true
     * @endcode
     *
     * @param key The key.
     * @param value The value.
     *
     * @return true if insertion is successful.
     */
    bool set_setting(const std::string& section, bool value) noexcept;

    /**
     * Get an @c std::string from specified key.
     *
     * @code
     * std::string value;
     * context->get("section.key", &value);
     * assert(value == "azertyuiop";
     *
     * // section.key = "azertyuiop"
     * @endcode
     *
     * @param[in] key The key.
     * @param[out] value The value to fill.
     *
     * @throw utils::ArgError if section, key do not exist or if the value
     * is not an @c std::string.
     */
    bool get_setting(const std::string& key, std::string* value) const
      noexcept;

    /**
     * Get a @c double from specified key.
     *
     * @code
     * double value;
     * context->get("section.key", &value);
     * assert(value == 1.0);
     *
     * // section1.key = 1.0
     * @endcode
     *
     * @param[in] key The key.
     * @param[out] value The value to fill.
     *
     * @throw utils::ArgError if section, key do not exist or if the value
     * is not a @c real.
     */
    bool get_setting(const std::string& key, double* value) const noexcept;

    /**
     * Get a @c long from specified key.
     *
     * @code
     * long value;
     * context->get("section.key", &value);
     * assert(value == 12345);
     *
     * // section.key = 12345
     * @endcode
     *
     * @param[in] key The key.
     * @param[out] value The value to fill.
     *
     * @throw utils::ArgError if section, key do not exist or if the value
     * is not an @c integer.
     */
    bool get_setting(const std::string& key, long* value) const noexcept;

    /**
     * Get a @c bool from specified key.
     *
     * @code
     * bool value;
     * context->get("section.key", &value);
     * assert(value);
     *
     * // section.key = true
     * @endcode
     *
     * @param[in] key The key.
     * @param[out] value The value to fill.
     *
     * @throw utils::ArgError if section, key do not exist or if the value
     * is not a @c boolean.
     */
    bool get_setting(const std::string& key, bool* value) const noexcept;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage modules
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /*
     * @brief ModuleManager permit to store a list of shared libraries or
     * modules.
     *
     * ModuleManager stores shared libraries of VLE (simulators and
     * output). For each module, an handle is stored, a pointer to the
     * associated function, a type and a version.
     *
     * ModuleManager checks the symbols:
     * - "vle_api_level": A function which returns three @c uint32_t. This
     *   integers represent the version of VLE which build the module.
     * - "vle_make_new_dynamics" (MODULE_DYNAMICS).
     * - "vle_make_new_dynamics_wrapper" (MODULE_DYNAMICS).
     * - "vle_make_new_executive" (MODULE_DYNAMICS).
     * - "vle_make_new_oov" (MODULE_OOV). *
     * @code
     * vle::utils::ModuleManager mng;
     * void* mng.get("foo", "sim", vle::utils::MODULE_DYNAMICS);
     * @endcode
     */

    /**
     * @brief A @e Module must have a type which determine the symbol to load.
     */
    enum class ModuleType
    {
        MODULE_DYNAMICS, /*|< The @e Module is a simulator and it needs to
                          * have the following symbol: @c
                          * vle_make_new_dynamics. */

        MODULE_DYNAMICS_WRAPPER, /*!< The @e Module is a simulator and it
                                   needs to have the following symbols: @c
                                   vle_make_new_dynamics_wrapper. */

        MODULE_DYNAMICS_EXECUTIVE, /*!< The @e Module is a simulator and
                                    it needs to have the following
                                    symbols: @c vle_make_new_executive. */

        MODULE_OOV, /*|< The @e Module is an output (from oov library) and
                      it needs to have the following symbol: @c
                      vle_make_new_oov. */
    };

    /**
     * @brief A @e Module describes a dynamic loaded shared library.
     *
     * @e Module is a public representation of @e ModuleManager's internal
     * Module
     * which store an handle to the shared library and a pointer to the symbol
     * referenced by the @e ModuleType.
     */
    struct Module
    {
        std::string package;
        std::string library;
        Path path;
        Context::ModuleType type;
    };

    /**
     * @brief Get a symbol from a shared library.
     *
     * Check the list of shared libraries already loaded. If the shared
     * library exists, the symbol @c type is researched into the
     * library. Otherwise, the ModuleManager try to load the shared
     * library named from the combination of @c package, @c library and @e
     * type.
     *
     * This function return is @e newtype parameter the type of the module
     * read. The @e type is MODULE_DYNAMICS, MODULE_DYNAMICS_EXECUTIVE or
     * MODULE_DYNAMICS_WRAPPER the @e newtype check the type and if it
     * does not corresponds it check the two other simulators (MODULE_DYNAMICS,
     * MODULE_DYNAMICS_EXECUTIVE or MODULE_DYNAMICS_EXECUTIVE). If @e type is
     * not MODULE_DYNAMICS, MODULE_DYNAMICS_WRAPPER,
     * MODULE_DYNAMICS_EXECUTIVE , the @e newtype will be affected by the
     * founded type.
     *
     * @code
     * // try to load the shared library
     * // $VLE_HOME/.vle/pkgs/glue/lib/libcounter.so
     * auto ctx = utils::make_context();
     * ModuleType type;
     * ctx->get_symbol("glue", "counter", MODULE_DYNAMICS, &type);
     * assert(type == MODULE_DYNAMICS || type == MODULE_DYNAMICS_WRAPPER ||
     *        type == MODULE_DYNAMICS_WRAPPER);
     *
     * ctx->get_symbol("glue", "counter", MODULE_DYNAMICS_WRAPPER, &type);
     * assert(type == MODULE_DYNAMICS || type == MODULE_DYNAMICS_WRAPPER ||
     *        type == MODULE_DYNAMICS_WRAPPER);
     *
     * ctx->get_symbol("glue", "counter", MODULE_OOV, &type);
     * assert(type == MODULE_OOV);
     * @endcode
     *
     * @param package The name of the package. If several packages with
     * the same name appear in paths provided by \c
     * Context::getBinaryPackagesDir() then the first found is returned
     * (priority to the begin).
     *
     * @param library
     * @param type
     * @param[out] newtype If @e newtype is null, newtype will not be affected.
     *
     * @throw utils::InternalError if shared library does not exists or if the
     * symbols wanted does not exist.
     *
     * @return A pointer to the founded symbol.
     */
    void* get_symbol(const std::string& package,
                     const std::string& pluginname,
                     ModuleType type,
                     ModuleType* newtype = nullptr);

    /**
     * @brief Get a symbol directly in the executable.
     *
     * Try to get a symbol from the executable if it was never
     * loaded. This function allows to build executable where we can store
     * simulators, oov's modules for instance in unit test.
     *
     * @code
     * auto ctx = vle::utils::make_context();
     * ctx->get_symbol("main");
     * @endcode
     *
     * @param symbol The name of the symbol.
     *
     * @throw utils::InternalError if the executable does not have a symbol of
     * this name.
     *
     * @return A pointer to the founded symbol.
     */
    void* get_symbol(const std::string& pluginname);

    /**
     * \e Brief Unload of loaded shared libraries.
     *
     * Unload (via \c ::dlclose or \c ::FreeLibrary) all previously loaded
     * shared libraries.
     */
    void unload_dynamic_libraries() noexcept;

    /**
     * \brief Retrieves the list of dynamic libraries of the specified
     * package and type (simulator, output).
     *
     * \note If the same package is available in multiple binary package
     * repositories, the \c std::vector is filled with the first package
     * found according to the \c getBinaryPackagesDir priority.
     *
     * \param package The name of the package to by retrieve.
     * \param type The type of module to by retrieve.
     *
     * \return All modules availables.
     */
    std::vector<Module> get_dynamic_libraries(const std::string& package,
                                              ModuleType type) const;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage log
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    struct LogFunctor
    {
        virtual void write(const Context& ctx,
                           int priority,
                           const char* file,
                           int line,
                           const char* fn,
                           const char* format,
                           va_list args) noexcept = 0;
        virtual ~LogFunctor(){};
    };

    void set_log_function(std::unique_ptr<LogFunctor> fn) noexcept;

    void set_log_priority(int priority) noexcept;

    int get_log_priority() const noexcept;

    void log(int priority,
             const char* file,
             int line,
             const char* fn,
             const char* format,
             ...)
#if defined(__GNUC__)
      const noexcept __attribute__((format(printf, 6, 7)));
#else
      const noexcept;
#endif

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage path
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * Return the prefix of the compilation on Unix or installation
     * directory taken from registry on Windows.
     * @return A string path.
     */
    const Path& getPrefixDir() const;

    /**
     * Return the VLE home directory. Default, returns $HOME/.vle on
     * Unix, %HOMEDRIVE%%HOMEPATH%/vle on Windows. If user defines a
     * VLE_HOME environment variable, this function returns the contents of
     * this variable.
     * @return A string path.
     */
    const Path& getHomeDir() const;

    /**
     * Returns the \e $VLE_HOME/vle-x.y.conf file path.
     *
     * \return A complete path to access the vle configuration file.
     */
    Path getConfigurationFile() const;

    /**
     * Return the \e $HOMEVLE/vle-x.y.log file path.
     *
     * \return A complete path to access the vle log file.
     */
    Path getLogFile() const;

    /**
     * Return the \e $HOMEVLE/prefix-x.y.log file path.
     *
     * \return A complete path to access a specific log file.
     */
    Path getLogFile(const std::string& prefix) const;

    /**
     * Get a file from the getHomeDir() directory.
     * @param name The filename to concat.
     */
    Path getHomeFile(const std::string& name) const;

    /**
     * Get the locale directory (/usr/share/locale).
     */
    Path getLocaleDir() const;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage package
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * Default returns the $HOMEVLE/.vle/pkgs $prefix/usr/lib/vle-x.y/pkgs
     * dirnames.
     *
     * @return A list of path.
     */
    std::vector<Path> getBinaryPackagesDir() const;

    /**
     * Returns the list of \e Path available into all \e
     * getBinaryPackagesDir() paths..
     *
     * @return A lisf of path.
     */
    std::vector<Path> getBinaryPackages() const;

    /**
     * Return the path @e "$VLE_HOME/local.pkg".
     *
     * @return The path @e "$VLE_HOME/local.pkg."
     */
    Path getLocalPackageFilename() const;

    /**
     * Return the path @e "$VLE_HOME/remote.pkg".
     *
     * @return The path @e "$VLE_HOME/remote.pkg."
     */
    Path getRemotePackageFilename() const;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage template
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * Get the $PREFIX/share/vle-x.y.z/template directory name.
     * @return A string.
     */
    Path getTemplateDir() const;

    /**
     * Return the $PREFIX/share/vle-x.y.z/template/name directory
     * name.
     * @path The name of the template.
     * @return A string.
     */
    Path getTemplate(const std::string& name) const;

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
     * Find the absolute path of the DESTDIR or CMAKE_INSTALL_DIR variable
     * under posix or install directory under Win32.
     * @return the absolute path to the install directory or empty Path.
     *
     * @code
     * auto ctx = vle::make_context();
     * std::cout << ctx->findInstallPrefix() << "\n";
     * // May returns "/usr/local" or "/usr" or "C:\\Program Files\\vle".
     * @endcode
     */
    Path findInstallPrefix();

    /**
     * Find the absolute path to a program.
     * @param exe the program name.
     * @return the absolute path to a program or empty Path.
     */
    Path findProgram(const std::string& exe);

    /**
     * Find the absolute path to a library.
     * @param lib the library name.
     * @return the absolute path to a library or empty Path.
     */
    Path findLibrary(const std::string& lib);

private:
    std::unique_ptr<PrivateContextImpl> m_pimpl;

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
};

/**
 * Build a std::shared_ptr<Context> without modifying the locale.
 *
 * \e param prefix Default (\e prefix.empty() or \e
 * !prefix.is_directory()), it reads the \e VLE_HOME environment variable
 * and if it fails, try to build prefix from the \e HOME user.
 *
 * \e return An initialized std::shared_ptr<Context>.
 *
 */
VLE_API ContextPtr make_context(const Path& prefix = {});

/**
 * Build a std::shared_ptr<Context> with locale.
 *
 * \e param locale Default "C" for ASCII, empty for current locale
 * otherwise, you can specify what you want.
 *
 * \e param prefix Default (\e prefix.empty() or \e
 * !prefix.is_directory()), it reads the \e VLE_HOME environment variable
 * and if it fails, try to build prefix from the \e HOME user.
 *
 * \e return An initialized std::shared_ptr<Context>.
 */
VLE_API ContextPtr make_context(std::string locale, const Path& prefix = {});
}
} // namespace vle utils

#endif
