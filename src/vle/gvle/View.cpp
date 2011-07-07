/*
 * @file vle/gvle/View.cpp
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


#include <vle/gvle/ConnectionBox.hpp>
#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/ModelDescriptionBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/PortDialog.hpp>
#include <vle/gvle/View.hpp>
#include <vle/gvle/ViewDrawingArea.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/utils/Tools.hpp>
#include <gtkmm/filechooserdialog.h>
#include <gdkmm/cursor.h>
#include <limits>
#include <iostream>

using std::string;
using std::list;
using std::vector;

namespace vle { namespace gvle {

View::View(Modeling* m, graph::CoupledModel* c, size_t index) :
        mModeling(m),
        mCurrent(c),
        mIndex(index),
        mDestinationModel(NULL)
{
    assert(m);
    assert(c);

    mCompleteDrawing = new CompleteViewDrawingArea(this);
    mSimpleDrawing = new SimpleViewDrawingArea(this);
}

View::~View()
{
    delete mCompleteDrawing;
    delete mSimpleDrawing;
}

void View::redraw()
{
    mCompleteDrawing->queueRedraw();
    mSimpleDrawing->queueRedraw();
}

void View::initAllOptions()
{
    clearSelectedModels();
    mDestinationModel = NULL;

    GVLE::ButtonType current = getCurrentButton();

    switch (current) {
    case GVLE::VLE_GVLE_POINTER:
        mModeling->getGVLE()->
	    get_window()->set_cursor(Gdk::Cursor(Gdk::ARROW));
        break;
    case GVLE::VLE_GVLE_ADDMODEL:
    case GVLE::VLE_GVLE_GRID:
    case GVLE::VLE_GVLE_ADDLINK:
    case GVLE::VLE_GVLE_ADDCOUPLED:
    case GVLE::VLE_GVLE_ZOOM:
    case GVLE::VLE_GVLE_QUESTION:
    case GVLE::VLE_GVLE_PLUGINMODEL:
        mModeling->getGVLE()->
	    get_window()->set_cursor(Gdk::Cursor(Gdk::CROSSHAIR));
        break;
    case GVLE::VLE_GVLE_DELETE:
        mModeling->getGVLE()->
	    get_window()->set_cursor(Gdk::Cursor(Gdk::PIRATE));
        break;
    }
}

bool View::on_delete_event(GdkEventAny* event)
{
    if (event->type == GDK_DELETE) {
        mModeling->delViewIndex(mIndex);
        return true;
    }
    return false;
}


bool View::on_focus_in_event(GdkEventFocus* event)
{
    if (event->in == TRUE) {
        mModeling->getGVLE()->
	    showRowTreeBox(mCurrent->getName());
    }
    return true;
}

void View::addModelInListModel(graph::Model* model, bool shiftorcontrol)
{
    if (model) {
        if (shiftorcontrol == false) {
            clearSelectedModels();
        }
        addModelToSelectedModels(model);
    }
}

void View::addModelToSelectedModels(graph::Model* m)
{
    if (not existInSelectedModels(m)) {
	if (existInSelectedModels(mCurrent) or m == mCurrent) {
	    clearSelectedModels();
	}
        mSelectedModels[m->getName()] = m;
    }
}

graph::Model* View::getFirstSelectedModels()
{
    return mSelectedModels.empty() ? 0 : mSelectedModels.begin()->second;
}

bool View::existInSelectedModels(graph::Model* m)
{
    return mSelectedModels.find(m->getName()) != mSelectedModels.end();
}

void View::clearCurrentModel()
{
    if (gvle::Question(_("All children model will be deleted, continue ?"))) {
        mCurrent->delAllConnection();
        mCurrent->delAllModel();
	mModeling->vpz().project().model().atomicModels().clear();
        mModeling->getGVLE()->redrawModelTreeBox();
    }
}

void View::exportCurrentModel()
{
    const vpz::Experiment& experiment = ((const Modeling*)mModeling)
	->vpz().project().experiment();
    if (experiment.name().empty() || experiment.duration() == 0) {
        Error(_("Fix a Value to the name and the duration of the experiment before exportation."));
        return;
    }

    Gtk::FileChooserDialog file("VPZ file", Gtk::FILE_CHOOSER_ACTION_SAVE);
    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    Gtk::FileFilter filter;
    filter.set_name(_("Vle Project gZipped"));
    filter.add_pattern("*.vpz");
    file.add_filter(filter);

    if (file.run() == Gtk::RESPONSE_OK) {
        std::string filename(file.get_filename());
        vpz::Vpz::fixExtension(filename);

        vpz::Vpz* save = new vpz::Vpz();
        if (utils::Path::existFile(filename)) {
            save->clear();
        }
        save->setFilename(filename);

        mModeling->exportCoupledModel(mCurrent, save, mCurrentClass);
        delete save;
    }
}

void View::importModel()
{
    Gtk::FileChooserDialog file(_("VPZ file"), Gtk::FILE_CHOOSER_ACTION_OPEN);
    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    Gtk::FileFilter filter;
    filter.set_name(_("Vle Project gZipped"));
    filter.add_pattern("*.vpz");
    file.add_filter(filter);

    if (file.run() == Gtk::RESPONSE_OK) {
        string project_file = file.get_filename();
        try {
            vpz::Vpz* import = new vpz::Vpz(project_file);
            mModeling->importModel(mCurrent, import);
            delete import;
        } catch (std::exception& E) {
            Error(E.what());
        }
    }
}

void View::onCutModel()
{
    mModeling->setModified(true);
    mModeling->cut(mSelectedModels, mCurrent, mCurrentClass);
    mModeling->getGVLE()->redrawModelTreeBox();
    mModeling->getGVLE()->redrawModelClassBox();
    mSelectedModels.clear();
    redraw();
}

void View::onCopyModel()
{
    if (existInSelectedModels(mCurrent)) {
	mModeling->copy(mSelectedModels, mCurrent->getParent(), mCurrentClass);
    } else {
	mModeling->copy(mSelectedModels, mCurrent, mCurrentClass);
    }
    mSelectedModels.clear();
    redraw();
}

void View::onPasteModel()
{
    mModeling->setModified(true);
    mModeling->paste(mCurrent, mCurrentClass);
    mModeling->getGVLE()->redrawModelTreeBox();
    mModeling->getGVLE()->redrawModelClassBox();
    redraw();
}

void View::removeFromSelectedModel(graph::Model* mdl)
{
    std :: map < std::string, graph::Model* >:: iterator it;
    it = mSelectedModels.find(mdl->getName());
    if (it != mSelectedModels.end ()) {
        mSelectedModels.erase(it++);
        redraw();
    }
}

void View::addAtomicModel(int x, int y)
{
    ModelDescriptionBox* box = new
    ModelDescriptionBox(mModeling->getNames());
    if (box->run()) {
        graph::AtomicModel* new_atom =
            mModeling->newAtomicModel(mCurrent, box->getName(), "", x, y);

        if (new_atom) {
            try {
		if (mCurrentClass == "") {
		    mModeling->vpz().project().model().atomicModels().add(
			new_atom, vpz::AtomicModel("", "", ""));
		} else {
		    mModeling->addAtomicModelClass(mCurrentClass, new_atom);
		}
                redraw();
		mModeling->getGVLE()->redrawModelTreeBox();
		mModeling->getGVLE()->redrawModelClassBox();
		mModeling->setModified(true);
            } catch (utils::SaxParserError& e) {
                Error(e.what());
            }
        }
    }
    delete box;
}

void View::addPluginModel(int /*x*/, int /*y*/)
{
}

