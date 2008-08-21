/**
 * @file vle/gvle/ExpMeasures.hpp
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


#ifndef GUI_EXPMEASURES_HPP
#define GUI_EXPMEASURES_HPP

#include <vle/gvle/TreeViewSingleString.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/DialogString.hpp>
#include <vle/gvle/ComboboxString.hpp>
#include <gtkmm/dialog.h>
#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treestore.h>
#include <list>
#include <string>
#include <vle/utils/Tools.hpp>

namespace vle
{
namespace graph {

class Model;
class AtomicModel;
class CoupledModel;

}
} // namespace vle graph

namespace vle
{
namespace gvle {

class Modeling;

/**
 * A Dialog to get information on a new Outputs for ExpMeasures class
 * TreeView outputs
 *
 */
class OutputDialogMeasures : public Gtk::Dialog
{
public:
    OutputDialogMeasures(Modeling* modeling,
                         const std::string& name,
                         const std::string& format,
                         const std::string& plugin,
                         const std::string& location,
                         const std::string& parameters);

    virtual ~OutputDialogMeasures() {}

    /**
     * return name set by users
     *
     * @return string name entry by user.
     */
    std::string getName() const {
        return mEntryName.get_text();
    }

    /**
     * return format set by user
     *
     * @return string format select by user
     */
    std::string getFormat() const {
        return mComboFormat.get_active_string();
    }

    /**
     * return location set by user
     *
     * @return string location select by user
     */
    std::string getLocation() const {
        return mEntryLocation.get_text();
    }

    /**
     * return parameter set by user
     *
     * @return XML string parameter select by user
     */
    std::string getParameters() const {
        return mParameters;
    }

    std::string getPlugin() const {
        return mComboPlugin.get_active_string();
    }

private:
    void on_button_location_clicked();
    void on_button_parameters_clicked();

    Gtk::HBox                       mHBoxName;
    Gtk::Label                      mLabelName;
    Gtk::Entry                      mEntryName;
    Gtk::HBox                       mHBoxFormat;
    Gtk::Label                      mLabelFormat;
    gvle::ComboBoxString         mComboFormat;
    Gtk::HBox                       mHBoxPlugin;
    Gtk::Label                      mLabelPlugin;
    gvle::ComboBoxString         mComboPlugin;
    Gtk::HBox                       mHBoxLocation;
    Gtk::Label                      mLabelLocation;
    Gtk::Entry                      mEntryLocation;
    Gtk::Button                     mButtonLocation;
    Gtk::HBox                       mHBoxParameters;
    Gtk::Label                      mLabelParameters;
    Gtk::Button                     mButtonParameters;
    std::string                     mParameters;
    Modeling*                       mModeling;
};


/**
 * A Dialog to get information on a new Measure for ExpMeasures class
 * TreeView measures
 *
 */
class MeasureDialog : public Gtk::Dialog
{
public:
    MeasureDialog(const std::list < std::string >& lst,
                  const std::string name,
                  const std::string type,
                  const std::string timeStep,
                  const std::string output);

    virtual ~MeasureDialog() {}

    /**
     * get name set by user
     *
     * @return string set by user
     */
    std::string getName() {
        return mEntryName.get_text();
    }

    /**
     * get type select by user
     *
     * @return string select by user
     */
    std::string getType() {
        return mComboType.get_active_string();
    }

    /**
     * get time step set by user
     *
     * @return string set by user
     */
    std::string getTimeStep() {
        return mEntryTimeStep.get_text();
    }

    /**
     * get output select by user
     *
     * @return string select by user
     */
    std::string getOutput() {
        return mComboOutput.get_active_string();
    }

private:
    void on_combotype_changed();

    Gtk::HBox                       mHBoxName;
    Gtk::Label                      mLabelName;
    Gtk::Entry                      mEntryName;
    Gtk::HBox                       mHBoxType;
    Gtk::Label                      mLabelType;
    gvle::ComboBoxString         mComboType;
    Gtk::HBox                       mHBoxTimeStep;
    Gtk::Label                      mLabelTimeStep;
    Gtk::Entry                      mEntryTimeStep;
    Gtk::HBox                       mHBoxOutput;
    Gtk::Label                      mLabelOutput;
    gvle::ComboBoxString         mComboOutput;
};

