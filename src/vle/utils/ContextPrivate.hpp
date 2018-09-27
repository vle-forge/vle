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
}
} // namespace vle utils

#endif
