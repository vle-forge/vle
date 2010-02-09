/**
 * @file vle/gvle/Modeling.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#include <vle/gvle/AtomicModelBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/Editor.hpp>
#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/GVLEMenuAndToolbar.hpp>
#include <vle/gvle/View.hpp>
#include <vle/gvle/AtomicModelBox.hpp>
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

namespace vle
{
namespace gvle {

Modeling::Modeling(GVLE* gvle, const string& filename) :
    mTop(0),
    mGVLE(gvle),
    mIsModified(false),
    mIsSaved(false),
    mSocketPort(8000),
    mAtomicBox(0),
    mImportModelBox(0),
    mImportClassesBox(0),
    mCoupledBox(0)
{
    assert(gvle);

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
            graph::CoupledModel* t = newCoupledModel(0, "Top model", "", 0,
                                     0);
            t->addModel(mVpz.project().model().model());
            mVpz.project().model().setModel(t);
            mTop = t;
            setTitles();
        }
    } else {
        setTitles();
    }
}

Modeling::~Modeling()
{
    delViews();
    delete mAtomicBox;
    delete mImportModelBox;
    delete mImportClassesBox;
    delete mCoupledBox;
    delete mRand;
    if (mTop) {
       delete mTop;
    }
}

void Modeling::clearModeling()
{
    setModified(false);
    mIsSaved = false;
    mGVLE->getEditor()->closeVpzTab();
    mFileName.clear();
    mVpz.clear();
    if (mTop) {
       delete mTop;
    }
    mTop = 0;
    setTitles();
}

void Modeling::setGlade(Glib::RefPtr < Gnome::Glade::Xml > xml)
{
    mRefXML = xml;
    mAtomicBox = new AtomicModelBox(xml, this);
    mImportModelBox = new ImportModelBox(xml, this);
    mImportClassesBox = new ImportClassesBox(xml, this);
    mCoupledBox = new CoupledModelBox(xml, this);
}

Glib::RefPtr < Gnome::Glade::Xml > Modeling::getGlade() const
{
    assert(mRefXML);
    return mRefXML;
}

void Modeling::start()
{
    clearModeling();
    delNames();
    mTop = newCoupledModel(0, "Top model", "", 0, 0);
    mVpz.project().model().setModel(mTop);
    mGVLE->redrawModelTreeBox();
    mGVLE->redrawModelClassBox();
    if (utils::Package::package().name().empty()) {
	mFileName.assign("noname.vpz");
    } else {
	mFileName.assign(Glib::build_filename(
			     utils::Path::path().getPackageExpDir(),
			     "noname.vpz"));
    }
    mIsSaved = false;
    setModified(true);
    addView(mTop);
    setModifiedTitles();
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
	mFileName.assign(filename);
        addView(mTop);
        mIsSaved = true;
        setModified(false);
        setTitles();
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
        setTitles();
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

void Modeling::addView(graph::Model* model)
{
    if (model) {
        if (model->isCoupled()) {
            graph::CoupledModel* m = graph::Model::toCoupled(model);
            addView(m);
        } else if (model->isAtomic()) {
            try {
                mAtomicBox->show((graph::AtomicModel*)model);
            } catch (utils::SaxParserError& /*e*/) {
                parse_model(mVpz.project().model().atomicModels());
            }
        }
    }
    refreshViews();
}

void Modeling::addView(graph::CoupledModel* model)
{
    const size_t szView = mListView.size();

    View* search = findView(model);
    if (search != NULL) {
        search->selectedWindow();
    } else {
        mListView.push_back(new View(this, model, szView));
    }
    mGVLE->getEditor()->openTabVpz(mFileName, model);
}

void Modeling::addViewClass(graph::Model* model, std::string name)
{
    assert(model);
    if (model->isCoupled()) {
        graph::CoupledModel* m = (graph::CoupledModel*)(model);
        addViewClass(m, name);
    } else if (model->isAtomic()) {
        try {
	    mAtomicBox->show((graph::AtomicModel*)model, name);
        } catch (utils::SaxParserError& E) {
            parse_model(mVpz.project().classes().get(mCurrentClass).atomicModels());
        }
    }
    refreshViews();
}

