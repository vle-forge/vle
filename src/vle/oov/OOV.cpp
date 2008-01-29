/** 
 * @file OOV.cpp
 * @brief 
 * @author The vle Development Team
 * @date 2007-08-23
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

#include <vle/oov/OOV.hpp>


namespace vle { namespace oov {

void OOV::printInformations(std::ostream& out)
{
    out <<
        "Output of Virtual Laboratory Environment (oov) - "
        VLE_PACKAGE_VERSION
        "\n"
        "Copyright (C) 2004, 05, 06, 07 VLE Development Team.\n"
        "VLE comes with ABSOLUTELY NO WARRANTY.\n"
        "You may redistribute copies of VLE\n"
        "under the terms of the GNU General Public License.\n"
        "For more information about these matters, see the file named COPYING."
        << std::endl;
}

void OOV::printVersion(std::ostream& out)
{
    out << VLE_PACKAGE_VERSION << std::endl;
}

}} // vle oov
