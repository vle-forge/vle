/*
 * @file vle/gvle/CutCopyPaste.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#include <vle/gvle/CutCopyPaste.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/ViewDrawingArea.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Debug.hpp>
#include <iostream>

namespace vle { namespace gvle {

CutCopyPaste::CutCopyPaste(GVLE* gvle) :
    mGVLE(gvle),
    mNumero(0),
    mType(NOTHING)
{ }

CutCopyPaste::~CutCopyPaste()
{
    try {
        clear();
    } catch (utils::NotYetImplemented) {
    }
}

/*********************************************************************
 *
 * FUNCTION OF CUT COPY AND PASTE
 *
 *********************************************************************/

void CutCopyPaste::cut(graph::ModelList& l, graph::CoupledModel* parent,
                       vpz::AtomicModelList& src)
{
    if (parent->hasConnectionProblem(l)) {
        mGVLE->showMessage(_("Selected model list have connection with " \
                             "external model"));
    } else if (l.empty()) {
        mGVLE->showMessage(_("No model selected"));
    } else if (l.find(parent->getName()) == l.end()) {
        clear();
        mType = CUT;

        // to keep connections
        mCnts = parent->getBasicConnections(l);

        parent->detachModels(l);
        mList_graph = l;

        for (graph::ModelList::const_iterator it = mList_graph.begin();
             it !=  mList_graph.end(); ++it) {
            if (it->second->isAtomic()) {
                cut_atomic(it->second, src);
            } else {
                cut_coupled(it->second, src);
            }
        }
    } else {
        mGVLE->showMessage(_("Nothing to Cut - parent is ") + parent->getName());
    }
}

void CutCopyPaste::copy(graph::ModelList& l, graph::CoupledModel* parent,
                        vpz::AtomicModelList& src, bool isClass)
{
    if (not isClass and parent->hasConnectionProblem(l)) {
        mGVLE->showMessage(_("Selected model list have connection with " \
                             "external model"));
    } else if (l.empty()) {
        mGVLE->showMessage(_("No model selected\n"));
    } else {
        clear();
        mType = COPY;

        // to keep connections
	if (not isClass and parent != NULL) {
	    mCnts = parent->getBasicConnections(l);
	}

        for (graph::ModelList::iterator it = l.begin();
             it != l.end(); ++it) {
            graph::Model* m = it->second->clone();

            m->setParent(NULL);
            mList_graph.insert(make_pair(m->getName(), m));
            if (m->isAtomic()) {
                copy_atomic(it->second, m, src);
            } else {
                copy_coupled(it->second, m, src);
            }
        }
    }
}

void CutCopyPaste::paste(graph::CoupledModel* gc, vpz::AtomicModelList& dst)
{
    graph::ModelList list_graph_clone;
    vpz::AtomicModelList list_vpz_clone;

    if (gc) {
        if (mType == CUT) {
            clone(list_graph_clone,
                  list_vpz_clone,
                  (mNumero == 0) ? -1 : mNumero - 1);
            ++mNumero;
        } else if (mType == COPY) {
            clone(list_graph_clone,
                  list_vpz_clone,
                  mNumero);
            ++mNumero;
        }

        // to keep initial connections
        std::vector <std::string> mwCnts= mCnts;

        // to avoid naming conflicts.
        for (graph::ModelList::const_iterator itmls = list_graph_clone.begin();
             itmls != list_graph_clone.end(); ++itmls) {
            std::string mname = itmls->second->getName();

            if (gc->exist(mname)) {
                int mcounter = 1;
                std::string mnewname = mname + "_" +
                    boost::lexical_cast<std::string>(mcounter) ;

                while (gc->exist(mnewname)) {
                    mnewname = mname + "_" +
                        boost::lexical_cast<std::string>(mcounter++) ;
                }

                graph::Model::rename(itmls->second, mnewname);

                //to update connections
                std::vector <std::string>::iterator itc = mwCnts.begin();

                for (std::vector <std::string>::iterator itco = mCnts.begin();
                     itco != mCnts.end(); itco = itco + 2) {
                    if (*itco == mname) {
                        *itc = mnewname;
                    }
                    itc = itc + 2;
                }
            }
            gc->attachModel(itmls->second);
        }
        gc->setBasicConnections(mwCnts);
        dst.add(list_vpz_clone);
    } else {
        mGVLE->showMessage(_("No Coupled Model"));
    }
}

bool CutCopyPaste::paste_is_empty() { 
    return mList_graph.empty();
}

/*********************************************************************
 *
 * PRIVATE FUNCTION
 *
 *********************************************************************/

