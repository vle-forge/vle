/**
 * @file vle/gvle/Modeling.cpp
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


#include <vle/gvle/AtomicModelBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/View.hpp>
#include <vle/gvle/AtomicModelBox.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/ModelDescriptionBox.hpp>
#include <vle/gvle/DialogString.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Path.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/graph/Model.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <libxml++/libxml++.h>
#include <gtkmm/stock.h>
#include <iostream>

using std::string;

namespace vle
{
namespace gvle {

Modeling::Modeling(GVLE* gvle, const string& filename) :
    mTop(NULL),
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

    if (filename.empty() == false) {
        mVpz.parseFile(filename);
        if (mVpz.project().model().model() == 0) {
            gvle::Error(
                (fmt(_("Problem loading file %1%, empty structure.")) %
                 filename).str());
            mTop = newCoupledModel(0, "Top model", "", 0, 0);
            //mTop->setParent(NULL);
            mVpz.project().model().setModel(mTop);
        } else if (mVpz.project().model().model()->isAtomic()) {
            graph::CoupledModel* t = newCoupledModel(0, "Top model", "", 0,
                                     0);
            //t->setParent(NULL);
            t->addModel(mVpz.project().model().model());
            mVpz.project().model().setModel(t);
            mTop = t;
            setTitles();
        }
    } else {
        mTop = newCoupledModel(0, "Top model", "", 0, 0);
        //mTop->setParent(NULL);
        mVpz.project().model().setModel(mTop);
        setTitles();
    }
    std::string name = "";
    View* v = new View(this, mTop, mListView.size());
    mListView.push_back(v);
    setModified(false);
}

Modeling::~Modeling()
{
    delViews();
    delete mAtomicBox;
    delete mImportModelBox;
    delete mImportClassesBox;
    delete mCoupledBox;
}

void Modeling::clearModeling()
{
    mIsModified = false;
    mIsSaved = false;
    mFileName.clear();
    mVpz.clear();
    delViews();
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
    mTop = newCoupledModel(0, "Top model", "", 0, 0);
    mVpz.project().model().setModel(mTop);
    addView(mTop);
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
        mGVLE->redrawModelTreeBox();
	mGVLE->redrawModelClassBox();
	mFileName.assign(filename);
        addView(mTop);
        mIsSaved = true;
        mIsModified = false;
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
        mIsModified = false;
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


/*
  graph::CoupledModel* Modeling::getClassModel(const std::string& name)
  {
  graph::CoupledModel* result = 0;
  ;
  MapStringGCoupledModel::iterator it = mClassModel.find(name);
  if (it != mClassModel.end()) {
  result = (*it).second;
  }
  return result;
  }

  void Modeling::addClassModel(const std::string& name)
  {
  GCoupledModel* result = getClassModel(name);
  if (result == NULL) {
  result = new GCoupledModel(0, new graph::CoupledModel(NULL), 0, 0);
  result->setName(name);
  result->setDescription("class model");
  mModelsNames.insert(name);
  mClassModel[name] = result;
  } else {
  g_print("model %s existant\n", name.c_str());
  }
  }

  void Modeling::delClassModel(const std::string& name)
  {
  MapStringGCoupledModel::iterator it = mClassModel.find(name);
  if (it != mClassModel.end()) {
  delete (*it).second;
  mClassModel.erase(it);
  }
  }
*/

void Modeling::showDynamics(const std::string& /* name */)
{
    Error(_("This function is not yet implemented"));
}

/*void Modeling::addView(const string& name)
  {
  graph::Model* model = 0;
  model = mTop->findModel(name);
  if (model == NULL) {
  //model = getClassModel(name);
  //std::cout << "find Model '"<<name<<"' non trouvé\n";
  } else {
  addView(model);
  }
  }*/