void View::addCoupledModel(int x, int y)
{
    ModelDescriptionBox* box;
    box = new ModelDescriptionBox(mModeling->getNames());
    if (box->run()) {
        if (not mCurrent or not mCurrent->exist(box->getName())) {
            graph::CoupledModel* new_gc =
                mModeling->newCoupledModel(mCurrent, box->getName(),
                                           "", x, y);

            new_gc->setSize(ViewDrawingArea::MODEL_WIDTH,
                            ViewDrawingArea::MODEL_HEIGHT);
            try {
                mCurrent->displace(mSelectedModels, new_gc);
            } catch (const std::exception& e) {
                mModeling->delViewOnModel(new_gc);
                mCurrent->delModel(new_gc);
            }
            mModeling->getGVLE()->redrawModelTreeBox();
            mModeling->getGVLE()->redrawModelClassBox();
            mSelectedModels.clear();
	    mModeling->setModified(true);
        } else {
            graph::Model* model = mCurrent->findModel(box->getName());
            bool select = false;
            if (model != 0)
                select = mCurrent->isInList(
                    mSelectedModels, mCurrent->findModel(box->getName()));
            if (select) {
                graph::CoupledModel* new_gc =
                    mModeling->newCoupledModel(0, box->getName(),
                                               "", x, y);

                new_gc->setSize(ViewDrawingArea::MODEL_WIDTH,
                                ViewDrawingArea::MODEL_HEIGHT);
                try {
                    mCurrent->displace(mSelectedModels, new_gc);
                } catch(const std::exception& e) {
                    mModeling->delViewOnModel(new_gc);
                    mCurrent->delModel(new_gc);
                }
                if (mCurrent) {
                    new_gc->setParent(mCurrent);
                    mCurrent->addModel(new_gc);
                }
                mModeling->getGVLE()->redrawModelTreeBox();
                mModeling->getGVLE()->redrawModelClassBox();
                mSelectedModels.clear();
		mModeling->setModified(true);
            }
        }
    }
    delete box;
}

