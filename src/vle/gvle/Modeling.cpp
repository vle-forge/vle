/*
 * @file vle/gvle/Modeling.cpp
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


#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/Editor.hpp>
#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/GVLEMenuAndToolbar.hpp>
#include <vle/gvle/View.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/ModelDescriptionBox.hpp>
#include <vle/gvle/DialogString.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Path.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/graph/Model.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <gtkmm/stock.h>
#include <iostream>

using std::string;

namespace vle { namespace gvle {

Modeling::Modeling(const string& filename) :
    mTop(0),
    mIsModified(false),
    mIsSaved(false),
    mSocketPort(8000)
{
    mRand = new utils::Rand();
    if (not filename.empty()) {
        mVpz.parseFile(filename);
        if (mVpz.project().model().model() == 0) {
            gvle::Error(
                (fmt(_("Problem loading file %1%, empty structure.")) %
                 filename).str());
            mTop = newCoupledModel(0, "Top model", "", 0, 0);
            mVpz.project().model().setModel(mTop);
        } else if (mVpz.project().model().model()->isAtomic()) {
            graph::CoupledModel* t = newCoupledModel(0, "Top model",
                                                     "", 0, 0);
            t->addModel(mVpz.project().model().model());
            mVpz.project().model().setModel(t);
            mTop = t;
        }
    }
}

Modeling::~Modeling()
{
    delete mRand;
    if (mTop) {
       delete mTop;
    }
}

void Modeling::clearModeling()
{
    setModified(false);
    mIsSaved = false;
    mFileName.clear();
    mVpz.clear();
    if (mTop) {
       delete mTop;
    }
    mTop = 0;
}

void Modeling::setGlade(Glib::RefPtr < Gnome::Glade::Xml > xml)
{
    mRefXML = xml;
}

Glib::RefPtr < Gnome::Glade::Xml > Modeling::getGlade() const
{
    assert(mRefXML);
    return mRefXML;
}

void Modeling::parseXML(const string& filename)
{
    try {
        clearModeling();
        delNames();
        mVpz.parseFile(filename);
        if (mVpz.project().model().model() == 0) {
            gvle::Error(
                (fmt(_("Problem loading file %1%, empty structure.")) %
                 filename).str());
            mTop = newCoupledModel(0, "Top model", "", 0, 0);
            mVpz.project().model().setModel(mTop);
        } else if (mVpz.project().model().model()->isAtomic()) {
            graph::CoupledModel* t = newCoupledModel(0, "Top model", "", 0, 0);
            t->addModel(mVpz.project().model().model());
            mVpz.project().model().setModel(t);
            mTop = t;
        } else {
            mTop = (graph::CoupledModel*)(mVpz.project().model().model());
        }
    } catch (const utils::SaxParserError& e) {
        gvle::Error((fmt(_("Error parsing file, %1%")) %
                     e.what()).str(),false);
    } catch (const utils::ParseError& p) {
        gvle::Error((fmt(_("Error parsing file, %1%")) %
                     p.what()).str());
    } catch (const utils::FileError& f) {
        gvle::Error((fmt(_("Error opening file, %1%")) %
                     f.what()).str());
    } catch (const Glib::Error& e) {
        gvle::Error((fmt(_("Error opening file, %1%")) %
                     e.what()).str());
    } catch (const std::exception& e) {
        gvle::Error((fmt(_("Error opening file, %1%")) %
                     e.what()).str());
    }
}

void Modeling::saveXML(const std::string& name)
{
    try {
        mVpz.write(name);
        mFileName.assign(name);
        mIsSaved = true;
        setModified(false);
    } catch (const utils::FileError& file) {
        gvle::Error(
            (fmt(_("Project save problem. Check your file %1%. %2%")) % name %
             file.what()).str());
    } catch (const utils::ParseError& parse) {
        gvle::Error(
            (fmt(_("Project save problem. Parsing error %1%. %2%")) % name %
             parse.what()).str());
    } catch (const std::exception& e) {
        gvle::Error(
            (fmt(_("Project save problem. Exception %1%. %2%")) % name %
             e.what()).str());
    }
}

void Modeling::setTopModel(graph::CoupledModel* cp)
{
    if (mTop) {
        clearModeling();
    }

    mTop = cp;
}

vpz::AtomicModelList& Modeling::getAtomicModelClass(std::string className)
{
    if (className == "") {
        return mVpz.project().model().atomicModels();
    } else {
        return mVpz.project().classes().get(className).atomicModels();
    }
}

void Modeling::showDynamics(const std::string& /* name */)
{
    Error(_("This function is not yet implemented"));
}

