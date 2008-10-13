/**
 * @file vle/oov/plugins/Rdata.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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


#include <vle/oov/plugins/Rdata.hpp>

namespace vle { namespace oov { namespace plugin {

Rdata::Rdata(const std::string& location) :
    SimpleFile(location)
{
}

Rdata::~Rdata()
{
}

std::string Rdata::extension() const
{
    return ".rdata";
}

void Rdata::writeSeparator(std::ostream& out)
{
    out << '\t';
}

void Rdata::writeHead(std::ostream& out,
                      const std::vector < std::string >& heads)
{
    for (std::vector < std::string >::const_iterator it = heads.begin();
         it != heads.end(); ++it) {
        out << "\"" << *it << "\"\t";
    }
    out << '\n';
}

}}} // namespace vle oov plugin
