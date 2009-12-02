/**
 * @file vle/gvle/PreferencesBox.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace vle { namespace gvle {

PreferencesBox::PreferencesBox(Glib::RefPtr<Gnome::Glade::Xml> xml):
    mXml(xml)
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
    xml->get_widget("ButtonPreferencesConnectionColor", mConnectionColor);
    mConnectionColor->signal_color_set().connect(
        sigc::mem_fun(*this, &PreferencesBox::onButtonConnectionColorChange));
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
}

PreferencesBox::~PreferencesBox()
{
    mDialog->hide_all();
}

int PreferencesBox::run()
{
    init();
    mDialog->show_all();
    return mDialog->run();
}

std::string PreferencesBox::getGraphicsFont()
{
    return mFont->get_font_name();
}

std::string PreferencesBox::getEditorFont()
{
    return mFontEditor->get_font_name();
}

// Actions
void PreferencesBox::onApply()
{
    saveSettings();
    mDialog->hide_all();
    mDialog->response(Gtk::RESPONSE_OK);
}

void PreferencesBox::onCancel()
{
    mDialog->hide_all();
    mDialog->response(Gtk::RESPONSE_CANCEL);
}

void PreferencesBox::onRestore()
{
    mCurrentSettings.setDefault();
    activate();
}

// Graphics
void PreferencesBox::onButtonBackgroundColorChange()
{
    mCurrentSettings.setBackgroundColor(mBackgroundColor->get_color());
}

void PreferencesBox::onButtonForegroundColorChange()
{
    mCurrentSettings.setForegroundColor(mForegroundColor->get_color());
}

void PreferencesBox::onButtonAtomicColorChange()
{
    mCurrentSettings.setAtomicColor(mAtomicColor->get_color());
}

void PreferencesBox::onButtonCoupledColorChange()
{
    mCurrentSettings.setCoupledColor(mCoupledColor->get_color());
}

void PreferencesBox::onButtonSelectedColorChange()
{
    mCurrentSettings.setSelectedColor(mSelectedColor->get_color());
}

void PreferencesBox::onButtonConnectionColorChange()
{
    mCurrentSettings.setConnectionColor(mConnectionColor->get_color());
}

void PreferencesBox::onButtonFontChange()
{
    std::vector< std::string > splitVec;
    mCurrentSettings.setFont(mFont->get_font_name());

    boost::split(splitVec, mCurrentSettings.getFont(), boost::is_any_of(" "));

    mCurrentSettings.setFontSize(boost::lexical_cast<double>(
                                     splitVec[splitVec.size() -1]));
}

void PreferencesBox::onLineWidthChange()
{
    mCurrentSettings.setLineWidth(mLineWidth->get_value());
}

// Editor
void PreferencesBox::onHighlightSyntax()
{
    mCurrentSettings.setHighlightSyntax(mHighlightSyntax->get_active());
}

void PreferencesBox::onHighlightMatchingBrackets()
{
    mCurrentSettings.setHighlightBrackets(
        mHighlightMatchingBrackets->get_active());
}

void PreferencesBox::onHighlightCurrentLine()
{
    mCurrentSettings.setHighlightLine(mHighlightCurrentLine->get_active());
}

void PreferencesBox::onShowLineNumbers()
{
    mCurrentSettings.setLineNumbers(mLineNumbers->get_active());
}

void PreferencesBox::onShowRightMargin()
{
    mCurrentSettings.setRightMargin(mRightMargin->get_active());
}

void PreferencesBox::onAutoIndent()
{
    mCurrentSettings.setAutoIndent(mAutoIndent->get_active());
}

void PreferencesBox::onIndentOnTab()
{
    mCurrentSettings.setIndentOnTab(mIndentOnTab->get_active());
}

void PreferencesBox::onIndentSizeChange()
{
    mCurrentSettings.setIndentSize(mIndentSize->get_value_as_int());
}

void PreferencesBox::onSmartHomeEnd()
{
    mCurrentSettings.setSmartHomeEnd(mSmartHomeEnd->get_active());
}

void PreferencesBox::onEditorFontChange()
{
    mCurrentSettings.setFontEditor(mFontEditor->get_font_name());
}

// private
void PreferencesBox::init()
{
    loadSettings();
    activate();
}

void PreferencesBox::activate()
{
    // Graphics
    mBackgroundColor->set_color(mCurrentSettings.getBackgroundColor());
    mForegroundColor->set_color(mCurrentSettings.getForegroundColor());
    mAtomicColor->set_color(mCurrentSettings.getAtomicColor());
    mCoupledColor->set_color(mCurrentSettings.getCoupledColor());
    mSelectedColor->set_color(mCurrentSettings.getSelectedColor());
    mConnectionColor->set_color(mCurrentSettings.getConnectionColor());
    mFont->set_font_name(mCurrentSettings.getFont());
    mLineWidth->set_value(mCurrentSettings.getLineWidth());

    // Editor
    mHighlightSyntax->set_active(mCurrentSettings.getHighlightSyntax());
    mHighlightMatchingBrackets->set_active(
        mCurrentSettings.getHighlightBrackets());
    mHighlightCurrentLine->set_active(mCurrentSettings.getHighlightLine());
    mLineNumbers->set_active(mCurrentSettings.getLineNumbers());
    mRightMargin->set_active(mCurrentSettings.getRightMargin());
    mAutoIndent->set_active(mCurrentSettings.getAutoIndent());
    mIndentOnTab->set_active(mCurrentSettings.getIndentOnTab());
    mIndentSize->set_value(mCurrentSettings.getIndentSize());
    mSmartHomeEnd->set_active(mCurrentSettings.getSmartHomeEnd());
    mFontEditor->set_font_name(mCurrentSettings.getFontEditor());
}

void PreferencesBox::saveSettings()
{
    Settings::settings() = mCurrentSettings;
    Settings::settings().save();
}

void PreferencesBox::loadSettings()
{
    Settings::settings().load();
    mCurrentSettings = Settings::settings();
}

}} //namespace vle gvle
