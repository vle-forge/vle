/**
 * @file vle/gvle/View.cpp
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


#include <vle/gvle/ConnectionBox.hpp>
#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/ModelDescriptionBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/PortDialog.hpp>
#include <vle/gvle/Plugin.hpp>
#include <vle/gvle/View.hpp>
#include <vle/gvle/ViewDrawingArea.hpp>
#include <vle/gvle/ViewMenu.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Debug.hpp>
#include <gtkmm/filechooserdialog.h>
#include <gdkmm/cursor.h>
#include <iostream>

using std::string;
using std::list;
using std::vector;

namespace vle { namespace gvle {

View::View(Modeling* m, graph::CoupledModel* c, size_t index) :
        mModeling(m),
        mCurrent(c),
        mIndex(index),
        mVbox(false, 0),
        mAdjustWidth(0,0,1),
        mAdjustHeigth(0,0,1),
        mViewport(mAdjustWidth, mAdjustHeigth),
        mScrolledWindow(),
        mDestinationModel(NULL)
{
    assert(m);
    assert(c);

    mMenuBar = new ViewMenu(this);
    mDrawing = new ViewDrawingArea(this);
    mViewport.add(*mDrawing);
    mViewport.set_shadow_type(Gtk::SHADOW_NONE);
    mViewport.set_border_width(0);

    mScrolledWindow.set_policy(Gtk::POLICY_ALWAYS, Gtk::POLICY_ALWAYS);
    mScrolledWindow.set_flags(Gtk::CAN_FOCUS);
    mScrolledWindow.add(mViewport);
    mScrolledWindow.set_shadow_type(Gtk::SHADOW_NONE);
    mScrolledWindow.set_border_width(0);

    mVbox.pack_start(*mMenuBar, Gtk::PACK_SHRINK);
    mVbox.pack_start(mScrolledWindow, Gtk::PACK_EXPAND_WIDGET);

    add(mVbox);
    mDrawing->set_size_request(mCurrent->width(), mCurrent->height());
    resize(450, 350);

    //show_all();
}

View::~View()
{
    delete mMenuBar;
    delete mDrawing;
}

void View::redraw()
{
    mDrawing->draw();
}

void View::setTitle(const Glib::ustring& name)
{
    if (mCurrent) {
        Glib::ustring title(name);
        title += " - ";
        title += mCurrent->getName();

        set_title(title);
    }
}

void View::setModifiedTitle()
{
    Glib::ustring current("* ");
    current += get_title();

    set_title(current);
}

void View::initAllOptions()
{
    clearSelectedModels();
    mDestinationModel = NULL;

    GVLE::ButtonType current = getCurrentButton();

    switch (current) {
    case GVLE::POINTER:
        get_window()->set_cursor(Gdk::Cursor(Gdk::ARROW));
        break;
    case GVLE::ADDMODEL:
    case GVLE::GRID:
    case GVLE::ADDLINK:
    case GVLE::ADDCOUPLED:
    case GVLE::ZOOM:
    case GVLE::QUESTION:
    case GVLE::PLUGINMODEL:
        get_window()->set_cursor(Gdk::Cursor(Gdk::CROSSHAIR));
        break;
    case GVLE::DELETE:
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
        mModeling->showRowTreeBox(mCurrent->getName());
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
    vpz::Experiment& experiment = mModeling->vpz().project().experiment();
    if (experiment.name().empty() || experiment.duration() == 0) {
        Error(_("Fix a Value to the name and the duration of the experiment before exportation."));
        return;
    }

    Gtk::FileChooserDialog file("VPZ file", Gtk::FILE_CHOOSER_ACTION_SAVE);
    file.set_transient_for(*this);
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
        if (utils::exist_file(filename)) {
            save->clear();
        }
        save->setFilename(filename);

        mModeling->exportCoupledModel(mCurrent, save, mCurrentClass);
        delete save;
    }
}

void View::exportGraphic()
{
    vpz::Experiment& experiment = mModeling->vpz().project().experiment();
    if (experiment.name().empty() || experiment.duration() == 0) {
        Error(_("Fix a Value to the name and the duration of the experiment before exportation."));
        return;
    }

    Gtk::FileChooserDialog file(_("Image file"), Gtk::FILE_CHOOSER_ACTION_SAVE);
    file.set_transient_for(*this);
    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    Gtk::FileFilter filterPng;
    Gtk::FileFilter filterPdf;
    Gtk::FileFilter filterSvg;
    filterPng.set_name(_("Portable Newtork Graphics (.png)"));
    filterPng.add_pattern("*.png");
    filterPdf.set_name(_("Portable Format Document (.pdf)"));
    filterPdf.add_pattern("*.pdf");
    filterSvg.set_name(_("Scalable Vector Graphics (.svg)"));
    filterSvg.add_pattern("*.svg");
    file.add_filter(filterPng);
    file.add_filter(filterPdf);
    file.add_filter(filterSvg);


    if (file.run() == Gtk::RESPONSE_OK) {
        std::string filename(file.get_filename());
	std::string extension(file.get_filter()->get_name());

	if (extension == _("Portable Newtork Graphics (.png)"))
	    mDrawing->exportPng(filename);
	else if (extension == _("Portable Format Document (.pdf)"))
	    mDrawing->exportPdf(filename);
	else if (extension == _("Scalable Vector Graphics (.svg)"))
	    mDrawing->exportSvg(filename);
    }
}

void View::importModel()
{
    Gtk::FileChooserDialog file(_("VPZ file"), Gtk::FILE_CHOOSER_ACTION_OPEN);
    file.set_transient_for(*this);
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
            std::cout << "Exception :\n" << E.what() << "\n";
        }
    }
}

void View::onCutModel()
{
    // TODO
    mModeling->setModified(true);
    mModeling->cut(mSelectedModels, mCurrent, mCurrentClass);
    mModeling->redrawModelTreeBox();
    mModeling->redrawModelClassBox();
    mSelectedModels.clear();
    mDrawing->queue_draw();
}

void View::onCopyModel()
{
    //TODO
    if (existInSelectedModels(mCurrent)) {
	mModeling->copy(mSelectedModels, mCurrent->getParent(), mCurrentClass);
    } else {
	mModeling->copy(mSelectedModels, mCurrent, mCurrentClass);
    }
    mSelectedModels.clear();
    mDrawing->queue_draw();
}

void View::onPasteModel()
{
    //TODO
    mModeling->setModified(true);
    mModeling->paste(mCurrent, mCurrentClass);
    mModeling->redrawModelTreeBox();
    mModeling->redrawModelClassBox();
    mDrawing->queue_draw();
}

void View::addAtomicModel(int x, int y)
{
    mModeling->setModified(true);
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
                mDrawing->draw();
            } catch (utils::SaxParserError& e) {
                Error(e.what());
            }
        }
    }
    delete box;
    mModeling->getGVLE()->redrawModelTreeBox();
    mModeling->getGVLE()->redrawModelClassBox();
}

void View::addPluginModel(int /*x*/, int /*y*/)
{
}

