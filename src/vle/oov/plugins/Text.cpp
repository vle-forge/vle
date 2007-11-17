/** 
 * @file Text.cpp
 * @brief 
 * @author The vle Development Team
 * @date 2007-11-17
 */

/*
 * Copyright (C) 2007 - The vle Development Team
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

#include <vle/oov/plugins/Text.hpp>

namespace vle { namespace oov { namespace plugin {

Text::Text(const std::string& location) :
    SimpleFile(location)
{
}

Text::~Text()
{
}

std::string Text::extension() const
{
    return ".dat";
}

void Text::writeSeparator(std::ostream& out)
{
    out << '\t';
}

void Text::writeHead(std::ostream& out,
                     const std::vector < std::string >& heads)
{
    out << '#';
    for (std::vector < std::string >::const_iterator it = heads.begin();
         it != heads.end(); ++it) {
        out << *it << '\t';
    }
    out << '\n';
}

}}} // namespace vle oov plugin
