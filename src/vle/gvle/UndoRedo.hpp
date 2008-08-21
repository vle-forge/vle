/**
 * @file vle/gvle/UndoRedo.hpp
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


#ifndef GUI_UNDOREDO_HPP
#define GUI_UNDOREDO_HPP

#include <list>

namespace vle
{
namespace graph {

class Model;

}
} // namespace vle graph

namespace vle
{
namespace gvle {

/**
 * @brief A class use to store all modification from gui::graph::Model
 * hierarchy.  This class store all modified hierarchy into a list of
 * modification.
 */
class UndoRedo
{
public:
    /**
     * Create an UndoRedo object with nbUndo for number UndoRedo.
     *
     * @param nbUndo number UndoRedo
     */
    UndoRedo(int nbUndo = 10);

    /**
     * Delete all cloned saved list
     *
     */
    ~UndoRedo();

    /**
     * Add a clone of graph::Model tree models m to the UndoRedo object.
     *
     * @param m graph::Model to clone and archive
     */
    void addModel(const graph::Model* m);

    /**
     * Get the last added graph::Model if exist
     *
     * @return A graph::Model clone
     */
    graph::Model* getModel();

    /**
     * set the number of undo redo
     *
     * @param nbUndo number of undo redo
     */
    void setNbUndo(int nbUndo);

    /**
     * return number of undo redo stored
     *
     * @return number of undo redo stored
     */
    int getNbUndo() const {
        return mNbUndo;
    }

private:
    std::list < graph::Model* > mList;
    int mNbUndo;
};

}
} // namespace vle gvle

#endif
