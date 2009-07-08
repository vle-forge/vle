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
#include <vle/gvle/Editor.hpp>
#include <vle/utils/Preferences.hpp>
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

    // Graphics tab
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
    xml->get_widget("HScalePreferencesLineWidth", mLineWidth);
    mLineWidth->signal_value_changed().connect(
	sigc::mem_fun(*this, &PreferencesBox::onLineWidthChange));

    // Editor tab
    xml->get_widget("CheckPreferencesHighlightSyntax", mHighlightSyntax);
    mHighlightSyntax->signal_clicked().connect(
	sigc::mem_fun(*this, &PreferencesBox::onHighlightSyntax));
    xml->get_widget("CheckPreferencesHighlightMatchingBrackets", mHighlightMatchingBrackets);
    mHighlightMatchingBrackets->signal_clicked().connect(
	sigc::mem_fun(*this, &PreferencesBox::onHighlightMatchingBrackets));
    xml->get_widget("CheckPreferencesHighlightCurrentLine", mHighlightCurrentLine);
    mHighlightCurrentLine->signal_clicked().connect(
	sigc::mem_fun(*this, &PreferencesBox::onHighlightCurrentLine));
    xml->get_widget("CheckPreferencesLineNumbers", mLineNumbers);
    mLineNumbers->signal_clicked().connect(
	sigc::mem_fun(*this, &PreferencesBox::onShowLineNumbers));
    xml->get_widget("CheckPreferencesRightMargin", mRightMargin);
    mRightMargin->signal_clicked().connect(
	sigc::mem_fun(*this, &PreferencesBox::onShowRightMargin));
    xml->get_widget("CheckPreferencesAutoIndent", mAutoIndent);
    mAutoIndent->signal_clicked().connect(
	sigc::mem_fun(*this, &PreferencesBox::onAutoIndent));
    xml->get_widget("CheckPreferencesIndentOnTab", mIndentOnTab);
    mIndentOnTab->signal_clicked().connect(
	sigc::mem_fun(*this, &PreferencesBox::onIndentOnTab));
    xml->get_widget("SpinButtonPreferencesIndentSize", mIndentSize);
    mIndentSize->signal_value_changed().connect(
	sigc::mem_fun(*this, &PreferencesBox::onIndentSizeChange));
    xml->get_widget("CheckPreferencesSmartHomeEnd", mSmartHomeEnd);
    mSmartHomeEnd->signal_clicked().connect(
	sigc::mem_fun(*this, &PreferencesBox::onSmartHomeEnd));
    xml->get_widget("FontButtonPreferencesEditor", mFontEditor);
    mFontEditor->signal_font_set().connect(
	sigc::mem_fun(*this, &PreferencesBox::onEditorFontChange));

    // Action area
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

// Actions
void PreferencesBox::onApply()
{
    activate(currentSettings);
    saveSettings();
    copy(currentSettings, backupSettings);
    mModeling->refreshViews();
    mModeling->getGVLE()->getEditor()->refreshViews();
    mDialog->hide_all();
}

void PreferencesBox::onCancel()
{
    copy(backupSettings, currentSettings);
    // Graphics
    mBackgroundColor->set_color(backupSettings.background);
    mForegroundColor->set_color(backupSettings.foreground);
    mAtomicColor->set_color(backupSettings.atomic);
    mCoupledColor->set_color(backupSettings.coupled);
    mSelectedColor->set_color(backupSettings.selected);
    std::ostringstream oss;
    oss << backupSettings.font.c_str() << " " << backupSettings.fontSize;
    mFont->set_font_name(oss.str());
    mLineWidth->set_value(backupSettings.lineWidth);
    //Editor
    mHighlightSyntax->set_active(backupSettings.highlightSyntax);
    mHighlightMatchingBrackets->set_active(backupSettings.highlightBrackets);
    mHighlightCurrentLine->set_active(backupSettings.highlightLine);
    mLineNumbers->set_active(backupSettings.lineNumbers);
    mRightMargin->set_active(backupSettings.rightMargin);
    mAutoIndent->set_active(backupSettings.autoIndent);
    mIndentOnTab->set_active(backupSettings.indentOnTab);
    mIndentSize->set_value(backupSettings.indentSize);
    mSmartHomeEnd->set_active(backupSettings.smartHomeEnd);
    mFontEditor->set_font_name(backupSettings.fontEditor);

    mDialog->hide_all();
}

