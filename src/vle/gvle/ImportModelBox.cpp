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


#include <vle/gvle/ImportModelBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/Vpz.hpp>
#include <gtkmm/stock.h>
#include <boost/algorithm/string/trim.hpp>

namespace vle
{
namespace gvle {
using namespace vpz;

ImportWidget::ImportWidget(ImportModelBox* parent, vpz::Base* base):
        Gtk::Box(Gtk::ORIENTATION_HORIZONTAL),
        mParent(parent),
        mBase(base),
        mAtom(0)
{
    switch (base->getType()) {
    case Base::VLE_VPZ_DYNAMIC:
        mEntry.set_text(dynamic_cast<vpz::Dynamic*>(base)->name());
        break;
    case Base::VLE_VPZ_CONDITION:
        mEntry.set_text(dynamic_cast<vpz::Condition*>(base)->name());
        break;
    case Base::VLE_VPZ_OUTPUT:
        mEntry.set_text(dynamic_cast<vpz::Output*>(base)->name());
        break;
    case Base::VLE_VPZ_OBSERVABLE:
        mEntry.set_text(dynamic_cast<vpz::Observable*>(base)->name());
        break;
    case Base::VLE_VPZ_VIEW:
        mEntry.set_text(dynamic_cast<vpz::View*>(base)->name());
        break;
    default:
        break;
    }
    mEntry.signal_changed().connect(
        sigc::mem_fun(*this, &ImportWidget::on_change));
    pack_start(mEntry);

    set_state(evaluate());
    if (!mState)
        parent->inc_nb_widget_incorrect();
    pack_start(mImage);
}

ImportWidget::ImportWidget(ImportModelBox* parent, vpz::BaseModel* atom):
        Gtk::Box(Gtk::ORIENTATION_HORIZONTAL),
        mParent(parent),
        mBase(0),
        mAtom(atom)
{
    mEntry.set_text(mAtom->getName());
    mEntry.signal_changed().connect(
        sigc::mem_fun(*this, &ImportWidget::on_change));
    pack_start(mEntry);
    set_state(evaluate());
    if (!mState)
        parent->inc_nb_widget_incorrect();
    pack_start(mImage);
}

std::string ImportWidget::get_text()
{
    return mEntry.get_text();
}

void ImportWidget::set_state(bool state)
{
    mState = state;
    if (state)
        mImage.set(Gtk::StockID(Gtk::Stock::YES), Gtk::IconSize(1));
    else
        mImage.set(Gtk::StockID(Gtk::Stock::NO), Gtk::IconSize(1));
}

void ImportWidget::on_change()
{
    bool new_state = evaluate();
    if (mState != new_state) {
        if (new_state) {
            mParent->dec_nb_widget_incorrect();
        } else {
            mParent->inc_nb_widget_incorrect();
        }
    }
    set_state(new_state);
}

bool ImportWidget::evaluate()
{
    if (mBase) {
        switch (mBase->getType()) {
        case Base::VLE_VPZ_DYNAMIC:
            return mParent->is_valid_dynamic(mEntry.get_text());
            break;
        case Base::VLE_VPZ_CONDITION:
            return mParent->is_valid_condition(mEntry.get_text());
            break;
        case Base::VLE_VPZ_OUTPUT:
            return mParent->is_valid_output(mEntry.get_text());
            break;
        case Base::VLE_VPZ_OBSERVABLE:
            return mParent->is_valid_observable(mEntry.get_text());
            break;
        case Base::VLE_VPZ_VIEW:
            return mParent->is_valid_view(mEntry.get_text());
            break;
        default:
            return mState;
            break;
        }
    } else {
        return mParent->is_valid_model(mEntry.get_text());
    }
}

ImportModelBox::ImportModelBox(const Glib::RefPtr < Gtk::Builder >& xml,
                               Modeling* modeling)
  : mXml(xml), mModeling(modeling), mNbWidgetIncorrect(0)
{
    xml->get_widget("DialogImportModel", mDialog);
    xml->get_widget("ImportVboxDynamics", mVboxDyns);
    xml->get_widget("ImportVboxConditions", mVboxConds);
    xml->get_widget("ImportVboxOutputs", mVboxOutputs);
    xml->get_widget("ImportVboxObservables", mVboxObs);
    xml->get_widget("ImportVboxViews", mVboxViews);
    xml->get_widget("ImportVboxModels", mVboxModels);

    xml->get_widget("ButtonImportApply", mButtonApply);
    mButtonApply->signal_clicked().connect(
        sigc::mem_fun(*this, &ImportModelBox::on_apply));

    xml->get_widget("ButtonImportCancel", mButtonCancel);
    mButtonCancel->signal_clicked().connect(
        sigc::mem_fun(*this, &ImportModelBox::on_cancel));
}

bool ImportModelBox::show(vpz::Vpz* src)
{
    mSrc = src;

    mButtonApply->set_sensitive(true);
    mButtonCancel->set_sensitive(true);
    mNbWidgetIncorrect = 0;
    mReturn = false;

    makeDynamics();
    makeConditions();
    makeOutputs();
    makeObservables();
    makeViews();
    makeModels();

    mDialog->show_all();
    mDialog->run();
    mDialog->hide();


    return mReturn;
}

bool ImportModelBox::is_valid_dynamic(std::string dynamic)
{
    std::string s = boost::trim_copy(dynamic);

    if (s.empty())
        return false;

    bool ret = mModeling->dynamics().exist(s);
    if (ret)
        return false;
    else {
        guint nb = 0;
        list_widget::iterator it = mListDyns.begin();
        while (it != mListDyns.end()) {
            if ((*it)->get_text() == dynamic)
                ++nb;

            ++it;
        }
        return ((nb<=1) ? true : false);
    }
}


bool ImportModelBox::is_valid_condition(std::string condition)
{
    std::string s = boost::trim_copy(condition);

    if (s.empty())
        return false;

    bool ret = mModeling->conditions().exist(s);
    if (ret)
        return false;
    else {
        guint nb = 0;
        list_widget::iterator it = mListConds.begin();
        while (it != mListConds.end()) {
            if ((*it)->get_text() == condition)
                ++nb;

            ++it;
        }
        return ((nb<=1) ? true : false);
    }
}

bool ImportModelBox::is_valid_output(std::string output)
{
    std::string s = boost::trim_copy(output);

    if (s.empty())
        return false;

    bool ret = mModeling->outputs().exist(s);
    if (ret)
        return false;
    else {
        guint nb = 0;
        list_widget::iterator it = mListOutputs.begin();
        while (it != mListOutputs.end()) {
            if ((*it)->get_text() == output)
                ++nb;

            ++it;
        }
        return ((nb<=1) ? true : false);
    }
}

bool ImportModelBox::is_valid_observable(std::string obs)
{
    std::string s = boost::trim_copy(obs);

    if (s.empty())
        return false;

    bool ret = mModeling->observables().exist(s);
    if (ret)
        return false;
    else {
        guint nb = 0;
        list_widget::iterator it = mListObs.begin();
        while (it != mListObs.end()) {
            if ((*it)->get_text() == obs)
                ++nb;

            ++it;
        }
        return ((nb<=1) ? true : false);
    }
}

bool ImportModelBox::is_valid_view(std::string view)
{
    std::string s = boost::trim_copy(view);

    if (s.empty())
        return false;

    bool ret = mModeling->views().exist(s);
    if (ret)
        return false;
    else {
        guint nb = 0;
        list_widget::iterator it = mListViews.begin();
        while (it != mListViews.end()) {
            if ((*it)->get_text() == view)
                ++nb;

            ++it;
        }
        return ((nb<=1) ? true : false);
    }
}

bool ImportModelBox::is_valid_model(std::string model)
{
    if (mSrc->project().model().model()->isCoupled())
	return true;

    std::string s = boost::trim_copy(model);
    if (s.empty())
        return false;

    if (mParent->exist(model))
	return false;

    int nb = 0;
    list_widget::iterator it_widget = mListModels.begin();
    while (it_widget != mListModels.end()) {
        if ((*it_widget)->get_text() == model)
            ++nb;

        ++it_widget;
    }
    return ((nb<=1) ? true : false);
}

void ImportModelBox::dec_nb_widget_incorrect()
{
    if (--mNbWidgetIncorrect == 0) {
        mButtonApply->set_sensitive(true);
    }
}

void ImportModelBox::inc_nb_widget_incorrect()
{
    if (++mNbWidgetIncorrect != 0) {
        mButtonApply->set_sensitive(false);
    }
}

void ImportModelBox::makeDynamics()
{
    if (!mListDyns.empty()) {
        Glib::ListHandle<Gtk::Widget*> list = mVboxDyns->get_children();
        Glib::ListHandle<Gtk::Widget*>::const_iterator it = list.begin();
        while (it != list.end()) {
            mVboxDyns->remove(**it);
            delete *it;

            ++it;
        }
        mListDyns.clear();
    }

    Dynamics& dyns = mSrc->project().dynamics();
    const DynamicList& dyn_list = dyns.dynamiclist();
    DynamicList::const_iterator it = dyn_list.begin();
    while (it != dyn_list.end()) {
        ImportWidget* widget = new ImportWidget(this, &dyns.get(it->first));
        mListDyns.insert(widget);
        mVboxDyns->pack_start(*widget, true, true);

        ++it;
    }
}

void ImportModelBox::makeConditions()
{
    if (!mListConds.empty()) {
        Glib::ListHandle<Gtk::Widget*> list = mVboxConds->get_children();
        Glib::ListHandle<Gtk::Widget*>::const_iterator it = list.begin();
        while (it != list.end()) {
            mVboxConds->remove(**it);
            delete *it;

            ++it;
        }
        mListConds.clear();
    }

    Conditions& conds = mSrc->project().experiment().conditions();
    const ConditionList& cond_list = conds.conditionlist();
    ConditionList::const_iterator it = cond_list.begin();
    while (it != cond_list.end()) {
        ImportWidget* widget = new ImportWidget(this, &conds.get(it->first));
        mListConds.insert(widget);
        mVboxConds->pack_start(*widget, true, true);

        ++it;
    }
}

void ImportModelBox::makeOutputs()
{
    if (!mListOutputs.empty()) {
        Glib::ListHandle<Gtk::Widget*> list = mVboxOutputs->get_children();
        Glib::ListHandle<Gtk::Widget*>::const_iterator it = list.begin();
        while (it != list.end()) {
            mVboxOutputs->remove(**it);
            delete *it;

            ++it;
        }
        mListOutputs.clear();
    }

    Outputs& outputs = mSrc->project().experiment().views().outputs();
    const OutputList& output_list = outputs.outputlist();
    OutputList::const_iterator it = output_list.begin();
    while (it != output_list.end()) {
        ImportWidget* widget = new ImportWidget(this, &outputs.get(it->first));
        mListOutputs.insert(widget);
        mVboxOutputs->pack_start(*widget, true, true);

        ++it;
    }
}

void ImportModelBox::makeObservables()
{
    if (!mListObs.empty()) {
        Glib::ListHandle<Gtk::Widget*> list = mVboxObs->get_children();
        Glib::ListHandle<Gtk::Widget*>::const_iterator it = list.begin();
        while (it != list.end()) {
            mVboxObs->remove(**it);
            delete *it;

            ++it;
        }
        mListObs.clear();
    }

    Observables& obs = mSrc->project().experiment().views().observables();
    const ObservableList& obs_list = obs.observablelist();
    ObservableList::const_iterator it = obs_list.begin();
    while (it != obs_list.end()) {
        ImportWidget* widget = new ImportWidget(this, &obs.get(it->first));
        mListObs.insert(widget);
        mVboxObs->pack_start(*widget, true, true);

        ++it;
    }
}

void ImportModelBox::makeViews()
{
    if (!mListViews.empty()) {
        Glib::ListHandle<Gtk::Widget*> list = mVboxViews->get_children();
        Glib::ListHandle<Gtk::Widget*>::const_iterator it = list.begin();
        while (it != list.end()) {
            mVboxViews->remove(**it);
            delete *it;

            ++it;
        }
        mListViews.clear();
    }

    Views& views = mSrc->project().experiment().views();
    const ViewList& view_list = views.viewlist();
    ViewList::const_iterator it = view_list.begin();
    while (it != view_list.end()) {
        ImportWidget* widget = new ImportWidget(this, &views.get(it->first));
        mListViews.insert(widget);
        mVboxViews->pack_start(*widget, true, true);

        ++it;
    }
}

void ImportModelBox::makeModels()
{
    if (!mListModels.empty()) {
        Glib::ListHandle<Gtk::Widget*> list = mVboxModels->get_children();
        Glib::ListHandle<Gtk::Widget*>::const_iterator it = list.begin();
        while (it != list.end()) {
            mVboxModels->remove(**it);
            delete *it;

            ++it;
        }
        mListModels.clear();
    }

    vpz::AtomicModelVector list;
    mSrc->project().model().getAtomicModelList(list);
    vpz::AtomicModelVector::iterator it = list.begin();

    while (it != list.end()) {
        ImportWidget* widget = new ImportWidget(this, *it);
        mListModels.insert(widget);
        mVboxModels->pack_start(*widget, true, true);

        ++it;
    }
}

void ImportModelBox::on_apply()
{
    ImportWidget* widget;
    list_widget::iterator it;

    for (it = mListDyns.begin(); it != mListDyns.end(); ++it) {
        widget = *it;
        vpz::Dynamic* dyn = dynamic_cast<vpz::Dynamic*>((*it)->get_base());
        if (dyn->name() != widget->get_text())
            rename_dynamic(mSrc, dyn->name(), widget->get_text());
    }

    for (it = mListConds.begin(); it != mListConds.end(); ++it) {
        widget = *it;
        vpz::Condition* cond = dynamic_cast<vpz::Condition*>((*it)->get_base());
        if (cond->name() != widget->get_text())
            rename_condition(mSrc, cond->name(), widget->get_text());
    }

    for (it = mListOutputs.begin(); it != mListOutputs.end(); ++it) {
        widget = *it;
        vpz::Output* output = dynamic_cast<vpz::Output*>((*it)->get_base());
        if (output->name() != widget->get_text())
            rename_output(mSrc, output->name(), widget->get_text());
    }

    for (it = mListObs.begin(); it != mListObs.end(); ++it) {
        widget = *it;
        vpz::Observable* obs = dynamic_cast<vpz::Observable*>((*it)->get_base());
        if (obs->name() != widget->get_text())
            rename_observable(mSrc, obs->name(), widget->get_text());
    }

    for (it = mListViews.begin(); it != mListViews.end(); ++it) {
        widget = *it;
        vpz::View* view = dynamic_cast<vpz::View*>((*it)->get_base());
        if (view->name() != widget->get_text())
            rename_view(mSrc, view->name(), widget->get_text());
    }

    for (it = mListModels.begin(); it != mListModels.end(); ++it) {
        widget = *it;
	if (widget->get_model()->getName() != widget->get_text())
	    rename_model(widget->get_model(), widget->get_text());
    }

    mReturn = true;
}

void ImportModelBox::on_cancel()
{
    mReturn = false;
    mNbWidgetIncorrect = 0;
}

void ImportModelBox::rename_dynamic(vpz::Vpz* src, std::string old_name, std::string new_name)
{
    vpz::AtomicModelVector list;
    src->project().model().getAtomicModelList(list);
    vpz::AtomicModelVector::iterator it = list.begin();

    while (it != list.end()) {
        if ((*it)->dynamics() == old_name) {
            (*it)->setDynamics(new_name);
        }
        ++it;
    }

    Dynamics& dyns = src->project().dynamics();
    Dynamic& dyn = dyns.get(old_name);
    Dynamic* new_dyn = new Dynamic(new_name);
    new_dyn->setLibrary(dyn.library());
    new_dyn->setLanguage(dyn.language());
    new_dyn->permanent(dyn.isPermanent());
    dyns.del(old_name);
    dyns.add(*new_dyn);
}

void ImportModelBox::rename_condition(vpz::Vpz* src, std::string old_name, std::string new_name)
{
    vpz::AtomicModelVector list;
    src->project().model().getAtomicModelList(list);
    vpz::AtomicModelVector::iterator it = list.begin();

    while (it != list.end()) {
        std::vector < std::string > vec = (*it)->conditions();
        std::vector < std::string >::iterator it_vec = std::find(vec.begin(),
                                                                 vec.end(),
                                                                 old_name);
        if (it_vec != vec.end()) {
            *it_vec = new_name;
            (*it)->setConditions(vec);
        }
        ++it;
    }

    Conditions& conds = src->project().experiment().conditions();
    Condition& cond = conds.get(old_name);
    Condition* new_cond = new Condition(cond);
    new_cond->setName(new_name);

    conds.del(old_name);
    conds.add(*new_cond);
}

void ImportModelBox::rename_output(vpz::Vpz* src, std::string old_name,
                                   std::string new_name)
{
    src->project().experiment().views().renameOutput(old_name, new_name);
    Outputs& outs = src->project().experiment().views().outputs();
    Output& output = outs.get(old_name);
    Output* new_output = new Output(output);
    new_output->setName(new_name);
    outs.del(old_name);
    outs.add(*new_output);
}

void ImportModelBox::rename_observable(vpz::Vpz* src, std::string old_name,
                                       std::string new_name)
{
    vpz::AtomicModelVector list;
    src->project().model().getAtomicModelList(list);
    vpz::AtomicModelVector::iterator it = list.begin();

    while (it != list.end()) {
        if ((*it)->observables() == old_name)
            (*it)->setObservables(new_name);

        ++it;
    }

    Observables& obss = src->project().experiment().views().observables();
    Observable& obs = obss.get(old_name);
    Observable* new_obs = new Observable(new_name);
    const ObservablePortList& port_list = obs.observableportlist();
    ObservablePortList::const_iterator it_port = port_list.begin();
    while (it_port != port_list.end()) {
        new_obs->add(it_port->second);
        ++it_port;
    }
    obss.del(old_name);
    obss.add(*new_obs);
}

void ImportModelBox::rename_view(vpz::Vpz* src, std::string old_name,
                                 std::string new_name)
{
    vpz::Observables& obss = src->project().experiment().views().observables();
    const vpz::ObservableList& obs_list = obss.observablelist();
    vpz::ObservableList::const_iterator it_obs = obs_list.begin();
    while (it_obs != obs_list.end()) {
        const vpz::ObservablePortList& port_list =
            it_obs->second.observableportlist();
        vpz::ObservablePortList::const_iterator it_port = port_list.begin();
        while (it_port != port_list.end()) {
            if (it_port->second.exist(old_name)) {
                vpz::ObservablePort& port =
                    obss.get(it_obs->first).get(it_port->first);
                port.del(old_name);
                port.add(new_name);
            }
            ++it_port;
        }

        ++it_obs;
    }

    Views& the_views = src->project().experiment().views();
    vpz::View copy(the_views.get(old_name));
    the_views.del(old_name);

    switch (copy.type()) {
    case vpz::View::TIMED:
        the_views.addTimedView(new_name, copy.timestep(),
                               copy.output()).setData(copy.data());
        break;
    case vpz::View::EVENT:
        the_views.addEventView(new_name, copy.output()).setData(copy.data());
        break;
    case vpz::View::FINISH:
        the_views.addFinishView(new_name, copy.output()).setData(copy.data());
        break;
    }
}

void ImportModelBox::rename_model(vpz::BaseModel* old_model,
                                  std::string new_name)
{
    vpz::BaseModel::rename(old_model, new_name);
}

}
} // namespace vle gvle