void Modeling::addView(graph::Model* model)
{
    assert(model);
    if (model->isCoupled()) {
        graph::CoupledModel* m = (graph::CoupledModel*)(model);
        addView(m);
    } else if (model->isAtomic()) {
        graph::AtomicModel* graph_am = dynamic_cast<graph::AtomicModel*>(model);
        try {
            vpz::AtomicModel& vpz_am = get_model(graph_am);
            mAtomicBox->show(vpz_am, *graph_am);
        } catch (utils::SaxParserError& E) {
            parse_model(mVpz.project().model().atomicModels());
        }

        //graph::AtomicModel* m = (graph::AtomicModel*)(model);
        /*Plugin* p = getPlugin(
        mVpz.project().dynamics().get(model->getName()).model());
        if (p) {
        //            std::string structure(gui::Save::saveStructures(m->getModel()));
        //            std::string dynamic(((GAtomicModel*)m)->getDynamic());
        //
        std::string structure("TODO");
        std::string dynamic("TODO");

        //          if (p->getNextXML(structure, dynamic)) {
        //                ((graph::AtomicModel*)m)->setDynamic(dynamic);
        //                ((graph::AtocmiModel*)m)->setPixBuf(p->getImage());
        //                gui::Load::attachPortToStructure(m->getModel(), structure);
        }
        } else {
        //AtomicModelBox* box = new AtomicModelBox(m);
        //if (box->run() == Gtk::RESPONSE_OK) {
        //box->adoptModification(this);
        //}
        //delete box;
        }
        */
    }
    refreshViews();
}

void Modeling::addView(graph::CoupledModel* model)
{
    const size_t szView = mListView.size();
    /*size_t firstEmpty = 0;
    size_t foundedModel = 0;

    for (size_t i = 0; i < szView; ++i) {
      if (mListView[i] == 0) {
    if (mListView[firstEmpty] != 0) {
    firstEmpty = i;
    }
      } else {
    if (mListView[i]->getGCoupledModel() == model) {
    mListView[i]->selectedWindow();
    return;
    }
      }
    }
    if (firstEmpty == 0 and mListView[firstEmpty] == 0) {
      mListView[foundedModel] = new View(this, model, foundedModel);
    } else {
      mListView.push_back(new View(this, model, szView));
    }
    */
    View* search = findView(model);
    if (search != NULL) {
        search->selectedWindow();
    } else {
        mListView.push_back(new View(this, model, szView));
	mGVLE->openTabVpz(mFileName, model);
    }
}

