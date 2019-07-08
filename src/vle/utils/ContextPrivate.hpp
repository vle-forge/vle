/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
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

#ifndef VLE_UTILS_CONTEXT_PRIVATE_HPP
#define VLE_UTILS_CONTEXT_PRIVATE_HPP

#include <boost/variant.hpp>
#include <map>
#include <vle/DllDefines.hpp>
#include <vle/utils/Context.hpp>

#define VLE_LOG_EMERG 0   // system is unusable
#define VLE_LOG_ALERT 1   // action must be taken immediately
#define VLE_LOG_CRIT 2    // critical conditions
#define VLE_LOG_ERR 3     // error conditions
#define VLE_LOG_WARNING 4 // warning conditions
#define VLE_LOG_NOTICE 5  // normal but significant condition
#define VLE_LOG_INFO 6    // informational
#define VLE_LOG_DEBUG 7   // debug-level messages

namespace vle {
namespace utils {

//
// API for settings
//

using PreferenceType = boost::variant<bool, std::string, long, double>;
using PreferenceMap = std::map<std::string, PreferenceType>;

struct ModuleManager;

struct PrivateContextImpl
{
    Path m_prefix; ///< dirname of $PREFIX of installation
    Path m_home;   ///< dirname of $VLEHOME directory

    PreferenceMap settings; ///< global settings

    std::shared_ptr<ModuleManager> modules;

    std::unique_ptr<Context::LogFunctor> log_fn;
    int log_priority;
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
void*
get_symbol(vle::utils::ContextPtr ctx,
           const std::string& package,
           const std::string& pluginname,
           Context::ModuleType type,
           Context::ModuleType* newtype = nullptr);

/**
 * @brief Get a factory.
 *
 * @code
 * auto ctx = vle::utils::make_context();
 * ctx->add_oov_factory("test",
 *                      [](const std::string& loc)
 *                      {
 *                          return new my_plugin(loc);
 *                      }));
 *
 * auto *fct = get_oov_factory(ctx, "test");
 * @endcode
 *
 * @param symbol The name of the factory.
 *
 * @return A pointer to the founded factory, nullptr otherwise.
 */
oov_factory_fct&
get_oov_factory(vle::utils::ContextPtr ctx, std::string pluginname);

/**
 * @brief Get a factory.
 *
 * @code
 * auto ctx = vle::utils::make_context();
 * ctx->get_dynamics_factory("test",
 *                      [](const std::string& loc)
 *                      {
 *                          return new my_plugin(loc);
 *                      }));
 *
 * auto *fct = get_dynamics_factory(ctx, "test");
 * @endcode
 *
 * @param symbol The name of the factory.
 *
 * @return A pointer to the founded factory, nullptr otherwise.
 */
dynamics_factory_fct&
get_dynamics_factory(vle::utils::ContextPtr ctx, std::string name);

/**
 * @brief Get a factory.
 *
 * @code
 * auto ctx = vle::utils::make_context();
 * ctx->get_executive_factory("test",
 *                      [](const std::string& loc)
 *                      {
 *                          return new my_plugin(loc);
 *                      }));
 *
 * auto *fct = get_executive_factory(ctx, "test");
 * @endcode
 *
 * @param symbol The name of the factory.
 *
 * @return A pointer to the founded factory, nullptr otherwise.
 */
executive_factory_fct&
get_executive_factory(vle::utils::ContextPtr ctx, std::string name);

boost::variant<oov_factory_fct, dynamics_factory_fct, executive_factory_fct>&
get_factory(vle::utils::ContextPtr ctx, std::string name);

} // namespace vle
} // namespace utils

#endif