bool Modeling::exist(const std::string& name) const
{
    return mTop->findModel(name) != 0;
}

bool Modeling::exist(const graph::Model* m) const
{
    return (mTop->findModel(m->getName()) != 0);
}

void Modeling::exportCoupledModel(graph::CoupledModel* model,
                                  vpz::Vpz* dst, std::string className)
{
    vpz::Project& project = dst->project();
    project.setAuthor(vpz().project().author());
    project.setDate(vpz().project().date());
    project.setVersion(vpz().project().version());
    vpz::Experiment& experiment = project.experiment();
    experiment.setName(vpz().project().experiment().name());
    experiment.setDuration(vpz().project().experiment().duration());

    export_coupled_model(dst, model, className);
    dst->project().model().setModel(model);
    dst->write();
}

void Modeling::exportClass(graph::Model* model, vpz::Class classe,
                           vpz::Vpz* dst)
{
    vpz::Project& project = dst->project();
    project.setAuthor(vpz().project().author());
    project.setDate(vpz().project().date());
    project.setVersion(vpz().project().version());
    vpz::Experiment& experiment = project.experiment();
    experiment.setName(vpz().project().experiment().name());
    experiment.setDuration(vpz().project().experiment().duration());

    std::string classeName = classe.name();

    if (model->isCoupled()) {
        export_coupled_model(dst, dynamic_cast<graph::CoupledModel*>(model),
                             classeName);
    } else {
	export_atomic_model(dst, dynamic_cast<graph::AtomicModel*>(model),
                            classeName);
    }
    dst->project().model().setModel(model);
    dst->write();
}

void Modeling::export_atomic_model(vpz::Vpz* dst, graph::AtomicModel* model,
                                   std::string className)
{
    using namespace vpz;

    AtomicModel atom = get_model(dynamic_cast<graph::AtomicModel*>(model),
                                 className);

    Project& project = dst->project();
    Dynamics& dyns = project.dynamics();
    Views& views = project.experiment().views();
    Outputs& outputs = views.outputs();
    Observables& obs = views.observables();
    Conditions& cond = project.experiment().conditions();

    //Dynamic
    if (!atom.dynamics().empty() && !dyns.exist(atom.dynamics())) {
        project.dynamics().add(vpz().project().dynamics().get(atom.dynamics()));
    }

    //Conditions
    const std::vector < std::string >& vec = atom.conditions();
    for (std::vector < std::string >::const_iterator it_cond = vec.begin();
         it_cond != vec.end(); ++it_cond) {
        if (!it_cond->empty() && !cond.exist(*it_cond)) {
            cond.add(vpz().project().experiment().conditions().get(*it_cond));
        }
    }

    //Observable
    if (!atom.observables().empty() && !obs.exist(atom.observables())) {
        Observable& o =
            vpz().project().experiment().views().observables().get(atom.observables());
        obs.add(o);
        const ObservablePortList& obs_list = o.observableportlist();
        ObservablePortList::const_iterator it_obs = obs_list.begin();
        while (it_obs != obs_list.end()) {
            const ViewNameList& view_list = it_obs->second.viewnamelist();
            ViewNameList::const_iterator it_view = view_list.begin();
            while (it_view != view_list.end()) {
                vpz::View& v = vpz().project().experiment().views().get(*it_view);
                if (!views.exist(*it_view))
                    views.add(v);

                if (!outputs.exist(v.output()))
                    outputs.add(vpz().project().experiment().views().outputs().get(v.output()));

                ++it_view;
            }
            ++it_obs;
        }
    }

    //Model
    if (className == "") {
        if (vpz().project().model().atomicModels().exist(model)) {
            dst->project().model().atomicModels().add(model, atom);
        }
    } else {
        if (vpz().project().classes().get(className).atomicModels().exist(model)) {
            dst->project().model().atomicModels().add(model, atom);
        }
    }
}

