/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef GUI_CUTCOPYPAST_HPP
#define GUI_CUTCOPYPAST_HPP

#include <vle/vpz/CoupledModel.hpp>
#include <string>

namespace vle
{
namespace gvle {

class GVLE;

/**
 * A class to manage cut/copy/paste action on GVLE views.
 *
 */
class CutCopyPaste
{
public:
    /** define type of paste : cut or copy */
    enum Type { CUT, COPY, NOTHING };

    /** create a cut copy paste object with no type */
    CutCopyPaste(GVLE* gvle);

    /** delete list of GModel if present */
    ~CutCopyPaste();

    /**
    * detach the list of GModel of vpz::CoupledModel parent.
    *
    * @param l list of GModel to detach.
    * @param gc vpz::CoupledModel where list will be cut.
    */
    void cut(vpz::ModelList & l, vpz::CoupledModel* gc);

    /**
     * clone the vpz::ModelList if has no connection with external
     * model.
     *
     * @param l list of GModel to copy.
     * @param gc vpz::CoupledModel where list will be copied.
     */
    void copy(vpz::ModelList& l, vpz::CoupledModel* gc,
              bool isClass);

    /**
     * paste the current list GModel into vpz::CoupledModel gc. Rename
     * models to eliminate duplicated name.
     *
     * @param gc vpz::CoupledModel parent to attach list of model.
     * passed with function copy or cut.
     */
    void paste(vpz::CoupledModel* gc);

    bool paste_is_empty();

private:
    /** delete list of GModel if present */
    void clear();

    /**
     * clone the internal vpz::ModelList mList. If num == -1 then
     * list is purely clone (parent is set to 0 and position move 5px).
     * Else, name is modified to add num
     *
     * @param lst_clone clone result
     * @param num number of clonage
     */
    void clone(vpz::ModelList& lst_graph_clone, int num);

    void clone_atomic(vpz::BaseModel* clone);
    void clone_coupled(vpz::BaseModel* model, vpz::BaseModel* clone);

    GVLE*                       mGVLE;

    vpz::ModelList             mList_graph;
    int                        mNumero;
    Type                       mType;

    std::vector <std::string> mCnts;

};

}
} // namespace vle gvle

#endif
