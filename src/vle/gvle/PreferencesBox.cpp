/**
 * @file vle/gvle/AtomicModelBox.hpp
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


#include <vle/gvle/PreferencesBox.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <boost/algorithm/string/split.hpp>

namespace vle { namespace gvle {

PreferencesBox::PreferencesBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* m):
    mXml(xml),
    mModeling(m)
{
    xml->get_widget("DialogPreferences", mDialog);

    xml->get_widget("ButtonPreferencesFgColor", mForegroundColor);
    mForegroundColor->signal_color_set().connect(
	sigc::mem_fun(*this, &PreferencesBox::onButtonForegroundColorChange));
    xml->get_widget("ButtonPreferencesBgColor", mBackgroundColor);
    mBackgroundColor->signal_color_set().connect(
	sigc::mem_fun(*this, &PreferencesBox::onButtonBackgroundColorChange));
    xml->get_widget("ButtonPreferencesAtomicColor", mAtomicColor);
    mAtomicColor->signal_color_set().connect(
	sigc::mem_fun(*this, &PreferencesBox::onButtonAtomicColorChange));
    xml->get_widget("ButtonPreferencesCoupledColor", mCoupledColor);
    mCoupledColor->signal_color_set().connect(
	sigc::mem_fun(*this, &PreferencesBox::onButtonCoupledColorChange));
    xml->get_widget("ButtonPreferencesSelectedColor", mSelectedColor);
    mSelectedColor->signal_color_set().connect(
	sigc::mem_fun(*this, &PreferencesBox::onButtonSelectedColorChange));
    xml->get_widget("ButtonPreferencesSelectFont", mFont);
    mFont->signal_font_set().connect(
	sigc::mem_fun(*this, &PreferencesBox::onButtonFontChange));

    xml->get_widget("ButtonPreferencesApply", mButtonApply);
    mButtonApply->signal_clicked().connect(
	sigc::mem_fun(*this, &PreferencesBox::onApply));
    xml->get_widget("ButtonPreferencesCancel", mButtonCancel);
    mButtonCancel->signal_clicked().connect(
	sigc::mem_fun(*this, &PreferencesBox::onCancel));
    xml->get_widget("ButtonPreferencesRestore", mButtonRestore);
    mButtonRestore->signal_clicked().connect(
	sigc::mem_fun(*this, &PreferencesBox::onRestore));

    init();
}

PreferencesBox::~PreferencesBox()
{
    mDialog->hide_all();
}

void PreferencesBox::show()
{
    mDialog->show_all();
    mDialog->run();
}

void PreferencesBox::onApply()
{
    activate(currentSettings);
    copy(currentSettings, backupSettings);
    mModeling->refreshViews();
    mDialog->hide_all();
}

void PreferencesBox::onCancel()
{
    copy(backupSettings, currentSettings);
    mBackgroundColor->set_color(backupSettings.background);
    mForegroundColor->set_color(backupSettings.foreground);
    mAtomicColor->set_color(backupSettings.atomic);
    mCoupledColor->set_color(backupSettings.coupled);
    mSelectedColor->set_color(backupSettings.selected);
    mFont->set_font_name(backupSettings.font);
    mDialog->hide_all();
}

void PreferencesBox::onRestore()
{
    Gdk::Color color;

    color.set_rgb_p(1.0, 1.0, 1.0);
    currentSettings.background = color;

    color.set_rgb_p(0.0, 0.0, 0.0);
    currentSettings.foreground = color;

    color.set_rgb_p(0.70, 0.83, 0.68);
    currentSettings.atomic = color;

    color.set_rgb_p(0.68, 0.70, 0.83);
    currentSettings.coupled = color;

    color.set_rgb_p(0.83, 0.68, 0.70);
    currentSettings.selected = color;

    currentSettings.font = "Sans";

    mBackgroundColor->set_color(currentSettings.background);
    mForegroundColor->set_color(currentSettings.foreground);
    mAtomicColor->set_color(currentSettings.atomic);
    mCoupledColor->set_color(currentSettings.coupled);
    mSelectedColor->set_color(currentSettings.selected);
    mFont->set_font_name(currentSettings.font);
}

void PreferencesBox::onButtonBackgroundColorChange()
{
    currentSettings.background = mBackgroundColor->get_color();
}

void PreferencesBox::onButtonForegroundColorChange()
{
    currentSettings.foreground = mForegroundColor->get_color();
}

void PreferencesBox::onButtonAtomicColorChange()
{
    currentSettings.atomic = mAtomicColor->get_color();
}

void PreferencesBox::onButtonCoupledColorChange()
{
    currentSettings.coupled = mCoupledColor->get_color();
}

void PreferencesBox::onButtonSelectedColorChange()
{
    currentSettings.selected = mSelectedColor->get_color();
}

void PreferencesBox::onButtonFontChange()
{
    currentSettings.font = mFont->get_font_name();
}

void PreferencesBox::init()
{
    currentSettings.background = mBackgroundColor->get_color();
    currentSettings.foreground = mForegroundColor->get_color();
    currentSettings.atomic = mAtomicColor->get_color();
    currentSettings.coupled = mCoupledColor->get_color();
    currentSettings.selected = mSelectedColor->get_color();
    currentSettings.font = mFont->property_font_name().get_value();
    copy(currentSettings, backupSettings);
    activate(currentSettings);
}

void PreferencesBox::activate(const Settings& settings)
{
    Color color;

    color.m_r = settings.foreground.get_red_p();
    color.m_g = settings.foreground.get_green_p();
    color.m_b = settings.foreground.get_blue_p();
    mModeling->setForegroundColor(color);

    color.m_r = settings.background.get_red_p();
    color.m_g = settings.background.get_green_p();
    color.m_b = settings.background.get_blue_p();
    mModeling->setBackgroundColor(color);

    color.m_r = settings.selected.get_red_p();
    color.m_g = settings.selected.get_green_p();
    color.m_b = settings.selected.get_blue_p();
    mModeling->setSelectedColor(color);

    color.m_r = settings.coupled.get_red_p();
    color.m_g = settings.coupled.get_green_p();
    color.m_b = settings.coupled.get_blue_p();
    mModeling->setCoupledColor(color);

    color.m_r = settings.atomic.get_red_p();
    color.m_g = settings.atomic.get_green_p();
    color.m_b = settings.atomic.get_blue_p();
    mModeling->setAtomicColor(color);

    /**
     * setting.font contains the font name + style + size (ex: "Sans italic 12")
     * but all we need is the name of the font "Sans". So we cut the string
     * to keep only what we need.
     */
    std::vector< std::string > splitVec;
    std::ostringstream oss;
    boost::split(splitVec, settings.font, boost::is_any_of(" "));
    std::copy(splitVec.begin(),
	      splitVec.end()-1,
	      std::ostream_iterator<std::string>(oss, " " ));
    std::string fontName = oss.str();
    mModeling->setFont(fontName);
}

void PreferencesBox::copy(const Settings& src, Settings& dest)
{
    dest.foreground = src.foreground;
    dest.background = src.background;
    dest.selected   = src.selected;
    dest.coupled    = src.coupled;
    dest.atomic     = src.atomic;
    dest.font       = src.font;
}

/*
void PreferencesBox::saveSettings(const std::string& path)
{
}

void PreferencesBox::loadSettings(const std::string& path)
{
}
*/

}} //namespace vle gvle
