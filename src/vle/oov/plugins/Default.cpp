/** 
 * @file Default.cpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 24 jui 2007 18:22:07 +0200
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

#include <vle/oov/plugins/Default.hpp>


namespace vle { namespace oov { namespace plugin {

Default::Default(const std::string& location) :
    Plugin(location)
{
    m_file.open(location.c_str());
    if (not m_file.is_open()) {
        Throw(utils::InternalError, (boost::format(
                    "Cannot open file '%1%'") % location));
    }
}

Default::~Default()
{
}

void Default::onParameter(const vpz::ParameterTrame& trame)
{
    m_file << trame << "\n";
}

void Default::onNewObservable(const vpz::NewObservableTrame& trame)
{
    m_file << trame << "\n";
}

void Default::onDelObservable(const vpz::DelObservableTrame& trame)
{
    m_file << trame << "\n";
}

void Default::onValue(const vpz::ValueTrame& trame)
{
    m_file << trame << "\n";
}

void Default::close(const vpz::EndTrame& trame)
{
    m_file << trame << "\n";
    m_file.flush();
}

}}} // namespace vle oov plugin