void Modeling::export_coupled_model(vpz::Vpz* dst, graph::CoupledModel* model, std::string className)
{
    graph::ModelList& list = model->getModelList();
    graph::ModelList::const_iterator it = list.begin();
    while (it!= list.end()) {
        if (it->second->isAtomic()) {
            export_atomic_model(dst, dynamic_cast<graph::AtomicModel*>(it->second), className);
        } else {
            export_coupled_model(dst, dynamic_cast<graph::CoupledModel*>(it->second), className);
        }

        ++it;
    }
}

void Modeling::importModel(graph::Model* import, vpz::Vpz* src)
{
    if (import->isAtomic()) {
        import_atomic_model(src, graph::Model::toAtomic(import));
    } else {
        import_coupled_model(src, graph::Model::toCoupled(import));
    }
    dynamics().add(src->project().dynamics());
    conditions().add(src->project().experiment().conditions());
    views().add(src->project().experiment().views());
    setModified(true);
}

void Modeling::importModelToClass(vpz::Vpz* src, std::string& className)
{
    using namespace vpz;

    Classes& classes = vpz().project().classes();
    Class& new_class = classes.add(className);
    graph::Model* import = src->project().model().model();
    if (import->isAtomic()) {
        new_class.setModel(graph::Model::toAtomic(import));
        import_atomic_model(src, graph::Model::toAtomic(import),
                            className);
    } else {
        new_class.setModel(graph::Model::toCoupled(import));
        import_coupled_model(src, graph::Model::toCoupled(import),
                             className);
    }
    dynamics().add(src->project().dynamics());
    conditions().add(src->project().experiment().conditions());
    views().add(src->project().experiment().views());
    setModified(true);
}

void Modeling::import_atomic_model(vpz::Vpz* src, graph::AtomicModel* atom, std::string className)
{
    using namespace vpz;

    AtomicModel& vpz_atom = src->project().model().atomicModels().get(atom);
    if (className.empty())
        vpz().project().model().atomicModels().add(atom, vpz_atom);
    else
        vpz().project().classes().get(className).atomicModels().add(atom, vpz_atom);
}

void Modeling::import_coupled_model(vpz::Vpz* src, graph::CoupledModel* model, std::string className)
{
    graph::ModelList& list = model->getModelList();
    if (!list.empty()) {
        graph::ModelList::const_iterator it = list.begin();
        while (it!= list.end()) {
            if (it->second->isAtomic()) {
                import_atomic_model(src, graph::Model::toAtomic(it->second), className);
            } else {
                import_coupled_model(src, graph::Model::toCoupled(it->second), className);
            }
            ++it;
        }
    }
}

bool Modeling::existModelName(const std::string& name) const
{
    return (mModelsNames.find(name) != mModelsNames.end());
}

bool Modeling::isCorrectName(const Glib::ustring& name) const
{
    return (name.empty() == false and name.is_ascii() == true and
            existModelName(name) == false);
}

bool Modeling::getNewName(std::string& name) const
{
    std::list < Glib::ustring > lst(mModelsNames.begin(), mModelsNames.end());
    gvle::DialogString box("Get a new name", "Name", lst);
    Glib::ustring error = box.run();

    if (error.empty() == true) {
        name.assign(box.get_string());
        return true;
    } else {
        gvle::Error(error);
        return false;
    }
}

void Modeling::delName(const std::string& name)
{
    mModelsNames.erase(name);
}

void Modeling::delNames()
{
    mModelsNames.clear();
}

void Modeling::addName(const std::string& name)
{
    mModelsNames.insert(name);
}

std::string Modeling::getNameWithPrefix(const std::string& name)
{
    Glib::Rand prand;
    std::string prefix(name);

    if (name.empty()) {
        for (int i=0; i < 8; ++i) {
            char big = (char)prand.get_int_range('A', 'Z');
            char sml = (char)prand.get_int_range('a', 'z');
            prefix.push_back(prand.get_bool() ? big : sml);
        }
    }
    prefix.push_back('_');
    while (existModelName(prefix)) {
        char big = (char)prand.get_int_range('A', 'Z');
        char sml = (char)prand.get_int_range('a', 'z');
        prefix.erase(prefix.end() - 1);
        prefix.push_back(prand.get_bool() ? big : sml);
        prefix.push_back('_');
    }
    return prefix;
}