void PreferencesBox::onRestore()
{
    init();
}

// Graphics
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
    /**
     * get_font_name() returns the font name + style + size (ex: "Sans italic 12")
     * but all we need is the name of the font "Sans". So we cut the string
     * to keep only what we need.
     */
    std::vector< std::string > splitVec;
    std::ostringstream oss;
    currentSettings.font = mFont->get_font_name();

    boost::split(splitVec, currentSettings.font, boost::is_any_of(" "));
    std::copy(splitVec.begin(),
	      splitVec.end()-1,
	      std::ostream_iterator<std::string>(oss, " " ));

    currentSettings.font = oss.str();
    currentSettings.fontSize = boost::lexical_cast<double>(
	splitVec[splitVec.size() -1]);
}

void PreferencesBox::onLineWidthChange()
{
    currentSettings.lineWidth = mLineWidth->get_value();
}

// Editor
void PreferencesBox::onHighlightSyntax()
{
    currentSettings.highlightSyntax = mHighlightSyntax->get_active();
}

void PreferencesBox::onHighlightMatchingBrackets()
{
    currentSettings.highlightBrackets =
	mHighlightMatchingBrackets->get_active();
}

void PreferencesBox::onHighlightCurrentLine()
{
    currentSettings.highlightLine = mHighlightCurrentLine->get_active();
}

void PreferencesBox::onShowLineNumbers()
{
    currentSettings.lineNumbers = mLineNumbers->get_active();
}

void PreferencesBox::onShowRightMargin()
{
    currentSettings.rightMargin = mRightMargin->get_active();
}

void PreferencesBox::onAutoIndent()
{
    currentSettings.autoIndent = mAutoIndent->get_active();
}

void PreferencesBox::onIndentOnTab()
{
    currentSettings.indentOnTab = mIndentOnTab->get_active();
}

void PreferencesBox::onIndentSizeChange()
{
    currentSettings.indentSize = mIndentSize->get_value_as_int();
}

void PreferencesBox::onSmartHomeEnd()
{
    currentSettings.smartHomeEnd = mSmartHomeEnd->get_active();
}

void PreferencesBox::onEditorFontChange()
{
    currentSettings.fontEditor = mFontEditor->get_font_name();
}

// private
void PreferencesBox::init()
{
    loadSettings();
    activate(currentSettings);
}

void PreferencesBox::activate(const Settings& settings)
{
    // Graphics
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

    mModeling->setFont(settings.font);
    mModeling->setFontSize(settings.fontSize);

    mModeling->setLineWidth(settings.lineWidth);

    mBackgroundColor->set_color(settings.background);
    mForegroundColor->set_color(settings.foreground);
    mAtomicColor->set_color(settings.atomic);
    mCoupledColor->set_color(settings.coupled);
    mSelectedColor->set_color(settings.selected);
    std::ostringstream oss;
    oss << settings.font.c_str() << " " << settings.fontSize;
    mFont->set_font_name(oss.str());
    mLineWidth->set_value(settings.lineWidth);

    // Editor
    mHighlightSyntax->set_active(settings.highlightSyntax);
    mModeling->setHighlightSyntax(settings.highlightSyntax);

    mHighlightMatchingBrackets->set_active(settings.highlightBrackets);
    mModeling->setHighlightBrackets(settings.highlightBrackets);

    mHighlightCurrentLine->set_active(settings.highlightLine);
    mModeling->setHighlightLine(settings.highlightLine);

    mLineNumbers->set_active(settings.lineNumbers);
    mModeling->setLineNumbers(settings.lineNumbers);

    mRightMargin->set_active(settings.rightMargin);
    mModeling->setRightMargin(settings.rightMargin);

    mAutoIndent->set_active(settings.autoIndent);
    mModeling->setAutoIndent(settings.autoIndent);

    mIndentOnTab->set_active(settings.indentOnTab);
    mModeling->setIndentOnTab(settings.indentOnTab);

    mIndentSize->set_value(settings.indentSize);
    mModeling->setIndentSize(settings.indentSize);

    mSmartHomeEnd->set_active(settings.smartHomeEnd);
    mModeling->setSmartHomeEnd(settings.smartHomeEnd);

    mFontEditor->set_font_name(settings.fontEditor);
    mModeling->setFontEditor(settings.fontEditor);
}

