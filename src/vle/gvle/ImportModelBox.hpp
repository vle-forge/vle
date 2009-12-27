/**
 * @file vle/gvle/ImportModelBox.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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


#ifndef GUI_IMPORTMODELBOX_HPP
#define GUI_IMPORTMODELBOX_HPP

#include <vle/graph/Model.hpp>
#include <vle/vpz/Vpz.hpp>
#include <gtkmm.h>
#include <libglademm.h>

namespace vle
{
namespace gvle {

class Modeling;

class ImportModelBox;

class ImportWidget : public Gtk::HBox
{
public:
    ImportWidget(ImportModelBox* parent, vpz::Base* base);
    ImportWidget(ImportModelBox* parent, graph::Model* atom);

    vpz::Base* get_base() {
        return mBase;
    }

    graph::Model* get_model() {
        return mAtom;
    }

    std::string get_text();

    void set_state(bool state);

private:
    ImportModelBox*             mParent;
    vpz::Base*                  mBase;
    graph::Model*               mAtom;

    Gtk::Entry                  mEntry;
    Gtk::Image                  mImage;
    bool                        mState;

    bool is_valid();
    void on_change();
    bool evaluate();
};

class ImportModelBox
{
    typedef std::set<ImportWidget*> list_widget;

public:
    ImportModelBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* modeling);

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

    inline void setGCoupled(graph::CoupledModel* parent)
	{ mParent = parent; }

    inline graph::CoupledModel* getGCoupled()
	{ return mParent; }

private:
    Glib::RefPtr<Gnome::Glade::Xml>      mXml;
    Modeling*                            mModeling;
    vpz::Vpz*                            mSrc;
    graph::CoupledModel*                 mParent;

    Gtk::Dialog*                         mDialog;

    Gtk::VBox*                           mVboxDyns;
    list_widget                          mListDyns;

    Gtk::VBox*                           mVboxConds;
    list_widget                          mListConds;

    Gtk::VBox*                           mVboxOutputs;
    list_widget                          mListOutputs;

    Gtk::VBox*                           mVboxObs;
    list_widget                          mListObs;

    Gtk::VBox*                           mVboxViews;
    list_widget                          mListViews;

    Gtk::VBox*                           mVboxModels;
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

    void rename_dynamic(vpz::Vpz* src, std::string old_name, std::string new_name);
    void rename_condition(vpz::Vpz* src, std::string old_name, std::string new_name);
    void rename_output(vpz::Vpz* src, std::string old_name, std::string new_name);
    void rename_observable(vpz::Vpz* src, std::string old_name, std::string new_name);
    void rename_view(vpz::Vpz* src, std::string old_name, std::string new_name);
    void rename_model(vpz::Vpz* src, graph::Model* old_model, std::string new_name);

    void on_apply();
    void on_cancel();
};

}
} // namespace vle gvle

#endif
