/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
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


#ifndef VLE_MAIN_INIT_HPP
#define VLE_MAIN_INIT_HPP

#include <vle/DllDefines.hpp>

namespace vle {

/**
 * Initialize the VLE subsystems:
 *
 * - libxml2 library
 * - Glib::thread
 * - VLE's singleton
 *
 * @example
 * int main(int argc, char **argv)
 * {
 *     vle::Init app;
 *
 *     [...]
 *
 * }
 * @endexample
 */
struct VLE_API Init
{
    /**
     * Initialize all subsystems. Use the default locale according to the
     * environment variables.
     *
     * @code
     * vle::Init app; // to use the default locale according to the environment
     *                // variables.
     * environment variables.
     * @endcode
     */
    Init();

    /**
     * Initialize all subsystems. Use the @e localname as local for
     * the current application.
     *
     * @code
     * vle::Init app("POSIX"); // to use the POSIX version.
     * vle::Init app("C"); // to use the POSIX version.
     * vle::Init app("fr_FR"); // to switch to the French locale.
     * vle::Init app(""); // to use the default locale according to the
     *                    // environment variables.
     * @endcode
     */
    Init(const char *localname);

    /**
     * Cleanup all subsystems.
     */
    ~Init();
};

}

#endif
