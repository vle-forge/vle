/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
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

#include <vle/DllDefines.hpp>
#include <vle/utils/Context.hpp>

#define VLE_LOG_EMERG   0               // system is unusable
#define VLE_LOG_ALERT   1               // action must be taken immediately
#define VLE_LOG_CRIT    2               // critical conditions
#define VLE_LOG_ERR     3               // error conditions
#define VLE_LOG_WARNING 4               // warning conditions
#define VLE_LOG_NOTICE  5               // normal but significant condition
#define VLE_LOG_INFO    6               // informational
#define VLE_LOG_DEBUG   7               // debug-level messages

static inline void
#if defined(__GNUC__)
 __attribute__((always_inline, format(printf, 2, 3)))
#endif
vle_log_null(const vle::utils::ContextPtr&, const char *, ...)
{
}

#define vle_log_cond(ctx, prio, arg...)                                \
    do {                                                               \
        if (ctx->get_log_priority() >= prio) {                         \
            ctx->log(prio, __FILE__, __LINE__,                         \
                     __FUNCTION__, ## arg);                            \
        }                                                              \
    } while (0)

/* Default, logging system is active and the \e dbg() macro checks log
 * priority argument.
 * Define DISABLE_LOGGING to hide all logging message.
 * Define NDEBUG to remove dbg() macro.
 */

#ifndef DISABLE_LOGGING
#  ifndef NDEBUG
#    define vDbg(ctx, arg...) vle_log_cond(ctx, VLE_LOG_DEBUG, ## arg)
#  else
#    define vDbg(ctx, arg...) vle_log_null(ctx, ## arg)
#  endif
#  define vInfo(ctx, arg...) vle_log_cond(ctx, VLE_LOG_INFO, ## arg)
#  define vErr(ctx, arg...) vle_log_cond(ctx, VLE_LOG_ERR, ## arg)
#else
#  define vDbg(ctx, arg...) vle_log_null(ctx, ## arg)
#  define vInfo(ctx, arg...) vle_log_null(ctx, ## arg)
#  define vErr(ctx, arg...) vle_log_null(ctx, ## arg)
#endif

namespace vle { namespace utils {

struct PrivateContextIimpl
{
    Path m_prefix; /*!< the $prefix of installation */
    Path m_home; /*!< the $VLE_HOME */

    PathList m_simulator;
    PathList m_stream;
    PathList m_output;
    PathList m_modeling;

    Context::LogFn log_fn;
    int log_priority;
};

}} // namespace vle utils

#endif