void Modeling::addViewClass(graph::Model* model, std::string name)
{
    assert(model);
    if (model->isCoupled()) {
        graph::CoupledModel* m = (graph::CoupledModel*)(model);
        addViewClass(m, name);
    } else if (model->isAtomic()) {
        graph::AtomicModel* graph_am = dynamic_cast<graph::AtomicModel*>(model);
        try {
	    vpz::AtomicModel& vpz_am = get_model(graph_am, name);
	    mAtomicBox->show(vpz_am, *graph_am);
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
	mGVLE->openTabVpz(mFileName, model);
    }
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

void Modeling::iconifyViews()
{
    /*for (ListView::const_iterator it = mListView.begin();
         it != mListView.end(); ++it) {
        if (*it) {
            (*it)->iconify();
        }
	}*/
}

void Modeling::deiconifyViews()
{
    /*for (ListView::const_iterator it = mListView.begin();
         it != mListView.end(); ++it) {
        if (*it) {
            (*it)->deiconify();
        }
	}*/
}

void Modeling::refreshViews()
{
    ListView::iterator it = mListView.begin();
    while (it != mListView.end()) {
        if (*it)
            (*it)->redraw();
        ++it;
    }
}

/*
  Modeling::SetString Modeling::getSimulationPlugin() const
  {
  std::set < std::string > modelNames;

  //{
  //Glib::Dir repertoire(utils::Path::path().getDefaultModelDir());
  //Glib::DirIterator it = repertoire.begin();
  //while (it != repertoire.end()) {
  //if ((*it).find("-tr", 0) == std::string::npos) {
  //std::string selected(*it);
  //if (selected.rfind('.') != std::string::npos) {
  //selected.assign(selected, 0, selected.rfind('.'));
  //}
  //if (selected.find("lib") == 0) {
  //selected.assign(selected, 3, selected.size());
  //}
  //modelNames.insert(selected);
  //}
  //++it;
  //}
  //}
  //{
  //Glib::Dir repertoire(utils::Path::path().getUserModelDir());
  //Glib::DirIterator it = repertoire.begin();
  //while (it != repertoire.end()) {
  //if ((*it).find("-tr", 0) == std::string::npos) {
  //std::string selected(*it);
  //if (selected.rfind('.') != std::string::npos) {
  //selected.assign(selected, 0, selected.rfind('.'));
  //}
  //if (selected.find("lib") == 0) {
  //selected.assign(selected, 3, selected.size());
  //}
  //modelNames.insert(selected);
  //}
  //++it;
  //}
  //}
  return modelNames;
  }*/

void Modeling::EditCoupledModel(graph::CoupledModel* model)
{
    assert(model);
    mCoupledBox->show(model);
}

void Modeling::setTitles()
{
    mGVLE->setTitle(mFileName);

    for (ListView::iterator it = mListView.begin(); it != mListView.end(); ++it)
    {
        if (*it) {
            (*it)->setTitle(mFileName);
        }
    }
}

void Modeling::setModifiedTitles()
{
    mGVLE->setModifiedTitle();

    for (ListView::iterator it = mListView.begin(); it != mListView.end(); ++it)
    {
        if (*it) {
            (*it)->setModifiedTitle();
        }
    }
}

void Modeling::redrawModelTreeBox()
{
    assert(mTop);
    mGVLE->redrawModelTreeBox();
}

void Modeling::showModelTreeBox()
{
    //std::cout << "--- PARSE MODEL ----\n";
    //parse_model( mVpz.project().model().atomicModels() );
    //std::cout << "--------------------\n";
    //mCutCopyPaste.state();
    redrawModelTreeBox();
}

void Modeling::hideModelTreeBox()
{
    redrawModelTreeBox();
}

void Modeling::toggleModelTreeBox()
{

}

void Modeling::showRowTreeBox(const std::string& name)
{
    mModelTreeBox->showRow(name);
}


void Modeling::redrawModelClassBox()
{
    mGVLE->redrawModelClassBox();
}

void Modeling::showModelClassBox()
{
    redrawModelClassBox();
    mModelClassBox->show_all();
}

void Modeling::hideClassModelTreeBox()
{
    //mClassModelTreeBox->hide();
}

void Modeling::hideModelClassBox()
{
    redrawModelClassBox();
}

void Modeling::toggleModelClassBox()
{
}

void Modeling::showRowModelClassBox(const std::string& name)
{
    mModelClassBox->showRow(name);
}

bool Modeling::exist(const std::string& name) const
{
    return mTop->findModel(name) != 0;
}

bool Modeling::exist(const graph::Model* m) const
{
    return (mTop->findModel(m->getName()) != 0);
}

void Modeling::cut(graph::ModelList& lst, graph::CoupledModel* gc, std::string className)
{
    if (className == "") {
	mCutCopyPaste.cut(lst, gc, mVpz.project().model().atomicModels());
    } else {
	mCutCopyPaste.cut(lst, gc, mVpz.project().classes().get(className).atomicModels());
    }
}

void Modeling::copy(graph::ModelList& lst, graph::CoupledModel* gc, std::string className)
{
    if (className == "") {
	mCutCopyPaste.copy(lst, gc, mVpz.project().model().atomicModels());
    } else {
	mCutCopyPaste.copy(lst, gc, mVpz.project().classes().get(className).atomicModels());
    }
}

void Modeling::paste(graph::CoupledModel* gc, std::string className)
{
    if (className == "") {
	mCutCopyPaste.paste(gc, mVpz.project().model().atomicModels());
    } else {
	mCutCopyPaste.paste(gc, mVpz.project().classes().get(className).atomicModels());
    }
    /*
    std::cout << "Atomic List :\n";
    const vpz::AtomicModelList& list = mVpz.project().model().atomicModels();
    vpz::AtomicModelList::const_iterator it = list.begin();
    while(it != list.end()) {
    std::cout << "\t" << it->first->getName() << "\n";

    ++it;
    }
    */
}

void Modeling::setModified(bool modified)
{
    if (mIsModified != modified) {
        mIsModified = modified;

        if (mIsModified) {
            setModifiedTitles();
        }
    }
}

void Modeling::exportCoupledModel(graph::CoupledModel* model, vpz::Vpz* dst, std::string className)
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

void Modeling::exportClass(graph::Model* model, vpz::Class classe, vpz::Vpz* dst)
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
	export_coupled_model(dst, dynamic_cast<graph::CoupledModel*>(model), classeName);
    } else {
	export_atomic_model(dst, dynamic_cast<graph::AtomicModel*>(model), classeName);
    }
    dst->project().model().setModel(model);
    dst->write();
}