void PreferencesBox::copy(const Settings& src, Settings& dest)
{
    // Graphics
    dest.foreground = src.foreground;
    dest.background = src.background;
    dest.selected   = src.selected;
    dest.coupled    = src.coupled;
    dest.atomic     = src.atomic;
    dest.font       = src.font;
    dest.fontSize   = src.fontSize;
    dest.lineWidth  = src.lineWidth;

    // Editor
    dest.highlightSyntax   = src.highlightSyntax;
    dest.highlightBrackets = src.highlightBrackets;
    dest.highlightLine     = src.highlightLine;
    dest.lineNumbers       = src.lineNumbers;
    dest.rightMargin       = src.rightMargin;
    dest.autoIndent        = src.autoIndent;
    dest.indentOnTab       = src.indentOnTab;
    dest.indentSize        = src.indentSize;
    dest.smartHomeEnd      = src.smartHomeEnd;
    dest.fontEditor        = src.fontEditor;
}


void PreferencesBox::saveSettings()
{
    vle::utils::Preferences prefs;
    prefs.load();

    // Graphics
    prefs.setAttributes("gvle.graphics",
			"foreground",
			makeStringFromColor(currentSettings.foreground));
    prefs.setAttributes("gvle.graphics",
			"background",
			makeStringFromColor(currentSettings.background));
    prefs.setAttributes("gvle.graphics",
			"atomic",
			makeStringFromColor(currentSettings.atomic));
    prefs.setAttributes("gvle.graphics",
			"coupled",
			makeStringFromColor(currentSettings.coupled));
    prefs.setAttributes("gvle.graphics",
			"selected",
			makeStringFromColor(currentSettings.selected));
    prefs.setAttributes("gvle.graphics",
			"font",
			currentSettings.font);
    std::ostringstream oss;
    oss << currentSettings.fontSize;
    prefs.setAttributes("gvle.graphics", "fontSize", oss.str());

    oss.str("");
    oss << currentSettings.lineWidth;
    prefs.setAttributes("gvle.graphics", "lineWidth", oss.str());

    // Editor
    prefs.setAttributes("gvle.editor",
			"highlightSyntax",
			currentSettings.highlightSyntax ? "1" : "0");
    prefs.setAttributes("gvle.editor",
			"highlightBrackets",
			currentSettings.highlightBrackets ? "1" : "0");
    prefs.setAttributes("gvle.editor",
			"highlightLine",
			currentSettings.highlightLine ? "1" : "0");
    prefs.setAttributes("gvle.editor",
			"lineNumbers",
			currentSettings.lineNumbers ? "1" : "0");
    prefs.setAttributes("gvle.editor",
			"rightMargin",
			currentSettings.rightMargin ? "1" : "0");
    prefs.setAttributes("gvle.editor",
			"autoIndent",
			currentSettings.autoIndent ? "1" : "0");
    prefs.setAttributes("gvle.editor",
			"indentOnTab",
			currentSettings.indentOnTab ? "1" : "0");
    prefs.setAttributes("gvle.editor",
			"indentSize",
			boost::lexical_cast<std::string>(currentSettings.indentSize));
    prefs.setAttributes("gvle.editor",
			"smartHomeEnd",
			currentSettings.smartHomeEnd ? "1" : "0");
    prefs.setAttributes("gvle.editor",
			"fontEditor",
			currentSettings.fontEditor);
    prefs.save();
}


