/*
 * @file vle/gvle/PreferencesBox.hpp
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


#ifndef VLE_GVLE_PREFERENCESBOX_HPP
#define VLE_GVLE_PREFERENCESBOX_HPP

#include <gtkmm.h>
#include <libglademm.h>
#include <vle/gvle/Settings.hpp>

namespace vle { namespace gvle {

class PreferencesBox
{
public:
    PreferencesBox(Glib::RefPtr<Gnome::Glade::Xml> xml);
    ~PreferencesBox();

    int run();

    std::string getGraphicsFont();
    std::string getEditorFont();

protected:
    // Actions
    void onApply();
    void onCancel();
    void onRestore();

    // Graphics
    void onButtonBackgroundColorChange();
    void onButtonForegroundColorChange();
    void onButtonAtomicColorChange();
    void onButtonCoupledColorChange();
    void onButtonSelectedColorChange();
    void onButtonConnectionColorChange();
    void onButtonFontChange();
    void onLineWidthChange();

    // Editor
    void onHighlightSyntax();
    void onHighlightMatchingBrackets();
    void onHighlightCurrentLine();
    void onShowLineNumbers();
    void onShowRightMargin();
    void onAutoIndent();
    void onIndentOnTab();
    void onIndentSizeChange();
    void onSmartHomeEnd();
    void onEditorFontChange();

private:
    void init();
    void activate();
    void saveSettings();
    void loadSettings();

    Glib::RefPtr<Gnome::Glade::Xml> mXml;
    Gtk::Dialog*                    mDialog;
    Settings                        mCurrentSettings;

    //Dialog widgets - Action
    Gtk::Button* mButtonApply;
    Gtk::Button* mButtonCancel;
    Gtk::Button* mButtonRestore;

    //Dialog widgets - Graphics
    Gtk::ColorButton* mBackgroundColor;
    Gtk::ColorButton* mForegroundColor;
    Gtk::ColorButton* mAtomicColor;
    Gtk::ColorButton* mCoupledColor;
    Gtk::ColorButton* mSelectedColor;
    Gtk::ColorButton* mConnectionColor;
    Gtk::FontButton*  mFont;
    Gtk::HScale*      mLineWidth;

    //Dialog widgets - Editor
    Gtk::CheckButton* mHighlightSyntax;
    Gtk::CheckButton* mHighlightMatchingBrackets;
    Gtk::CheckButton* mHighlightCurrentLine;
    Gtk::CheckButton* mLineNumbers;
    Gtk::CheckButton* mRightMargin;
    Gtk::CheckButton* mAutoIndent;
    Gtk::CheckButton* mIndentOnTab;
    Gtk::SpinButton*  mIndentSize;
    Gtk::CheckButton* mSmartHomeEnd;
    Gtk::FontButton*  mFontEditor;

};

}} //namespace vle gvle

#endif