const Modeling::SetString& Modeling::getNames() const
{
    return mModelsNames;
}


const std::vector < std::string >*
Modeling::get_conditions(graph::AtomicModel* atom)
{
    assert(atom);

    vle::vpz::AtomicModelList& list = mVpz.project().model().atomicModels();
    vle::vpz::AtomicModel& a = list.get(atom);
    return &(a.conditions());
}

graph::CoupledModel* Modeling::newCoupledModel(graph::CoupledModel* parent,
        const string& name,
        const string& /* description */,
        int x, int y)
{
    setModified(true);
    graph::CoupledModel* tmp = new graph::CoupledModel(name, parent);
    tmp->setPosition(x, y);
    return tmp;
}

graph::AtomicModel* Modeling::newAtomicModel(graph::CoupledModel* parent,
        const string& name,
        const string& /* description */,
        int x, int y)
{
    if (not parent or not parent->exist(name)) {
        setModified(true);
        graph::AtomicModel* tmp = new graph::AtomicModel(name, parent);
        tmp->setPosition(x, y);
        tmp->setWidth(ViewDrawingArea::MODEL_WIDTH);
        tmp->setHeight(ViewDrawingArea::MODEL_HEIGHT);
       return tmp;
    }
    return 0;
}

void Modeling::vpz_is_correct(std::vector<std::string>& vec)
{
    using namespace vpz;

    const Project& project = mVpz.project();

    if (project.author() == "") {
        vec.push_back(_("The author is not mentioned."));
    }

    const DynamicList& dyn_list = project.dynamics().dynamiclist();
    DynamicList::const_iterator dyn_it = dyn_list.begin();
    while (dyn_it != dyn_list.end()) {
        if (dyn_it->second.library() == "") {
            vec.push_back((fmt(_("The Dynamic %1% is not attached with a "
                                 "library.")) % dyn_it->first).str());
        }
        ++dyn_it;
    }

    const Experiment& exp = project.experiment();
    if (exp.name() == "")
        vec.push_back(_("The Experiment has no name."));

    if (exp.duration() == 0)
        vec.push_back(_("The Experiment has no duration."));

    const ViewList& view_list = exp.views().viewlist();
    ViewList::const_iterator view_it = view_list.begin();
    while (view_it != view_list.end()) {
        if (view_it->second.output() == "") {
            vec.push_back((fmt(_("The view %1% has no output.")) %
                           view_it->first).str());
        }
        ++view_it;
    }

}

std::string Modeling::getDynamicInfo(std::string dynamicName) const
{
    std::string cardp = "";

    if  (dynamics().exist(dynamicName)) {
        const vpz::Dynamic& dyn = dynamics().get(dynamicName);

        cardp += "\n<b>Project: </b>" + dyn.package();
        cardp += "\n<b>Library: </b>" + dyn.library();
    } else {
        cardp += "<i> (missing)</i>";
    }
    return cardp;
}

std::string Modeling::getIdCard(std::string className) const
{
    const vpz::Class& mclasses = vpz().project().classes().get(className);
    std::string nclas = "<b>Class: </b>"+ mclasses.name();
    return nclas;
}

std::string Modeling::getIdCard(graph::Model* model) const
{
    if (model->isCoupled()) {
        graph::CoupledModel* m = (graph::CoupledModel*)(model);
        return getIdCard(m);
    } else if (model->isAtomic()) {
        Glib::ustring card = "<b>Model : </b>"+
            model->getName() + " <i>(Atomic)</i>";

        const vpz::Model& mmodel = vpz().project().model();
        const vpz::AtomicModel& atm = mmodel.atomicModels().get(model);
        const std::string nom_dyn = atm.dynamics();

        if (!nom_dyn.empty()) {
            card += "\n<b>Dynamic : </b>" + nom_dyn;
            card += getDynamicInfo(nom_dyn);
        }
        return card;
    }
    return "";
}

