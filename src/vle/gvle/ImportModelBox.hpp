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


#ifndef GUI_IMPORTMODELBOX_HPP
#define GUI_IMPORTMODELBOX_HPP

#include <vle/vpz/BaseModel.hpp>
#include <vle/vpz/Vpz.hpp>
#include <gtkmm/builder.h>
#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/dialog.h>
#include <gtkmm/button.h>
#include <gtkmm/image.h>

namespace vle
{
namespace gvle {

class Modeling;

class ImportModelBox;

class ImportWidget : public Gtk::Box
{
public:
    ImportWidget(ImportModelBox* parent, vpz::Base* base);
    ImportWidget(ImportModelBox* parent, vpz::BaseModel* atom);

    vpz::Base* get_base() {
        return mBase;
    }

    vpz::BaseModel* get_model() {
        return mAtom;
    }

    std::string get_text();

    void set_state(bool state);

private:
    ImportModelBox*             mParent;
    vpz::Base*                  mBase;
    vpz::BaseModel*               mAtom;

    Gtk::Entry                  mEntry;
    Gtk::Image                  mImage;
    bool                        mState;

    void on_change();
    bool evaluate();
};

class ImportModelBox
{
    typedef std::set<ImportWidget*> list_widget;

public:
    ImportModelBox(const Glib::RefPtr < Gtk::Builder >& xml,
                   Modeling* modeling);

    bool show(vpz::Vpz* src);

    bool is_valid_dynamic(std::string dynamic);
    bool is_valid_condition(std::string condition);
    bool is_valid_output(std::string output);
    bool is_valid_observable(std::string observable);
    bool is_valid_view(std::string view);
    bool is_valid_model(std::string model);

    const int& get_nb_widget_incorrect() {
        return mNbWidgetIncorrect;
    }

    void dec_nb_widget_incorrect();
    void inc_nb_widget_incorrect();

    inline void setGCoupled(vpz::CoupledModel* parent)
	{ mParent = parent; }

    inline vpz::CoupledModel* getGCoupled()
	{ return mParent; }

private:
    Glib::RefPtr < Gtk::Builder >      mXml;
    Modeling*                            mModeling;
    vpz::Vpz*                            mSrc;
    vpz::CoupledModel*                 mParent;

    Gtk::Dialog*                         mDialog;

    Gtk::Box*                           mVboxDyns;
    list_widget                          mListDyns;

    Gtk::Box*                           mVboxConds;
    list_widget                          mListConds;

    Gtk::Box*                           mVboxOutputs;
    list_widget                          mListOutputs;

    Gtk::Box*                           mVboxObs;
    list_widget                          mListObs;

    Gtk::Box*                           mVboxViews;
    list_widget                          mListViews;

    Gtk::Box*                           mVboxModels;
    list_widget                          mListModels;


    Gtk::Button*                         mButtonApply;
    Gtk::Button*                         mButtonCancel;

    int                                  mNbWidgetIncorrect;
    bool                                 mReturn;

    void makeDynamics();
    void makeConditions();
    void makeOutputs();
    void makeObservables();
    void makeViews();
    void makeModels();

    void rename_dynamic(vpz::Vpz* src, std::string old_name,
                        std::string new_name);
    void rename_condition(vpz::Vpz* src, std::string old_name,
                          std::string new_name);
    void rename_output(vpz::Vpz* src, std::string old_name,
                       std::string new_name);
    void rename_observable(vpz::Vpz* src, std::string old_name,
                           std::string new_name);
    void rename_view(vpz::Vpz* src, std::string old_name, std::string new_name);
    void rename_model(vpz::BaseModel* old_model,
                      std::string new_name);

    void on_apply();
    void on_cancel();
};

}
} // namespace vle gvle

#endif