void CutCopyPaste::clear()
{
    for (graph::ModelList::iterator it = mList_graph.begin();
            it != mList_graph.end();
            ++it) {
        delete it->second;
    }
    mList_graph.clear();
    mList_vpz.clear();

    mNumero = 0;
    mType = NOTHING;
}

void CutCopyPaste::clone(graph::ModelList& lst_graph_clone,
                         vpz::AtomicModelList& lst_vpz_clone, int num)
{
    lst_graph_clone.clear();
    lst_vpz_clone.clear();

    if (num == -1) {
        for (graph::ModelList::iterator it = mList_graph.begin();
                it != mList_graph.end();
                ++it) {
            graph::Model* m = it->second->clone();

            lst_graph_clone.insert(make_pair(m->getName(), m));
            if (m->isCoupled()) {
                clone_coupled(it->second, m, lst_vpz_clone);
            } else {
                clone_atomic(it->second, m, lst_vpz_clone);
            }
        }

    } else {
        ++num;
        for (graph::ModelList::iterator it = mList_graph.begin(); it !=
                mList_graph.end();
                ++it) {
            graph::Model* m = it->second->clone();
            m->setParent(NULL);
            m->setPosition(m->x() + (num * 10), m->y() + (num * 10));

            lst_graph_clone.insert(make_pair(m->getName(), m));
            if (m->isCoupled()) {
                clone_coupled(it->second, m, lst_vpz_clone);
            } else {
                clone_atomic(it->second, m, lst_vpz_clone);
            }
        }
    }
}

void CutCopyPaste::cut_atomic(graph::Model* model, vpz::AtomicModelList& src)
{
    const vpz::AtomicModel& atom(src.get(model));
    mList_vpz.add(model, atom);
    src.del(model);
}

void CutCopyPaste::cut_coupled(graph::Model* model, vpz::AtomicModelList& src)
{
    graph::ModelList& list = dynamic_cast<graph::CoupledModel*>(model)->getModelList();
    graph::ModelList::iterator it = list.begin();
    while (it != list.end()) {
        if (it->second->isAtomic()) {
            cut_atomic(it->second, src);
        } else {
            cut_coupled(it->second, src);
        }

        ++it;
    }
}

void CutCopyPaste::copy_atomic(graph::Model* model, graph::Model* clone,
                               vpz::AtomicModelList& src)
{
    const vpz::AtomicModel& atom(src.get(model));
    mList_vpz.add(clone, atom);
    if (clone->width() < 0) {
	clone->setWidth(ViewDrawingArea::MODEL_WIDTH);
    }
    if (clone->height() < 0) {
	clone->setHeight(ViewDrawingArea::MODEL_HEIGHT +
			 std::max(clone->getInputPortNumber(),
				  clone->getOutputPortNumber()) *
			 (ViewDrawingArea::MODEL_SPACING_PORT +
			  ViewDrawingArea::MODEL_PORT));
    }
}

void CutCopyPaste::copy_coupled(graph::Model* model, graph::Model* clone,
                                vpz::AtomicModelList& src)
{
    graph::ModelList& list = dynamic_cast<graph::CoupledModel*>(model)->getModelList();
    graph::ModelList::iterator it = list.begin();

    graph::ModelList& list_clone = dynamic_cast<graph::CoupledModel*>(clone)->getModelList();
    graph::ModelList::iterator it_clone = list_clone.begin();

    while (it != list.end()) {
        if (it->second->isAtomic()) {
            copy_atomic(it->second, it_clone->second, src);
        } else {
            copy_coupled(it->second, it_clone->second, src);
        }

        ++it;
        ++it_clone;
    }
}

void CutCopyPaste::clone_atomic(graph::Model* model, graph::Model* clone,
                                vpz::AtomicModelList& vpz_list)
{
    const vpz::AtomicModel& atom(mList_vpz.get(model));
    clone->setPosition(clone->x() + 10, clone->y() + 10);
    vpz_list.add(clone, atom);
}

void CutCopyPaste::clone_coupled(graph::Model* model, graph::Model* clone,
                                 vpz::AtomicModelList& vpz_list)
{
    graph::ModelList& list = dynamic_cast<graph::CoupledModel*>(model)->getModelList();
    graph::ModelList& list_clone = dynamic_cast<graph::CoupledModel*>(clone)->getModelList();
    graph::ModelList::iterator it = list.begin();
    graph::ModelList::iterator it_clone = list_clone.begin();
    while (it != list.end()) {
        if (it->second->isAtomic()) {
            clone_atomic(it->second, it_clone->second, vpz_list);
        } else {
            clone_coupled(it->second, it_clone->second, vpz_list);
        }

        ++it;
        ++it_clone;
    }
}

} } // namespace vle gvle
