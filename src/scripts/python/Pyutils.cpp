/** 
 * @file Pyutils.cpp
 * @brief 
 * @author The vle Development Team
 * @date dim, 29 oct 2006 11:36:56 +0100
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

#include <scripts/python/Pyutils.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Exception.hpp>
#include <string>

namespace vle { namespace scripts { namespace python {

int Pyutils::instance_number = 0;

void Pyutils::start_interpreter()
{
    if(instance_number == 0) {
        Py_Initialize();
        set_path();
    }
    instance_number++;
}

void Pyutils::stop_interpreter()
{
    instance_number--;
    if(instance_number == 0) {
        Py_Finalize();
    }
}

void Pyutils::set_path()
{
    const std::string& vle_path = vle::utils::Path::path().getDefaultPythonDir();
    const std::string& local_path = vle::utils::Path::path().getUserPythonDir();

    std::string total_path(
        std::string(Py_GetPath()) + ':' + vle_path + ':' + local_path + ":.");

    PySys_SetPath((char *)total_path.c_str());
}

void Pyutils::pyassert(PyObject *obj, const std::string& msg)
{
    if(obj == NULL) {
        if (PyErr_Occurred() != NULL)
            PyErr_Print();
        throw utils::InternalError(msg);
    } 
}

}}} // namespace vle script python
