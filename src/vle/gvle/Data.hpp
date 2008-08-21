/**
 * @file vle/gvle/Data.hpp
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


#ifndef GUI_DATA_HPP
#define GUI_DATA_HPP

#include <vle/gvle/ComboboxString.hpp>
#include <libxml++/libxml++.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/dialog.h>
#include <gtkmm/frame.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/entry.h>
#include <gtkmm/treeview.h>
#include <gtkmm/button.h>

namespace vle
{
namespace gvle {

/**
 * @brief A Gtk::Dialog to select a VLE data type, string, integer, real,
 */
class TypeDialog : public::Gtk::Dialog
{
public:
    TypeDialog(const std::string & type, const std::string & value);

    virtual ~TypeDialog() {}

    /**
     * Return type selected by user
     *
     * @return string representation of type
     */
    std::string getType() const;

    /**
     * Return value selected by user
     *
     * @return string representation of value
     */
    std::string getValue() const;

private:
    Gtk::Frame                  mFrameType;
    Gtk::HBox                   mHBoxType;
    ComboBoxString     mComboType;
    Gtk::Label                  mLabelValueType;
    Gtk::Entry                  mEntryValueType;
};


/**
 * Data class dialog to edit a complete data type: simple (Double, String,
 * etc.) and complex like list, random etc.
 *
 */
class Data : public Gtk::Dialog
{
public:
    Data(const std::string& xml);

    /**
     * Nothing allocated
     */
    virtual ~Data() {}

    /**
     * Read graphics interface user an generate XML corresponding with use
     * choice.
     *
     * @return string transformation of a XML data
     */
    std::string getDataXML() const;

    /**
     * Valid value with type. If type = Double, then to_double is call with
     * value in parameter.
     *
     * @param type string representation of data type
     * @param value string representation of data type value
     */
    static void validDataTypeValue(const std::string& type,
                                   std::string& value);

private:
    /** Create interface manage type */
    void makeFrameType(const std::string& type, const std::string& value);

    /** Create interface manage list type */
    void makeFrameList(xmlpp::Element*,bool);

    /** Create interface manage random type */
    void makeFrameRandom(xmlpp::Element*,bool);

    /** Hide all complex widget and restore size of dialog */
    void hideComplex();

    /**
     * Add under root node, XML information of current selected random
     *
     * @param root node under attach information
     */
    void getRandomDataXML(xmlpp::Element* root) const;

    /**
     * Add under root node, XML information of current selected list.
     * TreeView list iterate to find information
     *
     * @param root node under attach information
     */
    void getListDataXML(xmlpp::Element* root) const;

    /** When user select combobox type to change data type */
    void on_combotype_changed();

    /** When user click on button add for add list treeview */
    void on_buttonadd_clicked();

    /** When user click on button add for edit list treeview */
    void on_buttonedit_clicked();

    /** When user click on button del current element in list treeview */
    void on_buttondel_clicked();

    /** When user select combobox type random to change random type */
    void on_combotyperandom_changed();

    /**
     * A class to define treeview column in init treeview.
     *
     */
class TreeColumnList : public Gtk::TreeModel::ColumnRecord
    {
    public:
        TreeColumnList() {
            add(mType);
            add(mValue);
        }

        Gtk::TreeModelColumn < Glib::ustring > mType;
        Gtk::TreeModelColumn < Glib::ustring > mValue;
    };

    Gtk::VBox           mVBox;
    int                 mWindowWidth;
    int                 mWindowHeight;

    Gtk::Frame                      mFrameType;
    Gtk::HBox                       mHBoxType;
    gvle::ComboBoxString         mComboType;
    Gtk::Label                      mLabelValueType;
    Gtk::Entry                      mEntryValueType;
    Gtk::Frame                      mFrameList;
    Gtk::VBox                       mVBoxList;
    Gtk::ScrolledWindow             mScrolledWindow;
    Gtk::TreeView                   mTreeViewList;
    Glib::RefPtr < Gtk::ListStore > mListStoreList;
    TreeColumnList                  mTreeColumnList;
    Gtk::HBox                       mHBoxButtonList;
    Gtk::Button                     mButtonAddList;
    Gtk::Button                     mButtonEditList;
    Gtk::Button                     mButtonDelList;
    Gtk::Frame                      mFrameRandom;
    Gtk::VBox                       mVBoxRandom;
    gvle::ComboBoxString         mComboTypeRandom;
    Gtk::HBox                       mHBoxInstance;
    Gtk::Label                      mLabelInstance;
    Gtk::Entry                      mInstance;
    Gtk::HBox                       mHBoxSeed;
    Gtk::Label                      mLabelSeed;
    Gtk::Entry                      mSeed;
    Gtk::HBox                       mHBoxAverage;
    Gtk::Label                      mLabelAverage;
    Gtk::Entry                      mAverage;
    Gtk::HBox                       mHBoxStandardDeviation;
    Gtk::Label                      mLabelStandardDeviation;
    Gtk::Entry                      mStandardDeviation;
    Gtk::HBox                       mHBoxMinMax;
    Gtk::Label                      mLabelMin;
    Gtk::Entry                      mMin;
    Gtk::Label                      mLabelMax;
    Gtk::Entry                      mMax;
    Gtk::Button*                    mButtonValid;
    Gtk::Button*                    mButtonCancel;
};

}
} // namespace vle gvle

#endif