/**
 * A Dialog to get information for a new Observable for ExpMeasures class
 * TreeView measures
 *
 */
class ObservableDialog : public Gtk::Dialog
{
public:
    ObservableDialog(graph::Model* top,
                     const std::string& name,
                     const std::string& port,
                     const std::string& group,
                     int index);

    virtual ~ObservableDialog() {}

    /**
     * get group name select by user
     *
     * @return group name seleted by user
     */
    std::string getGroup() const {
        return mEntryGroup.get_text();
    }

    /**
     * get index select by user
     *
     * @return index seleted by user
     */
    int getIndex() const {
        return utils::to_int(mEntryIndex.get_text());
    }

    /**
     * get model name select by user
     *
     * @return model name seleted by user
     */
    std::string getName() const {
        return mComboName.get_active_string();
    }

    /**
     * get port name selected by user
     *
     * @return model name selected by user
     */
    std::string getPort() const {
        return mComboPort.get_active_string();
    }

private:
    /**
     * Fill combobox name with all GAtomicModel children model of
     * GCoupledModel parent
     *
     * @param parent of GAtomicModel to show
     */
    void fillComboName(graph::CoupledModel* parent);

    /**
     * When user change combobox name, port list associed is modified to
     * show children
     *
     */
    void on_comboname_changed();

    graph::Model*              mTop;
    Gtk::HBox                       mHBoxName;
    Gtk::Label                      mLabelName;
    gvle::ComboBoxString         mComboName;
    Gtk::HBox                       mHBoxPort;
    Gtk::Label                      mLabelPort;
    gvle::ComboBoxString         mComboPort;
    Gtk::HBox                       mHBoxGroup;
    Gtk::Label                      mLabelGroup;
    Gtk::Entry                      mEntryGroup;
    Gtk::HBox                       mHBoxIndex;
    Gtk::Label                      mLabelIndex;
    Gtk::Entry                      mEntryIndex;
};

/**
 * A class to define treeview column in output treeview
 *
 */
class TreeColumnOutputsMeasures : public Gtk::TreeModel::ColumnRecord
{
public:
    TreeColumnOutputsMeasures() {
        add(mName);
        add(mFormat);
        add(mPlugin);
        add(mLocation);
    }

    Gtk::TreeModelColumn < Glib::ustring > mName;
    Gtk::TreeModelColumn < Glib::ustring > mFormat;
    Gtk::TreeModelColumn < Glib::ustring > mPlugin;
    Gtk::TreeModelColumn < Glib::ustring > mLocation;
};

/**
 * A class to define treeview column in eovs treeview
 *
 */
class TreeColumnEovs : public Gtk::TreeModel::ColumnRecord
{
public:
    TreeColumnEovs() {
        add(mName);
    }

    Gtk::TreeModelColumn < Glib::ustring > mName;
};

/**
 * A class to define treeview column in measures treeview
 *
 */
class TreeColumnMeasures : public Gtk::TreeModel::ColumnRecord
{
public:
    TreeColumnMeasures() {
        add(mName);
        add(mText);
        add(mModel);
        add(mPort);
        add(mGroup);
        add(mIndex);
        add(mMeasure);
    }

    Gtk::TreeModelColumn < Glib::ustring > mName;
    Gtk::TreeModelColumn < Glib::ustring > mText;
    Gtk::TreeModelColumn < Glib::ustring > mModel;
    Gtk::TreeModelColumn < Glib::ustring > mPort;
    Gtk::TreeModelColumn < Glib::ustring > mGroup;
    Gtk::TreeModelColumn < int >           mIndex;
    Gtk::TreeModelColumn < Glib::ustring > mMeasure;
};

/**
 * An HBox class what contains Experiments information: outputs and
 * Observers to run a simulation.
 *
 */
class ExpMeasures : public Gtk::VBox
{
public:
    /**
     * Construct HBox with all Measures from Modeling ptr
     *
     * @param mod Modeling parameter to initialise ExpMeasures
     */
    ExpMeasures(Modeling* mod);