void Modeling::export_atomic_model(vpz::Vpz* dst, graph::AtomicModel* model,
				   std::string className)
{
    using namespace vpz;

    AtomicModel atom = get_model(dynamic_cast<graph::AtomicModel*>(model), className);

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
	    mModelTreeBox->parseModel(mTop);
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
	    mModelClassBox->parseClass();
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
    gvle::DialogString* box;
    box = new gvle::DialogString("Get a new name", "Name", lst);


    Glib::ustring error = box->run();

    if (error.empty() == true) {
        name.assign(box->get_string());
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
/*
  vpz::Observable* Modeling::get_observables(graph::AtomicModel* atom){
  assert(atom);

  vpz::AtomicModel a = mVpz.project().model().atomicModels().get(atom);
  if (a.observables() != "") {
  vpz::Observable obs = observables().get(a.observables());

  vpz::ObservableList list = observables().observablelist();
  vpz::ObservableList::iterator it = list.begin();

  while ( it != list.end() ) {
  if ( it->first == a.observables() ) {
  return &it->second;
  }
  it++;
  }
  }
  return NULL;
  }
*/

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
        return tmp;
    }
    return 0;
}

graph::Model* Modeling::newPluginModel(graph::CoupledModel* /*parent*/,
                                       const string& /*name*/,
                                       const string& /*description*/,
                                       int /*x*/, int /*y*/)
{
    throw utils::NotYetImplemented("Modeling::newPluginModel");
    ////assert(parent);
    //assert(not name.empty());
    //assert(getPlugin());
    //
    //graph::Model* new_plugin = 0;
    //
    //try {
    //std::string xmlstructure;
    //std::string xmldynamics;
    //
    //vpz::Model mdl;
    //vpz::Dynamics dyn;
    //
    //if (getPlugin()->getXML(xmlstructure, xmldynamics)) {
    //
    //{
    //xmlpp::DomParser dom;
    //dom.get_document()->create_root_node("STRUCTURES");
    //utils::xml::import_children_nodes(
    //dom.get_document()->get_root_node(), xmlstructure);
    //xmlpp::Element* root = dom.get_document()->get_root_node();
    //xmlpp::Element* model = utils::xml::get_children(root, "MODEL");
    //assert(model);
    //model->set_attribute("NAME", name);
    //mdl.init(utils::xml::get_root_node(dom, "STRUCTURES"));
    //}
    //
    //{
    //xmlpp::DomParser dom;
    //xmlpp::Element* root;
    //root = dom.get_document()->create_root_node("DYNAMICS");
    //root = root->add_child("MODELS");
    //utils::xml::import_children_nodes(root, xmldynamics);
    //xmlpp::Element* model = utils::xml::get_children(root, "MODEL");
    //assert(model);
    //model->set_attribute("NAME", name);
    //dyn.init(utils::xml::get_root_node(dom, "DYNAMICS"));
    //}
    //
    //if (mdl.modelRef()) {
    //std::cerr << xmldynamics << std::endl;
    //graph::Model* newmdl = mdl.model();
    //newmdl->setPosition(x, y);
    //newmdl->setName(name);
    //newmdl->setDescription(description);
    //parent->addModel(newmdl);
    //
    //mVpz.project().dynamics().addDynamics(dyn);
    //// FIXME image
    //redrawModelTreeBox();
    //mModelsNames.insert(name);
    //}
    //}
    //} catch(const std::exception& e) {
    //gvle::Error((fmt(
    //"Failed to build the new plugin '%1%':\n%2%") % name %
    //e.what()).str());
    //}
    //
    //return new_plugin;
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
    redrawModelTreeBox();
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

    const AtomicModelList& atomic_list = project.model().atomicModels();
    AtomicModelList::const_iterator atomic_it = atomic_list.begin();
    while (atomic_it != atomic_list.end()) {
        if (atomic_it->second.dynamics() == "") {
            vec.push_back((fmt(_("The Atomic Model %1% is not linked to a "
                                 "dynamic.")) %
                           atomic_it->first->getName()).str());

        }
        ++atomic_it;
    }

    const DynamicList& dyn_list = project.dynamics().dynamiclist();
    DynamicList::const_iterator dyn_it = dyn_list.begin();
    while (dyn_it != dyn_list.end()) {
        if (dyn_it->second.library() == "") {
            vec.push_back((fmt(_("The Dynamic %1% is not attached to a "
                                 "library.")) % dyn_it->first).str());
        }
        ++dyn_it;
    }

    const Experiment& exp = project.experiment();
    if (exp.name() == "")
        vec.push_back(_("The Experiment has not a name."));

    if (exp.duration() == 0)
        vec.push_back(_("The Experiment has not a duration."));

    const ViewList& view_list = exp.views().viewlist();
    ViewList::const_iterator view_it = view_list.begin();
    while (view_it != view_list.end()) {
        if (view_it->second.output() == "") {
            vec.push_back((fmt(_("The view %1% has not an output.")) %
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
