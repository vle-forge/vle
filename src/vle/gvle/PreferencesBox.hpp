/**
 * @file vle/gvle/PreferencesBox.hpp
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


#ifndef VLE_GVLE_PREFERENCESBOX_HPP
#define VLE_GVLE_PREFERENCESBOX_HPP

#include <gtkmm.h>
#include <vle/gvle/Modeling.hpp>
#include <gdkmm/gc.h>
#include <gdkmm/types.h>

namespace vle {
namespace gvle {

class PreferencesBox
{
public:
    PreferencesBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* m);
    virtual ~PreferencesBox();

    void show();

protected:
    virtual void onApply();
    virtual void onCancel();
    virtual void onRestore();

    virtual void onButtonBackgroundColorChange();
    virtual void onButtonForegroundColorChange();
    virtual void onButtonAtomicColorChange();
    virtual void onButtonCoupledColorChange();
    virtual void onButtonSelectedColorChange();

    virtual void onButtonFontChange();

private:
    /**
     * A struct to store the configuration
     */
    struct Settings {
	Gdk::Color background;
	Gdk::Color foreground;
	Gdk::Color atomic;
	Gdk::Color coupled;
	Gdk::Color selected;
	std::string font;
    };

    //Class members
    Glib::RefPtr<Gnome::Glade::Xml> mXml;
    Gtk::Dialog* mDialog;
    Modeling*    mModeling;
    Settings     currentSettings;
    Settings     backupSettings;

    //Dialog widgets
    Gtk::Button* mButtonApply;
    Gtk::Button* mButtonCancel;
    Gtk::Button* mButtonRestore;
    Gtk::ColorButton* mBackgroundColor;
    Gtk::ColorButton* mForegroundColor;
    Gtk::ColorButton* mAtomicColor;
    Gtk::ColorButton* mCoupledColor;
    Gtk::ColorButton* mSelectedColor;
    Gtk::FontButton* mFont;

    //Methods
    void init();
    void activate(const Settings& set);
    void copy(const Settings& src, Settings& dest);
    Gdk::Color makeColorFromString(const std::string& value);
    std::string makeStringFromColor(const Gdk::Color& color);
    void saveSettings();
    void loadSettings();
};

}} //namespace vle gvle
#endif
