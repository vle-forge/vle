/** 
 * @file FinishView.cpp
 * @brief 
 * @author The vle Development Team
 * @date 2008-01-11
 */

/*
 * Copyright (C) 2008 - The vle Development Team
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

#include <vle/devs/FinishView.hpp>
#include <vle/devs/StreamWriter.hpp>



namespace vle { namespace devs {

FinishView::FinishView(const std::string& name, StreamWriter* stream) :
    View(name, stream)
{
}

StateEvent* FinishView::processStateEvent(StateEvent* event)
{
    value::Value val = event->getAttributeValue(event->getPortName());

    if (val.get()) {
        m_stream->process(*event);
    }
    return 0;
}

}} // namespace vle devs
