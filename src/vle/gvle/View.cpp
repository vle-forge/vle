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


//#include <vle/gvle/CoupledModelBox.hpp>
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
#include <vle/gvle/WarningBox.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Debug.hpp>
#include <gtkmm/filechooserdialog.h>
#include <gdkmm/cursor.h>

using std::max;
using std::min;
using std::string;
using std::list;
using std::vector;
using namespace vle;

namespace vle
{
namespace gvle {

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
    AssertI(m);
    AssertI(c);

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
    set_title(c->getName());
    mDrawing->set_size_request(mCurrent->width(), mCurrent->height());
    resize(450, 350);

    show_all();
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

bool View::on_key_release_event(GdkEventKey* event)
{
    switch (event->keyval) {
    case GDK_plus:
        mDrawing->addCoefZoom();
        break;
    case GDK_minus:
        mDrawing->delCoefZoom();
        break;
    case GDK_equal:
        mDrawing->restoreZoom();
        break;
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
    if (gvle::Question("All children model will be deleted, continue ?")) {
        mCurrent->delAllConnection();
        mCurrent->delAllModel();
        mModeling->redrawModelTreeBox();
    }
}

void View::exportCurrentModel()
{
    vpz::Experiment& experiment = mModeling->vpz().project().experiment();
    if (experiment.name().empty() || experiment.duration() == 0) {
        WarningBox box("Fix a Value to the name and the duration of the experiment before exportation.");
        box.run();
        return;
    }

    Gtk::FileChooserWidget filechooser(Gtk::FILE_CHOOSER_ACTION_SAVE);
    Gtk::FileFilter filter;
    filter.set_name("Vle Project gZipped");
    filter.add_pattern("*.vpz");
    filechooser.add_filter(filter);

    Gtk::Dialog dialog("Save as...");
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

    Gtk::VBox* vbox = dialog.get_vbox();
    vbox->pack_start(filechooser, true, true);

    dialog.show_all();

    if (dialog.run() == Gtk::RESPONSE_OK) {
        std::string filename(filechooser.get_filename());
        vpz::Vpz::fixExtension(filename);

        vpz::Vpz* save = new vpz::Vpz();
        if (utils::exist_file(filename)) {
            save->clear();
        }
        save->setFilename(filename);

        mModeling->exportCoupledModel(mCurrent, save);
        delete save;
    }
}

void View::importModel()
{
    Gtk::FileChooserWidget filechooser(Gtk::FILE_CHOOSER_ACTION_OPEN);
    Gtk::FileFilter filter;
    filter.set_name("Vle Project gZipped");
    filter.add_pattern("*.vpz");
    filechooser.add_filter(filter);

    Gtk::Dialog dialog("Import...");
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

    Gtk::VBox* vbox = dialog.get_vbox();
    vbox->pack_start(filechooser, true, true);

    dialog.show_all();

    if (dialog.run() == Gtk::RESPONSE_OK) {
        string project_file = filechooser.get_filename();
        try {
            vpz::Vpz* import = new vpz::Vpz(project_file);

            mModeling->importCoupledModel(mCurrent, import);

            delete import;
        } catch (std::exception& E) {
            std::cout << "Exception :\n" << E.what() << "\n";
        }
        /*
          GModel* m = mModeling->parseXML(project_file);
          
          if (m) {
          MapStringModel previous = mModeling->buidMapString(
          mModeling->getTopModel()->getModel());
          MapStringModel next = mModeling->buidMapString(m->getModel());
          list < string > intersection;
          
          for (MapStringModel::iterator itp = previous.begin(); itp !=
          previous.end(); itp++) {
          for (MapStringModel::iterator itn = next.begin(); itn !=
          next.end(); itn++) {
          if ((*itp).first == (*itn).first)
          intersection.push_back((*itn).first);
          }
          }
          
          if (intersection.empty() == false) {
          string s("Model(s) have same name :\n");
          for (list<string>::iterator it = intersection.begin(); it !=
          intersection.end(); ++it) {
          s += "\"" + (*it) + "\" ";
          }
          gvle::Error(s.c_str());
          delete m;
          } else {
          mCurrent->attachChildrenModel(m);
          mModeling->redrawModelTreeBox();
          }
          }
        */
    }
}

void View::onCutModel()
{
    // TODO
    mModeling->setModified(true);
    mModeling->cut(mSelectedModels, mCurrent);
    mModeling->redrawModelTreeBox();
    mSelectedModels.clear();
    mDrawing->queue_draw();
}

void View::onCopyModel()
{
    //TODO
    mModeling->copy(mSelectedModels, mCurrent);
    mSelectedModels.clear();
    mDrawing->queue_draw();
}

void View::onPasteModel()
{
    //TODO
    mModeling->setModified(true);
    mModeling->paste(mCurrent);
    mModeling->redrawModelTreeBox();
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
                mModeling->vpz().project().model().atomicModels().add(
                    new_atom, vpz::AtomicModel("", "", ""));
                mDrawing->draw();
            } catch (utils::SaxParserError& E) {

            }
            /*DynamicsBox dyn_box(vpz_am, *new_atom, mModeling->dynamics(),
            mModeling->observables(), mModeling->measures(), mModeling->conditions());
            std::cout << "&\n";
            dyn_box.run();
            */

            //AtomicModelBox box(mModeling->getGlade(), new_atom);
            //box.run();

            //AtomicModelBox* atom_box = new AtomicModelBox(new_atom);
            //list < string > lstInit, lstInput, lstOutput, lstState;
            //if (atom_box->run() == Gtk::RESPONSE_OK) {
            //atom_box->adoptModification(mModeling);
            //}
            //delete atom_box;
        }
    }
    delete box;
    mModeling->redrawModelTreeBox();
}

