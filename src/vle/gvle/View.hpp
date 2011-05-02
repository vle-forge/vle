/*
 * @file vle/gvle/View.hpp
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


#ifndef GUI_VIEW_HPP
#define GUI_VIEW_HPP

#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/SimpleViewDrawingArea.hpp>
#include <vle/graph/Model.hpp>
#include <gtkmm/window.h>
#include <gtkmm/viewport.h>
#include <algorithm>
#include <string>
#include <list>
#include <map>

namespace vle { namespace gvle {

class Modeling;
class ViewMenu;
class CompleteViewDrawingArea;
//class SimpleViewDrawingArea;

/**
 * @brief A Gtk::Window is use to define the view of a GCoupledModel. This
 * gui::View manage event from gui::Modeling class and use the
 * gui::ViewDrawingArea to draw on screen small box and line.
 */
class View
{
public:
    View(Modeling* m, graph::CoupledModel* c, size_t index);

    virtual ~View();

    void redraw();

    /**
     * Get current button type from GVLE GUI.
     *
     * @return a GVLE::ButtonType
     */
    inline GVLE::ButtonType getCurrentButton() const {
        return mModeling->getCurrentButton();
    }

    /**
     * Initialize all selected models, destination, and change mouse
     * pointer type.
     */
    void initAllOptions();

    //
    //
    // GET FUNCTION
    //
    //

    /**
     * Get document Modeling ptr.
     *
     * @return ptr to Modelig document
     */
    inline Modeling* getModeling() {
        return mModeling;
    }

    /**
     * Get GCoupledModel ptr.
     *
     * @return ptr to GCoupledModel
     */
    inline graph::CoupledModel* getGCoupledModel() {
        return mCurrent;
    }

    /**
     * Get view Index value.
     *
     * @return index value
     */
    inline guint getIndex() const {
        return mIndex;
    }

    inline CompleteViewDrawingArea* getCompleteArea() {
        return mCompleteDrawing;
    }

    inline SimpleViewDrawingArea* getSimpleArea() {
        return mSimpleDrawing;
    }

    //
    //
    // MANAGE GTK::WINDOW EVENT
    //
    //

    /**
     * When delete view, call modeling to delete window.
     *
     * @param event event structure
     * @return true
     */
    bool on_delete_event(GdkEventAny* event);

    /**
     * When focus, call modeling to show current view.
     *
     * @param event focus event structure
     * @return true
     */
    bool on_focus_in_event(GdkEventFocus* event);

    //
    //
    // MANAGE CURRENT SELECTED MODELS
    //
    //

    /**
     * add a graph::Model under coordinate into Model selected list for
     * coupling model into a GCoupledModel.
     *
     * @param model model to append into selected models
     * @param shiftorcontrol true if shift or control pressed during click
     * otherwise fale
     */
    void addModelInListModel(graph::Model* model, bool shiftorcontrol);

    /**
     * add graph::Model into selected models lists.
     *
     * @param model to append into selected models
     */
    void addModelToSelectedModels(graph::Model* model);

    /**
     * get selected models state.
     *
     * @return true if Selected models list is empty
     */
    inline bool isEmptySelectedModels() {
        return mSelectedModels.empty();
    }

    /** Clean selected models list. */
    void clearSelectedModels();

    /**
     * @brief Get first selected models from list.
     * @return first selected models from list, or null if empty list
     */
    graph::Model* getFirstSelectedModels();

    /**
     * @brief test if model exist in selected modeds.
     * @param m model to test
     * @return true if found, otherwise false
     */
    bool existInSelectedModels(graph::Model* m);

    /**
     * @brief return complete selected models.
     * @return get selected models list
     */
    inline const graph::ModelList& getAllSelectedModels() const {
        return mSelectedModels;
    }

    //
    //
    // MANAGE DESTINATION MODEL
    //
    //

    inline void addDestinationModel(graph::Model* m) {
        mDestinationModel = m;
    }

    inline graph::Model* getDestinationModel() {
        return mDestinationModel;
    }

    //
    //
    // MANAGE CLASS ASPECT
    //
    //

    inline void setCurrentClass(std::string classeName)
    { mCurrentClass = classeName; }

    inline std::string getCurrentClass() const
    { return mCurrentClass; }

    inline bool isClassView() const
    { return !mCurrentClass.empty(); }

    //
    //
    // IMPORT / EXPORT / CLEAR VIEW
    //
    //

    /** When user click on clear menu item. */
    void clearCurrentModel();

    /** export the current model into XML file. */
    void exportCurrentModel();

    /** import a model into current GCoupledModel. */
    void importModel();

    //
    //
    // CUT / COPY / PASTE
    //
    //

    /** Cut selected model list into CutCopyPaste. */
    void onCutModel();

    /** Copy selected model list into CutCopyPaste. */
    void onCopyModel();

    /**
     * Paste selected model list from CutCopyPaste into this GCoupledModel.
     */
    void onPasteModel();

     /** Select all models in VPZ. */
    void onSelectAll(graph::CoupledModel* cModel);

    //
    //
    // MANAGE MODELS
    //
    //

    /**
     * Unselect a model
     *
     * @param mdl model to unselect
     */
    void removeFromSelectedModel(graph::Model* mdl);

     /**
     * Add a single atomic model in position x, y.
     *
     * @param x new position of atomic model
     * @param y new position of atomic model
     */
    void addAtomicModel(int x, int y);

    /**
     * Add a plugin atomic model in position x, y.
     *
     * @param x new position of plugin model
     * @param y new position of plugin model
     */
    void addPluginModel(int x, int y);

    /**
     * Add a coupled model in position x, y.
     *
     * @param x new position of coupled model
     * @param y new position of coupled model
     */
    void addCoupledModel(int x, int y);

    /**
     * Show model information.
     *
     * @param model graph::Model to show information
     */
    void showModel(graph::Model* model);

    /**
     * Delete Model after Message::Question.
     *
     * @param model graph::Model to delete
     */
    void delModel(graph::Model* model);

    /**
     * Delete Connection after Message::Question.
     *
     * @param connection to delete
     */
    //void delConnection(graph::Connection* connect);

    /**
     * Displace all selected model to a new position in current view.
     *
     * @param oldx old position of model
     * @param oldy old position of model
     * @param x new position of model
     * @param y new position of model
     */
    void displaceModel(int oldx, int oldy, int x, int y);

    /**
     * Make a connection between two children models of current
     * model; on error, message box.
     *
     * @param src
     * @param dst
     */
    void makeConnection(graph::Model* src, graph::Model* dst);

    /**
     * To flash window, to inform user of this selected window.
     */
    void selectedWindow();

private:
    Modeling*                   mModeling;
    graph::CoupledModel*        mCurrent;
    std::string                 mCurrentClass;
    size_t                      mIndex;

    CompleteViewDrawingArea*    mCompleteDrawing;
    SimpleViewDrawingArea*      mSimpleDrawing;

    graph::ModelList            mSelectedModels;
    graph::Model*               mDestinationModel;
    GVLE::ButtonType            mCurrentButton;
};

}} // namespace vle gvle

#endif