void Modeling::addViewClass(graph::CoupledModel* model, std::string name)
{
    const size_t szView = mListView.size();

    View* search = findView(model);
    if (search != NULL) {
        search->selectedWindow();
    } else {
	View* v = new View(this, model, szView);
	v->setCurrentClass(name);
        mListView.push_back(v);
    }
    mGVLE->getEditor()->openTabVpz(mFileName, model);
}

bool Modeling::existView(graph::CoupledModel* model)
{
    assert(model);
    ListView::const_iterator it = mListView.begin();
    while (it != mListView.end()) {
        if ((*it) && (*it)->getGCoupledModel() == model)
            return true;
        ++it;
    }
    return false;
}

View* Modeling::findView(graph::CoupledModel* model)
{
    assert(model);
    ListView::const_iterator it = mListView.begin();
    while (it != mListView.end()) {
        if ((*it) && (*it)->getGCoupledModel() == model) {
            return *it;
        }
        ++it;
    }
    return NULL;
}

void Modeling::delViewIndex(size_t index)
{
    assert(index < mListView.size());
    delete mListView[index];
    mListView[index] = NULL;
}

void Modeling::delViewOnModel(const graph::CoupledModel* cm)
{
    assert(cm);
    const size_t sz = mListView.size();
    for (size_t i = 0; i < sz; ++i) {
        if (mListView[i] != 0 and mListView[i]->getGCoupledModel() == cm)
            delViewIndex(i);
    }
}

void Modeling::delViews()
{
    const size_t sz = mListView.size();
    for (size_t i = 0; i < sz; ++i)
        delViewIndex(i);
}

void Modeling::refreshViews()
{
    ListView::iterator it = mListView.begin();
    while (it != mListView.end()) {
        if (*it) {
            (*it)->redraw();
        }
        ++it;
    }
}

void Modeling::EditCoupledModel(graph::CoupledModel* model)
{
    assert(model);
    mCoupledBox->show(model);
}

void Modeling::setTitles()
{
    mGVLE->setTitle(mFileName);
}

void Modeling::setModifiedTitles()
{
    mGVLE->setModifiedTitle(mFileName);
}

bool Modeling::exist(const std::string& name) const
{
    return mTop->findModel(name) != 0;
}

bool Modeling::exist(const graph::Model* m) const
{
    return (mTop->findModel(m->getName()) != 0);
}

void Modeling::cut(graph::ModelList& lst, graph::CoupledModel* gc,
		   std::string className)
{
    if (className.empty()) {
	mCutCopyPaste.cut(lst, gc, mVpz.project().model().atomicModels());
    } else {
	mCutCopyPaste.cut(lst, gc, mVpz.project().classes().get(className).atomicModels());
    }
}

void Modeling::copy(graph::ModelList& lst, graph::CoupledModel* gc,
		    std::string className)
{
    // the current view is not a class
    if (className.empty()) {
	// no model is selected in current view and a class is selected
        // -> class instantiation
	if (lst.empty() and not mSelectedClass.empty()) {
	    vpz::Class& currentClass = mVpz.project().classes()
		.get(mSelectedClass);
	    graph::Model* model = currentClass.model();
	    graph::ModelList lst2;

	    lst2[model->getName()] = model;
	    mCutCopyPaste.copy(lst2, gc,
			       mVpz.project().classes().get(mSelectedClass)
			       .atomicModels(), true);
	} else {
	    mCutCopyPaste.copy(lst, gc,
			       mVpz.project().model().atomicModels(),
			       false);
	}
    } else {
	mCutCopyPaste.copy(lst, gc,
			   mVpz.project().classes().get(className)
			   .atomicModels(), false);
    }
}

void Modeling::paste(graph::CoupledModel* gc, std::string className)
{
    if (className.empty()) {
	mCutCopyPaste.paste(gc, mVpz.project().model().atomicModels());
    } else {
	mCutCopyPaste.paste(gc, mVpz.project().classes().
                            get(className).atomicModels());
    }
}