std::string Modeling::getIdCard(graph::CoupledModel* model) const
{
    return ("<b>Model: </b>" + model->getName() + " <i>(Coupled)</i>");
}

std::string Modeling::getInfoCard(std::string cond) const
{
    std::string card = "<b>Atomic Models:</b>\n";

    vpz::AtomicModelList list =
        vpz().project().model().atomicModels();
    vpz::AtomicModelList::iterator it = list.begin();

    while (it != list.end()) {
        std::vector < std::string > mdlConditions =
            it->second.conditions();
        std::vector < std::string >::const_iterator its =
            std::find(mdlConditions.begin(), mdlConditions.end(), cond);

        if(its != mdlConditions.end()) {
            card = card + it->first->getName() + "\n";
        }
        ++it;
    }

    vpz::ClassList::const_iterator itc = vpz().project().classes().begin();
    card = card + "<b>Classes:</b>";

    while (itc != vpz().project().classes().end()) {
        vpz::AtomicModelList atomlist( itc->second.atomicModels() );
        vpz::AtomicModelList::iterator itl = atomlist.begin();

        while (itl != atomlist.end()) {
            std::vector < std::string > mdlConditions =
                itl->second.conditions();
            std::vector < std::string >::const_iterator its =
                std::find(mdlConditions.begin(), mdlConditions.end(), cond);

            if(its != mdlConditions.end()) {
                card = card + "\n" + itc->first;
                break;
            }
            ++itl;
        }
        ++itc;
    }

    return card;
}

std::string Modeling::getClassIdCard(graph::Model* model,
                                     std::string className) const
{
    if (model->isCoupled()) {
        graph::CoupledModel* m = (graph::CoupledModel*)(model);
        return getClassIdCard(m);
    } else if (model->isAtomic()) {
        Glib::ustring card = "<b>Model : </b>" +
            model->getName() + " <i>(Atomic)</i>";

        const vpz::Class& classes = vpz().project().classes().get(className);
        const vpz::AtomicModel& atm = classes.atomicModels().get(model);
        const std::string nom_dyn = atm.dynamics();

        if (!nom_dyn.empty()) {
            card += "\n<b>Dynamic : </b>" + nom_dyn;
            card += getDynamicInfo(nom_dyn);
        }
        return card;
    }
    return "";
}

std::string Modeling::getClassIdCard(graph::CoupledModel* model) const
{
    return ("<b>Model: </b>" + model->getName() + " <i>(Coupled)</i>");
}


std::string Modeling::getIdCardConnection(graph::Model* src,
                                          graph::Model* dest,
                                          graph::CoupledModel* mTop) const
{
    Glib::ustring card;
    const graph::ModelList& children(mTop->getModelList());
    graph::ModelList::const_iterator it;

    for (it = children.begin(); it != children.end(); ++it) {
        const graph::ConnectionList& outs(it->second->getOutputPortList());
        graph::ConnectionList::const_iterator jt;
        if ((it->second == src) or (it->second == dest)){
            for (jt = outs.begin(); jt != outs.end(); ++jt) {
                const graph::ModelPortList&  ports(jt->second);
                graph::ModelPortList::const_iterator kt;
                for (kt = ports.begin(); kt != ports.end(); ++kt){
                    if ((kt->first == src) or (kt->first == dest)) {
                        if (card.compare("") != 0) {
                            card += "\n";
                        }
                        card += "<b>" + it->second->getName() + "</b>" +
                            + ":" + "<i>" + jt->first + "</i>" +
                            + " -> " + "<b>" + kt->first->getName() + "</b>" +
                            + ":" + "<i>" + kt->second + "</i>";
                    }
                }
            }
        }
    }
    return card;
}

std::string Modeling::getIdCardConnection(graph::Model* src,
                                          std::string srcport,
                                          graph::Model* dest,
                                          std::string destport,
                                          graph::CoupledModel* /* mTop */) const
{
    return ("<b>" + src->getName() + "</b>" + ":" + "<i>" + srcport + "</i>"
            +  " -> " + "<b>" +dest->getName() + "</b>" + ":" + "<i>" +
            destport + "</i>");
}

}
} // namespace vle gvle
