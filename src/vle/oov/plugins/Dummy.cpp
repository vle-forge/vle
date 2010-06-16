/**
 * @file vle/oov/plugins/Dummy.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/oov/plugins/Dummy.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Path.hpp>


namespace vle { namespace oov { namespace plugin {

Dummy::Dummy(const std::string& location) :
    Plugin(location)
{
}

Dummy::~Dummy()
{
}

void Dummy::onParameter(const std::string& /*plugin*/,
                         const std::string& /*location*/,
                         const std::string& /*file*/,
                         value::Value* /*parameters*/,
                         const double& /*time*/)
{
}

void Dummy::onNewObservable(const std::string& /*simulator*/,
                             const std::string& /*parent*/,
                             const std::string& /*port*/,
                             const std::string& /*view*/,
                             const double& /*time*/)
{
}

void Dummy::onDelObservable(const std::string& /*simulator*/,
                             const std::string& /*parent*/,
                             const std::string& /*port*/,
                             const std::string& /*view*/,
                             const double& /*time*/)
{
}

void Dummy::onValue(const std::string& /*simulator*/,
                     const std::string& /*parent*/,
                     const std::string& /*port*/,
                     const std::string& /*view*/,
                     const double& /*time*/,
                     value::Value* /*value*/)
{
}

void Dummy::close(const double& /*time*/)
{
}

}}} // namespace vle oov plugin
