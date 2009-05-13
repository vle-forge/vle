/**
 * @file vle/gvle/UndoRedo.cpp
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


#include <vle/gvle/UndoRedo.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/graph/Model.hpp>

using namespace vle;

namespace vle
{
namespace gvle {

UndoRedo::UndoRedo(int nbUndo)
{
    setNbUndo(nbUndo);
}

UndoRedo::~UndoRedo()
{
    std::list < graph::Model* >::iterator it = mList.begin();
    while (it != mList.end()) {
        delete(*it);
        ++it;
    }
}

void UndoRedo::addModel(const graph::Model* /* m */)
{
    throw utils::NotYetImplemented("UndoRedo::addModel");
}

graph::Model* UndoRedo::getModel()
{
    graph::Model* m = NULL;

    if (!mList.empty()) {
        m = *(mList.end()--);
        mList.pop_back();
    }

    return m;
}

void UndoRedo::setNbUndo(int nbUndo)
{
    if (nbUndo < 0)
        gvle::Error(_("Number undo defined is smaller than 0 ?."));
    else
        mNbUndo = nbUndo;
}

}
} // namespace vle gvle