void View::addCoupledModel(int x, int y)
{
    mModeling->setModified(true);
    ModelDescriptionBox* box;
    box = new ModelDescriptionBox(mModeling->getNames());
    if (box->run()) {
	if (not mCurrent or not mCurrent->exist(box->getName())) {
	    graph::CoupledModel* new_gc =
		mModeling->newCoupledModel(mCurrent, box->getName(),
					   "", x, y);

	    new_gc->setSize(ViewDrawingArea::MODEL_WIDTH,
			    ViewDrawingArea::MODEL_HEIGHT);
	    mCurrent->displace(mSelectedModels, new_gc);
	    mModeling->getGVLE()->redrawModelTreeBox();
	    mModeling->getGVLE()->redrawModelClassBox();
	    mSelectedModels.clear();

	} else {
	    graph::Model* model = mCurrent->findModel(box->getName());
	    bool select = false;
	    if (model != 0)
		select = mCurrent->isInList(mSelectedModels,
					    mCurrent->findModel(
						box->getName()));
	    if (select) {

		graph::CoupledModel* new_gc =
		    mModeling->newCoupledModel(0, box->getName(),
					       "", x, y);

		new_gc->setSize(ViewDrawingArea::MODEL_WIDTH,
				ViewDrawingArea::MODEL_HEIGHT);
		mCurrent->displace(mSelectedModels, new_gc);
		if (mCurrent) {
		    new_gc->setParent(mCurrent);
		    mCurrent->addModel(new_gc);
		}
		mModeling->getGVLE()->redrawModelTreeBox();
		mModeling->getGVLE()->redrawModelClassBox();
		mSelectedModels.clear();
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
	if (mCurrentClass == "")
	    mModeling->addView(model);
	else
	    mModeling->addViewClass(model, mCurrentClass);
    }
}

void View::delModel(graph::Model* model)
{
    mModeling->setModified(true);
    if (model) {
        if (gvle::Question(_("Do you really want destroy model ?"))) {
            if (model->isCoupled()) {
                mModeling->delViewOnModel((graph::CoupledModel*)model);
            }
            mModeling->delModel(model, mCurrentClass);
            mCurrent->delModel(model);
            mModeling->getGVLE()->redrawModelTreeBox();
	    mModeling->getGVLE()->redrawModelClassBox();
        }
    }
}

void View::displaceModel(int oldx, int oldy, int x, int y)
{
    mModeling->setModified(true);
    if (isEmptySelectedModels() == false) {
        int dx = x - oldx;
        int dy = y - oldy;

        for (graph::ModelList::iterator it = mSelectedModels.begin();
                it != mSelectedModels.end(); ++it) {
            it->second->setPosition(it->second->x() + dx, it->second->y() + dy);
        }
    }
}

void View::makeConnection(graph::Model* src, graph::Model* dst)
{
    mModeling->setModified(true);
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
    }
}

void View::selectedWindow()
{
    //present();
    //mDrawing->show();
}

void  View::updateAdjustment(double h, double v)
{
    mViewport.get_hadjustment()->set_lower(h);
    mViewport.get_hadjustment()->set_value(h);

    mViewport.get_vadjustment()->set_lower(v);
    mViewport.get_vadjustment()->set_value(v);
}

void View::addCoefZoom()
{
    mDrawing->addCoefZoom();
}

void View::delCoefZoom()
{
    mDrawing->delCoefZoom();
}

void View::setCoefZoom(double coef)
{
    mDrawing->setCoefZoom(coef);
}


}} // namespace vle gvle
