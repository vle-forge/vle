/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/gvle/PreferencesBox.hpp>
#include <vle/gvle/Settings.hpp>
#include <gtkmm/dialog.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/colorbutton.h>
#include <gtkmm/fontbutton.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/scale.h>

#define MIN_LINE_WIDTH 1
#define MAX_LINE_WIDTH 10

namespace vle { namespace gvle {

class PreferencesBox::Pimpl
{
public:
  Pimpl(Glib::RefPtr < Gtk::Builder > xml)
        : mXml(xml)
    {
        xml->get_widget("DialogPreferences", mDialog);

        xml->get_widget("ButtonPreferencesFgColor", mForegroundColor);
        xml->get_widget("ButtonPreferencesBgColor", mBackgroundColor);
        xml->get_widget("ButtonPreferencesAtomicColor", mAtomicColor);
        xml->get_widget("ButtonPreferencesCoupledColor", mCoupledColor);
        xml->get_widget("ButtonPreferencesSelectedColor", mSelectedColor);
        xml->get_widget("ButtonPreferencesConnectionColor", mConnectionColor);
        xml->get_widget("ButtonPreferencesSelectFont", mFont);
        xml->get_widget("HScalePreferencesLineWidth", mLineWidth);

        mLineWidth->set_range(MIN_LINE_WIDTH, MAX_LINE_WIDTH);

        xml->get_widget("CheckPreferencesHighlightSyntax", mHighlightSyntax);
        xml->get_widget("CheckPreferencesHighlightMatchingBrackets",
                        mHighlightMatchingBrackets);
        xml->get_widget("CheckPreferencesHighlightCurrentLine",
                        mHighlightCurrentLine);
        xml->get_widget("CheckPreferencesLineNumbers", mLineNumbers);
        xml->get_widget("CheckPreferencesRightMargin", mRightMargin);
        xml->get_widget("CheckPreferencesAutoIndent", mAutoIndent);
        xml->get_widget("CheckPreferencesIndentOnTab", mIndentOnTab);
        xml->get_widget("SpinButtonPreferencesIndentSize", mIndentSize);
        xml->get_widget("CheckPreferencesSmartHomeEnd", mSmartHomeEnd);
        xml->get_widget("FontButtonPreferencesEditor", mFontEditor);
        xml->get_widget("ButtonPreferencesApply", mButtonApply);
        xml->get_widget("ButtonPreferencesCancel", mButtonCancel);
        xml->get_widget("ButtonPreferencesRestore", mButtonRestore);

        mSigcConnections.push_back(mButtonApply->signal_clicked().connect(
            sigc::mem_fun(*this, &PreferencesBox::Pimpl::onApply)));

        mSigcConnections.push_back(mButtonCancel->signal_clicked().connect(
            sigc::mem_fun(*this, &PreferencesBox::Pimpl::onCancel)));

        mSigcConnections.push_back(mButtonRestore->signal_clicked().connect(
                sigc::mem_fun(*this, &PreferencesBox::Pimpl::onRestore)));
    }

    ~Pimpl()
    {
        std::for_each(mSigcConnections.begin(), mSigcConnections.end(),
                      DisconnectSignal());

        if (mDialog) {
            mDialog->hide();
        }
    }

    struct DisconnectSignal {
        void operator()(std::list < sigc::connection >::value_type& cnt) {
            cnt.disconnect();
        }
    };

    int run()
    {
        loadSettings();

        mDialog->show_all();
        int response = mDialog->run();
        mDialog->hide();
        return response;
    }

    void onApply()
    {
        saveSettings();

        mDialog->response(Gtk::RESPONSE_OK);
    }

    void onCancel()
    {
        mDialog->response(Gtk::RESPONSE_CANCEL);
    }

    void onRestore()
    {
        Settings::settings().setDefault();
        Settings::settings().save();

        mDialog->response(Gtk::RESPONSE_OK);
    }

    void loadSettings()
    {
        Settings::settings().load();
        updateWidgets();
    }

    void saveSettings()
    {
        Settings::settings().setBackgroundColor(mBackgroundColor->get_color());
        Settings::settings().setForegroundColor(mForegroundColor->get_color());
        Settings::settings().setAtomicColor(mAtomicColor->get_color());
        Settings::settings().setCoupledColor(mCoupledColor->get_color());
        Settings::settings().setSelectedColor(mSelectedColor->get_color());
        Settings::settings().setConnectionColor(mConnectionColor->get_color());
        Settings::settings().setFont(mFont->get_font_name());
        Settings::settings().setLineWidth(mLineWidth->get_value());

        Settings::settings().setHighlightSyntax(
            mHighlightSyntax->get_active());
        Settings::settings().setHighlightBrackets(
            mHighlightMatchingBrackets->get_active());
        Settings::settings().setHighlightLine(
            mHighlightCurrentLine->get_active());
        Settings::settings().setLineNumbers(mLineNumbers->get_active());
        Settings::settings().setRightMargin(mRightMargin->get_active());
        Settings::settings().setAutoIndent(mAutoIndent->get_active());
        Settings::settings().setIndentOnTab(mIndentOnTab->get_active());
        Settings::settings().setIndentSize(mIndentSize->get_value_as_int());
        Settings::settings().setSmartHomeEnd(mSmartHomeEnd->get_active());
        Settings::settings().setFontEditor(mFontEditor->get_font_name());

        Settings::settings().save();
    }

    void updateWidgets()
    {
        mBackgroundColor->set_color(Settings::settings().getBackgroundColor());
        mForegroundColor->set_color(Settings::settings().getForegroundColor());
        mAtomicColor->set_color(Settings::settings().getAtomicColor());
        mCoupledColor->set_color(Settings::settings().getCoupledColor());
        mSelectedColor->set_color(Settings::settings().getSelectedColor());
        mConnectionColor->set_color(Settings::settings().getConnectionColor());
        mFont->set_font_name(Settings::settings().getFont());
        mLineWidth->set_value(Settings::settings().getLineWidth());

        mHighlightSyntax->set_active(
            Settings::settings().getHighlightSyntax());
        mHighlightMatchingBrackets->set_active(
            Settings::settings().getHighlightBrackets());
        mHighlightCurrentLine->set_active(
            Settings::settings().getHighlightLine());
        mLineNumbers->set_active(Settings::settings().getLineNumbers());
        mRightMargin->set_active(Settings::settings().getRightMargin());
        mAutoIndent->set_active(Settings::settings().getAutoIndent());
        mIndentOnTab->set_active(Settings::settings().getIndentOnTab());
        mIndentSize->set_value(Settings::settings().getIndentSize());
        mSmartHomeEnd->set_active(Settings::settings().getSmartHomeEnd());
        mFontEditor->set_font_name(Settings::settings().getFontEditor());
    }

    Glib::RefPtr < Gtk::Builder >   mXml;
    Gtk::Dialog*                    mDialog;

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

    std::list < sigc::connection > mSigcConnections;
};

PreferencesBox::PreferencesBox(Glib::RefPtr < Gtk::Builder >& xml)
  : mPimpl(new PreferencesBox::Pimpl(xml))
{
}

PreferencesBox::~PreferencesBox()
{
    delete mPimpl;
}

int PreferencesBox::run()
{
    return mPimpl->run();
}

}} //namespace vle gvle
