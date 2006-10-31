/** 
 * @file Pyutils.hpp
 * @brief 
 * @author The vle Development Team
 * @date dim, 29 oct 2006 11:40:48 +0100
 */

/*
 * Copyright (C) 2004, 2005, 2006 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef VLE_SCRIPTS_PYTHON_PYUTILS_HPP
#define VLE_SCRIPTS_PYTHON_PYUTILS_HPP

#include <Python.h>
#include <string>

namespace vle { namespace scripts { namespace python {

    class Pyutils
    {
    public:
        static void start_interpreter();
        static void stop_interpreter();
        static void pyassert(PyObject *obj, const std::string& msg);
        
    private:
        static int instance_number;
        static void set_path();
    };

}}} // namespace vle script python

#endif