    /**
     * Delete frame :(
     */
    virtual ~ExpMeasures() {}

private:

    static std::string buildMeasureText(const std::string& type,
                                        const std::string& timeStep,
                                        const std::string& output);

    static std::string buildObservableText(const std::string& model,
                                           const std::string& port,
                                           const std::string& group,
                                           int index);

    /**
     * Read modeling information and put data from output in TreeViewOutputs
     *
     */
    void initOutputs();

    void on_button_add_output();
    void on_button_edit_output();
    void on_button_del_output();


    /**
     * Read modeling information and put data from measures to
     * TreeViewMeasures
     *
     */
    void initMeasures();

    /**
     * translate Measures::Outputs_t to Eov's data
     */
    std::vector < std::pair < std::string, std::string > >
    translate_output_to_eov();

    void on_button_add_eov();
    void on_button_edit_eov();
    void on_button_del_eov();

    /**
     * Update the eovs when an output is delete
     * @param lib the lib output
     * @param output the output output
     */
    void updateEovs(std::string lib, std::string output);

    void eov_name_on_cell_data(
        Gtk::CellRenderer* renderer,
        const Gtk::TreeModel::iterator& iter);
    void eov_cellrenderer_name_on_editing_started(Gtk::CellEditable* cell_editable, const Glib::ustring& path);
    void eov_cellrenderer_name_on_edited(const Glib::ustring& path_string, const Glib::ustring& new_text);

    /**
     * get current selected string. If no selection, return empty string
     *
     * @return current selected string or empty string if no selected
     */
    std::string eov_get_selected_row();
    /**
     * add a string into TreeView.
     *
     * @param name std::string to add into TreeView.
     */
    void eov_add_row(const std::string& name);

    /**
     * delete string from TreeView if exist.
     *
     * @param name to delete from TreeView.
     */
    void eov_del_row(const std::string& name);

    void on_button_add_measure();
    void on_button_edit_measure();
    void on_button_del_measure();
    void on_button_add_observable();
    void on_button_edit_observable();
    void on_button_del_observable();
    void on_select_row_measure();

    Modeling*                       mModeling;

    Gtk::Frame                      mFrameOutputs;
    Gtk::VBox                       mVBoxOutputs;
    TreeColumnOutputsMeasures       mColumnsOutputs;
    Gtk::TreeView                   mTreeViewOutputs;
    Glib::RefPtr < Gtk::ListStore > mListStoreOutputs;
    Gtk::HBox                       mHBoxButtonsOutput;
    Gtk::Button                     mButtonAddOutput;
    Gtk::Button                     mButtonEditOutput;
    Gtk::Button                     mButtonDelOutput;

    Gtk::Frame                      mFrameEovs;
    Gtk::VBox                       mVBoxEovs;
    TreeColumnEovs      mColumnsEovs;
    Gtk::TreeView                   mTreeViewEovs;
    Glib::RefPtr < Gtk::ListStore > mListStoreEovs;
    Gtk::CellRendererText     mCellrendererName;
    Gtk::TreeView::Column     mTreeviewColumnName;
    bool       mNameRetry;
    Glib::ustring      mInvalidTextForRetry;
    Gtk::HBox                       mHBoxButtonsEov;
    Gtk::Button                     mButtonAddEov;
    Gtk::Button                     mButtonEditEov;
    Gtk::Button                     mButtonDelEov;

    Gtk::Frame                      mFrameMeasures;
    Gtk::VBox                       mVBoxMeasures;
    TreeColumnMeasures              mColumnsMeasures;
    Gtk::TreeView                   mTreeViewMeasures;
    Glib::RefPtr < Gtk::TreeStore > mTreeStoreMeasures;
    Gtk::HBox                       mHBoxButtonsMeasure;
    Gtk::Button                     mButtonAddMeasure;
    Gtk::Button                     mButtonEditMeasure;
    Gtk::Button                     mButtonDelMeasure;
    Gtk::HBox                       mHBoxButtonsObservable;
    Gtk::Button                     mButtonAddObservable;
    Gtk::Button                     mButtonEditObservable;
    Gtk::Button                     mButtonDelObservable;
};

}
} // namespace vle gvle

#endif
