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


#include <vle/gvle/CutCopyPaste.hpp>
#include <vle/gvle/GVLE.hpp>
#include <boost/lexical_cast.hpp>

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

void CutCopyPaste::cut(vpz::ModelList& l, vpz::CoupledModel* parent)
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
    } else {
        mGVLE->showMessage(_("Nothing to Cut - parent is ") + parent->getName());
    }
}

void CutCopyPaste::copy(vpz::ModelList& l, vpz::CoupledModel* parent, bool isClass)
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

        for (vpz::ModelList::iterator it = l.begin();
             it != l.end(); ++it) {
            vpz::BaseModel* m = it->second->clone();

            m->setParent(NULL);
            mList_graph.insert(make_pair(m->getName(), m));
        }
    }
}

void CutCopyPaste::paste(vpz::CoupledModel* gc)
{
    vpz::ModelList list_graph_clone;

    if (gc) {
        if (mType == CUT) {
            clone(list_graph_clone,
                  (mNumero == 0) ? -1 : mNumero - 1);
            ++mNumero;
        } else if (mType == COPY) {
            clone(list_graph_clone,
                  mNumero);
            ++mNumero;
        }

        // to keep initial connections
        std::vector <std::string> mwCnts= mCnts;

        // to avoid naming conflicts.
        for (vpz::ModelList::const_iterator itmls = list_graph_clone.begin();
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

                vpz::BaseModel::rename(itmls->second, mnewname);

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
    for (vpz::ModelList::iterator it = mList_graph.begin();
            it != mList_graph.end();
            ++it) {
        delete it->second;
    }
    mList_graph.clear();

    mNumero = 0;
    mType = NOTHING;
}

void CutCopyPaste::clone(vpz::ModelList& lst_graph_clone, int num)
{
    lst_graph_clone.clear();

    if (num == -1) {
        for (vpz::ModelList::iterator it = mList_graph.begin();
                it != mList_graph.end();
                ++it) {
            vpz::BaseModel* m = it->second->clone();

            lst_graph_clone.insert(make_pair(m->getName(), m));
            if (m->isCoupled()) {
                clone_coupled(it->second, m);
            } else {
                clone_atomic(m);
            }
        }

    } else {
        ++num;
        for (vpz::ModelList::iterator it = mList_graph.begin(); it !=
                mList_graph.end();
                ++it) {
            vpz::BaseModel* m = it->second->clone();
            m->setParent(NULL);
            m->setPosition(m->x() + (num * 10), m->y() + (num * 10));

            lst_graph_clone.insert(make_pair(m->getName(), m));
            if (m->isCoupled()) {
                clone_coupled(it->second, m);
            } else {
                clone_atomic(m);
            }
        }
    }
}

void CutCopyPaste::clone_atomic(vpz::BaseModel* clone)
{
    clone->setPosition(clone->x() + 10, clone->y() + 10);
}

void CutCopyPaste::clone_coupled(vpz::BaseModel* model, vpz::BaseModel* clone)
{
    vpz::ModelList& list = dynamic_cast<vpz::CoupledModel*>(model)->getModelList();
    vpz::ModelList& list_clone = dynamic_cast<vpz::CoupledModel*>(clone)->getModelList();
    vpz::ModelList::iterator it = list.begin();
    vpz::ModelList::iterator it_clone = list_clone.begin();
    while (it != list.end()) {
        if (it->second->isAtomic()) {
            clone_atomic(it_clone->second);
        } else {
            clone_coupled(it->second, it_clone->second);
        }

        ++it;
        ++it_clone;
    }
}

} } // namespace vle gvle
