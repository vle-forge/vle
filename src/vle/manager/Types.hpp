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

#ifndef VLE_MANAGER_TYPES_HPP
#define VLE_MANAGER_TYPES_HPP

#include <string>

namespace vle {
namespace manager {

/**
 * The @c vle::manager::Error structure permits to report error.
 *
 * The @c vle::manager::Error structure is used by the
 * @c vle::manager::Simulation and the @c vle::manager::Manager
 * classes to report error during the run of the simulation or the
 * experimental frame.
 *
 * The @c vle::manager::Error is a structure with an error code (an
 * integer different to 0 to indicate error) and string to store the
 * message throws by the simulation or the experimental frames.
 */
struct Error
{
    Error()
      : code(0)
    {
    }

    Error(int code, const std::string& message)
      : code(code)
      , message(message)
    {
    }

    Error(const Error& error)
      : code(error.code)
      , message(error.message)
    {
    }

    Error& operator=(const Error& other)
    {
        Error tmp(other.code, other.message);

        std::swap(code, tmp.code);
        std::swap(message, tmp.message);

        return *this;
    }

    ~Error()
    {
    }

    int code;
    std::string message;
};

/**
 * Defines the type of log
 *
 */
enum LogOptions
{
    LOG_NONE = 0,         /**< Report nothing in output stream. */
    LOG_SUMMARY = 1 << 0, /**< Report summary of the experimental
                              * frame (size of the plan, number of
                              * replicas etc.). */
    LOG_RUN = 1 << 1      /**< Report information. */
};

/**
 * Defines the option to launch simulation.
 *
 */
enum SimulationOptions
{
    SIMULATION_NONE = 0,               /**< Default option. */
    SIMULATION_SPAWN_PROCESS = 1 << 0, /**< Launch the simulation in a
                                           * subprocess.  */
    SIMULATION_NO_RETURN = 1 << 1      /**< The simulation result are empty. */
};

inline LogOptions
operator|(LogOptions lhs, LogOptions rhs)
{
    return static_cast<LogOptions>(static_cast<unsigned>(lhs) |
                                   static_cast<unsigned>(rhs));
}

inline LogOptions operator&(LogOptions lhs, LogOptions rhs)
{
    return static_cast<LogOptions>(static_cast<unsigned>(lhs) &
                                   static_cast<unsigned>(rhs));
}

inline LogOptions
operator^(LogOptions lhs, LogOptions rhs)
{
    return static_cast<LogOptions>(static_cast<unsigned>(lhs) ^
                                   static_cast<unsigned>(rhs));
}

inline LogOptions operator~(LogOptions flags)
{
    return static_cast<LogOptions>(~static_cast<unsigned>(flags));
}

inline LogOptions&
operator|=(LogOptions& lhs, LogOptions rhs)
{
    return (lhs = static_cast<LogOptions>(static_cast<unsigned>(lhs) |
                                          static_cast<unsigned>(rhs)));
}

inline LogOptions&
operator&=(LogOptions& lhs, LogOptions rhs)
{
    return (lhs = static_cast<LogOptions>(static_cast<unsigned>(lhs) &
                                          static_cast<unsigned>(rhs)));
}

inline LogOptions&
operator^=(LogOptions& lhs, LogOptions rhs)
{
    return (lhs = static_cast<LogOptions>(static_cast<unsigned>(lhs) ^
                                          static_cast<unsigned>(rhs)));
}

inline SimulationOptions
operator|(SimulationOptions lhs, SimulationOptions rhs)
{
    return static_cast<SimulationOptions>(static_cast<unsigned>(lhs) |
                                          static_cast<unsigned>(rhs));
}

inline SimulationOptions operator&(SimulationOptions lhs,
                                   SimulationOptions rhs)
{
    return static_cast<SimulationOptions>(static_cast<unsigned>(lhs) &
                                          static_cast<unsigned>(rhs));
}

inline SimulationOptions
operator^(SimulationOptions lhs, SimulationOptions rhs)
{
    return static_cast<SimulationOptions>(static_cast<unsigned>(lhs) ^
                                          static_cast<unsigned>(rhs));
}

inline SimulationOptions operator~(SimulationOptions flags)
{
    return static_cast<SimulationOptions>(~static_cast<unsigned>(flags));
}

inline SimulationOptions&
operator|=(SimulationOptions& lhs, SimulationOptions rhs)
{
    return (lhs = static_cast<SimulationOptions>(static_cast<unsigned>(lhs) |
                                                 static_cast<unsigned>(rhs)));
}

inline SimulationOptions&
operator&=(SimulationOptions& lhs, SimulationOptions rhs)
{
    return (lhs = static_cast<SimulationOptions>(static_cast<unsigned>(lhs) &
                                                 static_cast<unsigned>(rhs)));
}

inline SimulationOptions&
operator^=(SimulationOptions& lhs, SimulationOptions rhs)
{
    return (lhs = static_cast<SimulationOptions>(static_cast<unsigned>(lhs) ^
                                                 static_cast<unsigned>(rhs)));
}
}
} // namespace vle manager

#endif