void Modeling::setModified(bool modified)
{
    if (mIsModified != modified) {
        mIsModified = modified;

        if (mIsModified) {
            setModifiedTitles();
            mGVLE->getMenu()->showSave();
        } else {
            mGVLE->getMenu()->hideSave();
        }
    }
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
    const Strings& vec = atom.conditions();
    for (Strings::const_iterator it_cond = vec.begin(); it_cond != vec.end();
         ++it_cond) {
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

void Modeling::importModel(graph::CoupledModel* parent, vpz::Vpz* src)
{
    using namespace vpz;
    assert(parent);
    assert(src);

    if (mImportModelBox) {
	mImportModelBox->setGCoupled(parent);
	if (mImportModelBox->show(src)) {
	    graph::Model* import = src->project().model().model();
	    parent->addModel(import);
	    if (import->isAtomic())
		import_atomic_model(src, graph::Model::toAtomic(import));
	    else
		import_coupled_model(src, graph::Model::toCoupled(import));
	    dynamics().add(src->project().dynamics());
	    conditions().add(src->project().experiment().conditions());
	    views().add(src->project().experiment().views());
	    mGVLE->redrawModelTreeBox();
	    setModified(true);
	}
    }
}

void Modeling::importModelToClass(vpz::Vpz* src, std::string& className)
{
    using namespace vpz;
    assert(src);
    boost::trim(className);

    if (mImportModelBox) {
	if (mImportModelBox->show(src)) {
	    Classes& classes = vpz().project().classes();
	    Class& new_class = classes.add(className);
	    graph::Model* import = src->project().model().model();
	    if (import->isAtomic()) {
		new_class.setModel(graph::Model::toAtomic(import));
		import_atomic_model(src, graph::Model::toAtomic(import), className);
	    } else {
		new_class.setModel(graph::Model::toCoupled(import));
		import_coupled_model(src, graph::Model::toCoupled(import), className);
	    }
	    dynamics().add(src->project().dynamics());
	    conditions().add(src->project().experiment().conditions());
	    views().add(src->project().experiment().views());
	    mGVLE->redrawModelClassBox();
	    setModified(true);
	}
    }
}

void Modeling::importClasses(vpz::Vpz* src)
{
    using namespace vpz;
    assert(src);

    if (mImportClassesBox) {
	mImportClassesBox->show(src);
    }
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

void parse_recurs(graph::Model* m, vpz::Vpz* vpz, int level)
{
    if (m->isAtomic()) {
        vpz::AtomicModel& atom = vpz->project().model().atomicModels().get(graph::Model::toAtomic(m));
        for (int i = 0; i < level; ++i)
            std::cout << "\t";
        std::cout << m->getName()
        <<" dyn("<<atom.dynamics()<<")"
        <<" obs("<<atom.observables()<<")"
        <<" cond(";
        for (vpz::Strings::const_iterator it = atom.conditions().begin();
                it != atom.conditions().end();
                ++it) {
            std::cout << *it << ";";
        }
        std::cout << ")\n";
    } else {
        for (int i = 0; i < level; ++i)
            std::cout << "\t";
        std::cout << m->getName() << "\n";
        graph::ModelList ml = (graph::Model::toCoupled(m))->getModelList();
        graph::ModelList::iterator it;
        for (it = ml.begin(); it != ml.end(); ++it) {
            parse_recurs(it->second, vpz, level+1);
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


const vpz::Strings* Modeling::get_conditions(graph::AtomicModel* atom)
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

void Modeling::renameModel(graph::Model* model,
                           const string& name,
                           const string& /* description */)
{
    setModified(true);

    SetString::iterator it = mModelsNames.find(model->getName());
    if (it != mModelsNames.end()) {
        mModelsNames.erase(it);
    }

    mModelsNames.insert(name);
    mGVLE->redrawModelTreeBox();
}

void Modeling::delModel(graph::Model* model, std::string className)
{
    if (model->isAtomic()) {
	vpz::AtomicModelList& list = getAtomicModelClass(className);
        list.del(model);
        setModified(true);
    } else {
	graph::ModelList& graphlist =
	    graph::Model::toCoupled(model)->getModelList();
	vpz::AtomicModelList& vpzlist = getAtomicModelClass(className);
	graph::ModelList::iterator it;
	for (it = graphlist.begin(); it!= graphlist.end(); ++it) {
	    if (it->second->isCoupled())
		delModel(it->second, className);
	    else
		vpzlist.del(it->second);
	}
	setModified(true);
    }
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

void parse_model(vpz::AtomicModelList& list)
{
    vpz::AtomicModelList::iterator it = list.begin();
    while (it != list.end()) {
        if (it->first)
            std::cout << "\t" << it->first << " : " << it->first->getName() << "\n";
        else
            std::cout << "\tNULL\n";

        ++it;
    }
}

}
} // namespace vle gvle