void PreferencesBox::loadSettings()
{
    vle::utils::Preferences prefs;
    std::string value;

    prefs.load();

    // Graphics
    value = prefs.getAttributes("gvle.graphics", "background");
    currentSettings.background = (value.empty() ? mBackgroundColor->get_color()
				  : makeColorFromString(value));

    value = prefs.getAttributes("gvle.graphics", "foreground");
    currentSettings.foreground = (value.empty()? mForegroundColor->get_color()
				  : makeColorFromString(value));

    value = prefs.getAttributes("gvle.graphics", "atomic");
    currentSettings.atomic = (value.empty() ? mAtomicColor->get_color()
			      : makeColorFromString(value));

    value = prefs.getAttributes("gvle.graphics", "coupled");
    currentSettings.coupled = (value.empty() ? mCoupledColor->get_color()
			       : makeColorFromString(value));

    value = prefs.getAttributes("gvle.graphics", "selected");
    currentSettings.selected = (value.empty() ? mSelectedColor->get_color()
				: makeColorFromString(value));

    value = prefs.getAttributes("gvle.graphics", "font");
    currentSettings.font = (value.empty() ? std::string(mFont->get_font_name())
			    : value);

    value = prefs.getAttributes("gvle.graphics", "fontSize");
    currentSettings.fontSize = (value.empty() ? 10.0
			    : boost::lexical_cast<double>(value));

    value = prefs.getAttributes("gvle.graphics", "lineWidth");
    currentSettings.lineWidth = (value.empty() ? mLineWidth->get_value()
			    : boost::lexical_cast<double>(value));

    // Editor
    value = prefs.getAttributes("gvle.editor", "highlightSyntax");
    currentSettings.highlightSyntax =
	(value.empty() ? mHighlightSyntax->get_active()
	 : boost::lexical_cast<bool>(value));
    value = prefs.getAttributes("gvle.editor", "highlightBrackets");
    currentSettings.highlightBrackets =
	(value.empty() ? mHighlightMatchingBrackets->get_active()
	 : boost::lexical_cast<bool>(value));
    value = prefs.getAttributes("gvle.editor", "highlightLine");
    currentSettings.highlightLine =
	(value.empty() ? mHighlightCurrentLine->get_active()
	 : boost::lexical_cast<bool>(value));
    value = prefs.getAttributes("gvle.editor", "lineNumbers");
    currentSettings.lineNumbers =
	(value.empty() ? mLineNumbers->get_active()
	 : boost::lexical_cast<bool>(value));
    value = prefs.getAttributes("gvle.editor", "rightMargin");
    currentSettings.rightMargin =
	(value.empty() ? mRightMargin->get_active()
	 : boost::lexical_cast<bool>(value));
    value = prefs.getAttributes("gvle.editor", "autoIndent");
    currentSettings.autoIndent =
	(value.empty() ? mAutoIndent->get_active()
	 : boost::lexical_cast<bool>(value));
    value = prefs.getAttributes("gvle.editor", "indentOnTab");
    currentSettings.indentOnTab =
	(value.empty() ? mIndentOnTab->get_active()
	 : boost::lexical_cast<bool>(value));
    value = prefs.getAttributes("gvle.editor", "indentSize");
    currentSettings.indentSize =
	(value.empty() ? mIndentSize->get_value_as_int()
	 : boost::lexical_cast<int>(value));
    value = prefs.getAttributes("gvle.editor", "smartHomeEnd");
    currentSettings.smartHomeEnd =
	(value.empty() ? mSmartHomeEnd->get_active()
	 : boost::lexical_cast<bool>(value));
    value = prefs.getAttributes("gvle.editor", "fontEditor");
    currentSettings.fontEditor =
	(value.empty() ? std::string(mFontEditor->get_font_name()) : value);

    copy(currentSettings, backupSettings);
}

Gdk::Color PreferencesBox::makeColorFromString(const std::string& value)
{
    std::vector< std::string > splitVec;
    boost::split(splitVec, value, boost::is_any_of(" "));
    double r = boost::lexical_cast<double>(splitVec[0]);
    double g = boost::lexical_cast<double>(splitVec[1]);
    double b = boost::lexical_cast<double>(splitVec[2]);
    Gdk::Color color;
    color.set_rgb_p(r, g, b);
    return color;
}

std::string PreferencesBox::makeStringFromColor(const Gdk::Color& color)
{
    std::ostringstream oss;
    oss << color.get_red_p() << " "
	<< color.get_green_p() << " "
	<< color.get_blue_p();
    return oss.str();
}

}} //namespace vle gvle