void View::showModel(graph::Model* model)
{
    if (not model) {
        mModeling->EditCoupledModel(mCurrent);
    } else {
	if (mCurrentClass.empty()) {
            mModeling->addView(model);
        } else {
            mModeling->addViewClass(model, mCurrentClass);
        }
    }
}

void View::delModel(graph::Model* model)
{
    if (model) {
        if (gvle::Question(_("Do you really want destroy model ?"))) {
            if (model->isCoupled()) {
                mModeling->delViewOnModel((graph::CoupledModel*)model);
            }
            mModeling->delModel(model, mCurrentClass);
            mCurrent->delModel(model);
            mModeling->getGVLE()->redrawModelTreeBox();
	    mModeling->getGVLE()->redrawModelClassBox();
	    mModeling->setModified(true);
            mSelectedModels.clear();
        }
    }
}

void View::displaceModel(int oldx, int oldy, int x, int y)
{
    mModeling->setModified(true);
    if (isEmptySelectedModels() == false) {
        int dx = x - oldx;
        int dy = y - oldy;

        int minx(std::numeric_limits < int >::max());
        int miny(std::numeric_limits < int >::max());

        for (graph::ModelList::iterator it = mSelectedModels.begin();
             it != mSelectedModels.end(); ++it) {
            minx = std::min(minx, it->second->x());
            miny = std::min(miny, it->second->y());
        }

        if ((minx + dx) < 0 or (minx == std::numeric_limits < int >::max())) {
            dx = 0;
        }

        if ((miny + dy) < 0 or (miny == std::numeric_limits < int >::max())) {
            dy = 0;
        }

        for (graph::ModelList::iterator it = mSelectedModels.begin();
             it != mSelectedModels.end(); ++it) {
            it->second->setPosition(it->second->x() + dx, it->second->y() + dy);
        }
    }
}

void View::makeConnection(graph::Model* src, graph::Model* dst)
{
    assert(src and dst);

    if (src == mCurrent && dst == mCurrent)
        return;

    if (src == mCurrent and src->getInputPortList().empty()) {
        PortDialog box(src, PortDialog::INPUT);
        if (box.run() == false) {
            gvle::Error(
                (fmt(_("Connection problem:\nSource %1%, a coupled "
                       "model does not have input port")) %
                 src->getName()).str());
            return;
        }
	mModeling->setModified(true);
    }
    if (dst == mCurrent and dst->getOutputPortList().empty()) {
        PortDialog box(dst, PortDialog::OUTPUT);
        if (box.run() == false) {
            gvle::Error(
                (fmt(_("Connection problem:\nDestination %1% a "
                       "coupled model does not have output port")) %
                 dst->getName()).str());
            return;
        }
	mModeling->setModified(true);
    }
    if (src != mCurrent and src->getOutputPortList().empty()) {
        PortDialog box(src, PortDialog::OUTPUT);
        if (box.run() == false) {
            gvle::Error(
                (fmt(_("Connection problem:\nSource %1% "
                       "does not have output port")) %
                 src->getName()).str());
            return;
        }
	mModeling->setModified(true);
    }
    if (dst != mCurrent and dst->getInputPortList().empty()) {
        PortDialog box(dst, PortDialog::INPUT);
        if (box.run() == false) {
            gvle::Error(
                (fmt(_("Connection problem:\nDestination %1% "
                       "does not have input port")) %
                 dst->getName()).str());
            return;
        }
	mModeling->setModified(true);
    }

    ConnectionBox a(mCurrent, src, dst);
    if (a.run() == Gtk::RESPONSE_OK) {
        string srcPort, dstPort;

        a.getSelectedInputPort(srcPort);
        a.getSelectedOutputPort(dstPort);

        if (src == mCurrent) {
            if (not mCurrent->existInputConnection(srcPort, dst->getName(),
                                                   dstPort))
                mCurrent->addInputConnection(srcPort, dst, dstPort);
        } else if (dst == mCurrent) {
            if (not mCurrent->existOutputConnection(src->getName(), srcPort,
                                                    dstPort))
                mCurrent->addOutputConnection(src, srcPort, dstPort);
        } else {
            if (not mCurrent->existInternalConnection(src->getName(), srcPort,
                    dst->getName(), dstPort))
                mCurrent->addInternalConnection(src, srcPort, dst, dstPort);
        }
	mModeling->setModified(true);
    }
}

void View::selectedWindow()
{
    //present();
    //mDrawing->show();
}


}} // namespace vle gvle