void View::addPluginModel(int /*x*/, int /*y*/)
{
    /*mModeling->setModified(true);
      ModelDescriptionBox*    box = NULL;
      box = new ModelDescriptionBox(mModeling->getNames());
      if (box->run()) {
          mModeling->newPluginModel(mCurrent, box->getName(), "", x, y);
      }
      delete box;*/
}

void View::addCoupledModel(int x, int y)
{
    //std::cout << "addCoupledModel x : " << x << " , y : " << y
    //    << "  on : '" << mCurrent->getName() << "'\n";

    mModeling->setModified(true);
    ModelDescriptionBox* box;
    box = new ModelDescriptionBox(mModeling->getNames());
    if (box->run()) {
        graph::CoupledModel* new_gc =
            mModeling->newCoupledModel(mCurrent, box->getName(),
                                       "", x, y);
        new_gc->setSize(ViewDrawingArea::MODEL_WIDTH,
                        ViewDrawingArea::MODEL_HEIGHT);
        mCurrent->displace(mSelectedModels, new_gc);
        mModeling->redrawModelTreeBox();
        mSelectedModels.clear();
    }
    delete box;

    /*
    mModeling->setModified(true);
    if (mCurrent->hasConnectionProblem(mSelectedModels)) {
      std::cout << "if\n";
      gvle::Error("Selected model list have connection with external"
       "model.");
      mSelectedModels.clear();
    } else if (not mSelectedModels.empty()) {
      std::cout << "else if\n";
      ModelDescriptionBox* box;
      box = new ModelDescriptionBox(mModeling->getNames());
      if (box->run()) {
      graph::CoupledModel* new_gc =
      mModeling->newCoupledModel(mCurrent, box->getName(),
      "", x, y);
      mCurrent->displace(mSelectedModels, new_gc);
    mModeling->redrawModelTreeBox();
    mSelectedModels.clear();

      }
      delete box;
    }
    */
}

void View::showModel(graph::Model* model)
{
    if (not model) {
        //CoupledModelBox* box = new CoupledModelBox(mCurrent);
        //box->run();
        //delete box;
        //std::cout << "TODO: show CoupledModelBox\n";
        mModeling->EditCoupledModel(mCurrent);
    } else {
        //std::cout << "showModel else\n";
        mModeling->addView(model);
    }
}

void View::delModel(graph::Model* model)
{
    mModeling->setModified(true);
    if (model) {
        if (gvle::Question("Do you really want destroy model ?")) {
            if (model->isCoupled()) {
                mModeling->delViewOnModel((graph::CoupledModel*)model);
            }
            //mModeling->delName(model->getName());
            mModeling->delModel(model);
            mCurrent->delModel(model);
            mModeling->redrawModelTreeBox();
        }
    }
}

//void View::delConnection(graph::Connection* connect)
//{
//mModeling->setModified(true);
//if (connect) {
//if (gvle::Question("Do you really want destroy this connection ?")) {
//mCurrent->delConnection(connect);
//}
//}
//}

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
    AssertI(src and dst);

    if (src == mCurrent && dst == mCurrent)
        return;

    if (src == mCurrent and src->getInputPortList().empty()) {
        PortDialog box(src, PortDialog::INPUT);
        if (box.run() == false) {
            gvle::Error(
                (boost::format("Connection problem:\nSource %1%, a coupled "
                               "model does not have input port") %
                 src->getName()).str());
            return;
        }
    }
    if (dst == mCurrent and dst->getOutputPortList().empty()) {
        PortDialog box(dst, PortDialog::OUTPUT);
        if (box.run() == false) {
            gvle::Error(
                (boost::format("Connection problem:\nDestination %1% a "
                               "coupled model does not have output port") %
                 dst->getName()).str());
            return;
        }
    }
    if (src != mCurrent and src->getOutputPortList().empty()) {
        PortDialog box(src, PortDialog::OUTPUT);
        if (box.run() == false) {
            gvle::Error(
                (boost::format("Connection problem:\nSource %1% "
                               "does not have output port") %
                 src->getName()).str());
            return;
        }
    }
    if (dst != mCurrent and dst->getInputPortList().empty()) {
        PortDialog box(dst, PortDialog::INPUT);
        if (box.run() == false) {
            gvle::Error(
                (boost::format("Connection problem:\nDestination %1% "
                               "does not have input port") %
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
    mDrawing->get_window()->clear();
    mDrawing->show();
}

void  View::updateAdjustment(double h, double v)
{
    mViewport.get_hadjustment()->set_value(h);
    mViewport.get_hadjustment()->value_changed();

    mViewport.get_vadjustment()->set_value(v);
    mViewport.get_vadjustment()->value_changed();
}

}
} // namespace vle gvle
