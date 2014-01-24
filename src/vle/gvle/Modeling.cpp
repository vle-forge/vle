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


#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/ViewDrawingArea.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/BaseModel.hpp>

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
            vpz::CoupledModel* t = newCoupledModel(0, "Top model",
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
            vpz::CoupledModel* t = newCoupledModel(0, "Top model", "", 0, 0);
            t->addModel(mVpz.project().model().model());
            mVpz.project().model().setModel(t);
            mTop = t;
        } else {
            mTop = (vpz::CoupledModel*)(mVpz.project().model().model());
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

void Modeling::setTopModel(vpz::CoupledModel* cp)
{
    if (mTop) {
        clearModeling();
    }

    mTop = cp;
}

void Modeling::showDynamics(const std::string& /* name */)
{
    Error(_("This function is not yet implemented"));
}

void Modeling::exportCoupledModel(vpz::CoupledModel* model,
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

void Modeling::exportClass(vpz::BaseModel* model, vpz::Class classe,
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
        export_coupled_model(dst, dynamic_cast<vpz::CoupledModel*>(model),
                             classeName);
    } else {
	export_atomic_model(dst, dynamic_cast<vpz::AtomicModel*>(model));
    }
    dst->project().model().setModel(model);
    dst->write();
}

void Modeling::export_atomic_model(vpz::Vpz* dst, vpz::AtomicModel* model)
{
    using namespace vpz;

    Project& project = dst->project();
    Dynamics& dyns = project.dynamics();
    Views& views = project.experiment().views();
    Outputs& outputs = views.outputs();
    Observables& obs = views.observables();
    Conditions& cond = project.experiment().conditions();

    //Dynamic
    if (!model->dynamics().empty() && !dyns.exist(model->dynamics())) {
        project.dynamics().add(vpz().project().dynamics().get(model->dynamics()));
    }

    //Conditions
    const std::vector < std::string >& vec = model->conditions();
    for (std::vector < std::string >::const_iterator it_cond = vec.begin();
         it_cond != vec.end(); ++it_cond) {
        if (!it_cond->empty() && !cond.exist(*it_cond)) {
            cond.add(vpz().project().experiment().conditions().get(*it_cond));
        }
    }

    //Observable
    if (!model->observables().empty() && !obs.exist(model->observables())) {
        Observable& o =
            vpz().project().experiment().views().observables().get(model->observables());
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
}

void Modeling::export_coupled_model(vpz::Vpz* dst, vpz::CoupledModel* model, std::string className)
{
    vpz::ModelList& list = model->getModelList();
    vpz::ModelList::const_iterator it = list.begin();
    while (it!= list.end()) {
        if (it->second->isAtomic()) {
            export_atomic_model(dst, dynamic_cast<vpz::AtomicModel*>(it->second));
        } else {
            export_coupled_model(dst, dynamic_cast<vpz::CoupledModel*>(it->second), className);
        }

        ++it;
    }
}

void Modeling::importModel(vpz::Vpz* src)
{
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
    vpz::BaseModel* import = src->project().model().model();
    if (import->isAtomic()) {
        new_class.setModel(vpz::BaseModel::toAtomic(import));
    } else {
        new_class.setModel(vpz::BaseModel::toCoupled(import));
    }
    dynamics().add(src->project().dynamics());
    conditions().add(src->project().experiment().conditions());
    views().add(src->project().experiment().views());
    setModified(true);
}

void Modeling::delNames()
{
    mModelsNames.clear();
}

const Modeling::SetString& Modeling::getNames() const
{
    return mModelsNames;
}

vpz::CoupledModel* Modeling::newCoupledModel(vpz::CoupledModel* parent,
        const string& name,
        const string& /* description */,
        int x, int y)
{
    setModified(true);
    vpz::CoupledModel* tmp = new vpz::CoupledModel(name, parent);
    tmp->setPosition(x, y);
    return tmp;
}

vpz::AtomicModel* Modeling::newAtomicModel(vpz::CoupledModel* parent,
        const string& name,
        const string& /* description */,
        int x, int y)
{
    if (not parent or not parent->exist(name)) {
        setModified(true);
        vpz::AtomicModel* tmp = new vpz::AtomicModel(name, parent);
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

std::string Modeling::getIdCard(vpz::BaseModel* model) const
{
    if (model->isCoupled()) {
        vpz::CoupledModel* m = vpz::BaseModel::toCoupled(model);
        return getIdCard(m);
    } else if (model->isAtomic()) {
        Glib::ustring card = "<b>Model : </b>"+
            model->getName() + " <i>(Atomic)</i>";

        const std::string nom_dyn = vpz::BaseModel::toAtomic(model)->dynamics();

        if (!nom_dyn.empty()) {
            card += "\n<b>Dynamic : </b>" + nom_dyn;
            card += getDynamicInfo(nom_dyn);
        }
        return card;
    }
    return "";
}

std::string Modeling::getIdCard(vpz::CoupledModel* model) const
{
    return ("<b>Model: </b>" + model->getName() + " <i>(Coupled)</i>");
}

std::string Modeling::getInfoCard(std::string cond) const
{
    std::string card = "<b>Atomic Models:</b>\n";

    vpz::AtomicModelVector list;

    vpz().project().model().getAtomicModelList(list);

    vpz::AtomicModelVector::iterator it = list.begin();

    while (it != list.end()) {
        std::vector < std::string > mdlConditions =
            (*it)->conditions();
        std::vector < std::string >::const_iterator its =
            std::find(mdlConditions.begin(), mdlConditions.end(), cond);

        if(its != mdlConditions.end()) {
            card = card + (*it)->getCompleteName()+ "\n";
        }
        ++it;
    }

    vpz::ClassList::const_iterator itc = vpz().project().classes().begin();
    card = card + "<b>Classes:</b>";

    while (itc != vpz().project().classes().end()) {
        itc->second.getAtomicModelList(list);
        it = list.begin();

        while (it != list.end()) {
            std::vector < std::string > mdlConditions =
                (*it)->conditions();
            std::vector < std::string >::const_iterator its =
                std::find(mdlConditions.begin(), mdlConditions.end(), cond);

            if(its != mdlConditions.end()) {
                card = card + "\n" + itc->first + ":" + (*it)->getCompleteName();
            }
            ++it;
        }
        ++itc;
    }

    return card;
}

std::string Modeling::getClassIdCard(vpz::BaseModel* model) const
{
    if (model->isCoupled()) {
        vpz::CoupledModel* m = vpz::BaseModel::toCoupled(model);
        return getClassIdCard(m);
    } else if (model->isAtomic()) {
        Glib::ustring card = "<b>Model : </b>" +
            model->getName() + " <i>(Atomic)</i>";

        const std::string nom_dyn = vpz::BaseModel::toAtomic(model)->dynamics();

        if (!nom_dyn.empty()) {
            card += "\n<b>Dynamic : </b>" + nom_dyn;
            card += getDynamicInfo(nom_dyn);
        }
        return card;
    }
    return "";
}

std::string Modeling::getClassIdCard(vpz::CoupledModel* model) const
{
    return ("<b>Model: </b>" + model->getName() + " <i>(Coupled)</i>");
}


std::string Modeling::getIdCardConnection(vpz::BaseModel* src,
                                          vpz::BaseModel* dest,
                                          vpz::CoupledModel* mTop) const
{
    Glib::ustring card;
    const vpz::ModelList& children(mTop->getModelList());
    vpz::ModelList::const_iterator it;

    for (it = children.begin(); it != children.end(); ++it) {
        const vpz::ConnectionList& outs(it->second->getOutputPortList());
        vpz::ConnectionList::const_iterator jt;
        if ((it->second == src) or (it->second == dest)){
            for (jt = outs.begin(); jt != outs.end(); ++jt) {
                const vpz::ModelPortList&  ports(jt->second);
                vpz::ModelPortList::const_iterator kt;
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

std::string Modeling::getIdCardConnection(vpz::BaseModel* src,
                                          std::string srcport,
                                          vpz::BaseModel* dest,
                                          std::string destport,
                                          vpz::CoupledModel* /* mTop */) const
{
    return ("<b>" + src->getName() + "</b>" + ":" + "<i>" + srcport + "</i>"
            +  " -> " + "<b>" +dest->getName() + "</b>" + ":" + "<i>" +
            destport + "</i>");
}

}
} // namespace vle gvle
