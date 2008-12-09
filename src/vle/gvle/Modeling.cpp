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

using std::string;

namespace vle
{
namespace gvle {

Modeling::Modeling(GVLE* gvle, const string& filename) :
        mTop(NULL),
        mGVLE(gvle),
        mModelTreeBox(new ModelTreeBox(this)),
        //mClassModelTreeBox(new ClassModelTreeBox(this)),
        mModelClassBox(0),
        mIsModified(false),
        mIsSaved(false),
        mSocketPort(8000),
        mAtomicBox(0),
        mImportBox(0),
        mCoupledBox(0)
{
    assert(gvle);

    if (filename.empty() == false) {
        mVpz.parseFile(filename);
        if (mVpz.project().model().model() == 0) {
            gvle::Error(
                (boost::format("Problem loading file %1%, empty structure.") %
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
        setModified(false);
    }
    View* v = new View(this, mTop, mListView.size());
    mListView.push_back(v);
    mModelTreeBox->parseModel(mTop);
}

Modeling::~Modeling()
{
    delete mModelTreeBox;
    delViews();
    if (mAtomicBox)
        delete mAtomicBox;
    if (mImportBox)
        delete mImportBox;
    if (mCoupledBox)
        delete mCoupledBox;
    if (mModelClassBox)
        delete mModelClassBox;
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
    mImportBox = new ImportModelBox(xml, this);
    mCoupledBox = new CoupledModelBox(xml, this);
    mModelClassBox = new ModelClassBox(xml, this);
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
                (boost::format("Problem loading file %1%, empty structure.") %
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
        mModelTreeBox->parseModel(mTop);
        addView(mTop);
        mIsSaved = true;
        mIsModified = false;
        mFileName.assign(filename);
        setTitles();
    } catch (const utils::ParseError& p) {
        gvle::Error((boost::format("Error parsing file, %1%") %
                     p.what()).str());
    } catch (const utils::FileError& f) {
        gvle::Error((boost::format("Error opening file, %1%") %
                     f.what()).str());
    } catch (const Glib::Error& e) {
        gvle::Error((boost::format("Error opening file, %1%") %
                     e.what()).str());
    } catch (const std::exception& e) {
        gvle::Error((boost::format("Error opening file, %1%") %
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
        gvle::Error((boost::format(
                         "Project save problem. Check your file %1%. %2%") % name %
                     file.what()).str());
    } catch (const utils::ParseError& parse) {
        gvle::Error((boost::format(
                         "Project save problem. Parsing error %1%. %2%") % name %
                     parse.what()).str());
    } catch (const std::exception& e) {
        gvle::Error((boost::format(
                         "Project save problem. Exception %1%. %2%") % name %
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
    Error("This function is not yet implemented");
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
            std::cout << "Can't add view for " << model->getName()
            //<< " because " << E.what()
            << "\n";
            std::cout << "looking for " << model << "\n";
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
    for (ListView::const_iterator it = mListView.begin();
         it != mListView.end(); ++it) {
        if (*it) {
            (*it)->iconify();
        }
    }
}

void Modeling::deiconifyViews()
{
    for (ListView::const_iterator it = mListView.begin();
         it != mListView.end(); ++it) {
        if (*it) {
            (*it)->deiconify();
        }
    }
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

void Modeling::redrawModelTreeBox()
{
    assert(mTop);
    mModelTreeBox->parseModel(mTop);
}

void Modeling::showModelTreeBox()
{
    //std::cout << "--- PARSE MODEL ----\n";
    //parse_model( mVpz.project().model().atomicModels() );
    //std::cout << "--------------------\n";
    //mCutCopyPaste.state();
    redrawModelTreeBox();
    mModelTreeBox->show_all();
}

void Modeling::hideModelTreeBox()
{
    redrawModelTreeBox();
    mModelTreeBox->hide();
}

void Modeling::toggleModelTreeBox()
{
    if (mModelTreeBox->is_visible()) {
        hideModelTreeBox();
    } else {
        showModelTreeBox();
    }
}

void Modeling::showRowTreeBox(const std::string& name)
{
    mModelTreeBox->showRow(name);
}

void Modeling::redrawClassModelTreeBox()
{
    std::cout << "TODO: redrawModelClassBox\n";
    //mClassModelTreeBox->parseModel();
}

void Modeling::showClassModelTreeBox()
{
    //redrawClassModelTreeBox();
    //mClassModelTreeBox->show_all();
    mModelClassBox->show();
}

void Modeling::hideClassModelTreeBox()
{
    //redrawClassModelTreeBox();
    //mClassModelTreeBox->hide();
    mModelClassBox->hide();
}

void Modeling::toggleClassModelTreeBox()
{
    //if (mClassModelTreeBox->is_visible()) {
    //  hideClassModelTreeBox();
    //} else {
    //  showClassModelTreeBox();
    //}

    if (mModelClassBox->is_visible()) {
        hideClassModelTreeBox();
    } else {
        showClassModelTreeBox();
    }
}

void Modeling::showRowClassModelTreeBox(const std::string& /*name*/)
{
    //mClassModelTreeBox->showRow(name);
}

bool Modeling::exist(const std::string& name) const
{
    return mTop->findModel(name) != 0;
}

bool Modeling::exist(const graph::Model* m) const
{
    return (mTop->findModel(m->getName()) != 0);
}

void Modeling::cut(graph::ModelList& lst, graph::CoupledModel* gc)
{
    mCutCopyPaste.cut(lst, gc, mVpz.project().model().atomicModels());
}

void Modeling::copy(graph::ModelList& lst, graph::CoupledModel* gc)
{
    mCutCopyPaste.copy(lst, gc, mVpz.project().model().atomicModels());
}

void Modeling::paste(graph::CoupledModel* gc)
{
    mCutCopyPaste.paste(gc, mVpz.project().model().atomicModels());
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

void Modeling::exportCoupledModel(graph::CoupledModel* model, vpz::Vpz* dst)
{
    vpz::Project& project = dst->project();
    project.setAuthor(vpz().project().author());
    project.setDate(vpz().project().date());
    project.setVersion(vpz().project().version());
    vpz::Experiment& experiment = project.experiment();
    experiment.setName(vpz().project().experiment().name());
    experiment.setDuration(vpz().project().experiment().duration());

    export_coupled_model(dst, model);
    dst->project().model().setModel(model);
    dst->write();
}

void Modeling::export_atomic_model(vpz::Vpz* dst, graph::AtomicModel* model)
{
    using namespace vpz;

    AtomicModel& atom = get_model(dynamic_cast<graph::AtomicModel*>(model));
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
    if (vpz().project().model().atomicModels().exist(model)) {
        dst->project().model().atomicModels().add(model, atom);
    }
}

void Modeling::export_coupled_model(vpz::Vpz* dst, graph::CoupledModel* model)
{
    graph::ModelList& list = model->getModelList();
    graph::ModelList::const_iterator it = list.begin();
    while (it!= list.end()) {
        if (it->second->isAtomic()) {
            export_atomic_model(dst, dynamic_cast<graph::AtomicModel*>(it->second));
        } else {
            export_coupled_model(dst, dynamic_cast<graph::CoupledModel*>(it->second));
        }

        ++it;
    }
}

void Modeling::importCoupledModel(graph::CoupledModel* parent, vpz::Vpz* src)
{
    using namespace vpz;
    assert(parent);
    assert(src);

    if (mImportBox && mImportBox->show(src)) {
        graph::Model* import = src->project().model().model();
        parent->addModel(import);
        import_coupled_model(src, dynamic_cast<graph::CoupledModel*>(import));
        dynamics().add(src->project().dynamics());
        conditions().add(src->project().experiment().conditions());
        measures().add(src->project().experiment().views());
        mModelTreeBox->parseModel(mTop);
    }
}

void Modeling::import_atomic_model(vpz::Vpz* src, graph::AtomicModel* atom)
{
    using namespace vpz;

    AtomicModel& vpz_atom = src->project().model().atomicModels().get(atom);
    vpz().project().model().atomicModels().add(atom, vpz_atom);
}

void Modeling::import_coupled_model(vpz::Vpz* src, graph::CoupledModel* model)
{
    graph::ModelList& list = model->getModelList();
    if (!list.empty()) {
        graph::ModelList::const_iterator it = list.begin();
        while (it!= list.end()) {
            if (it->second->isAtomic()) {
                import_atomic_model(src, dynamic_cast<graph::AtomicModel*>(it->second));
            } else {
                import_coupled_model(src, dynamic_cast<graph::CoupledModel*>(it->second));
            }

            ++it;
        }
    }
}

void parse_recurs(graph::Model* m, vpz::Vpz* vpz, int level)
{
    if (m->isAtomic()) {
        vpz::AtomicModel& atom = vpz->project().model().atomicModels().get(dynamic_cast<graph::AtomicModel*>(m));
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
        graph::ModelList ml = (dynamic_cast<graph::CoupledModel*>(m))->getModelList();
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
    if (not parent or not parent->exist(name)) {
        setModified(true);
        graph::CoupledModel* tmp = new graph::CoupledModel(name, parent);
        tmp->setPosition(x, y);
        return tmp;
    }
    return 0;
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
    Throw(utils::NotYetImplemented, "Modeling::newPluginModel");
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
    //gvle::Error((boost::format(
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

void Modeling::delModel(graph::Model* model)
{
    if (model->isAtomic()) {
        vpz::AtomicModelList& list = mVpz.project().model().atomicModels();
        list.del(model);
    }
}

void Modeling::vpz_is_correct(std::vector<std::string>& vec)
{
    using namespace vpz;

    const Project& project = mVpz.project();

    if (project.author() == "") {
        vec.push_back("The author is not mentioned.");
    }

    const AtomicModelList& atomic_list = project.model().atomicModels();
    AtomicModelList::const_iterator atomic_it = atomic_list.begin();
    while (atomic_it != atomic_list.end()) {
        if (atomic_it->second.dynamics() == "") {
            vec.push_back("The Atomic Model '"+atomic_it->first->getName()+"' is not linked to a dynamic.");
        }
        ++atomic_it;
    }

    const DynamicList& dyn_list = project.dynamics().dynamiclist();
    DynamicList::const_iterator dyn_it = dyn_list.begin();
    while (dyn_it != dyn_list.end()) {
        if (dyn_it->second.library() == "") {
            vec.push_back("The Dynamic '"+dyn_it->first+"' is not attached to a library.");
        }
        ++dyn_it;
    }

    const Experiment& exp = project.experiment();
    if (exp.name() == "")
        vec.push_back("The Experiment has not a name.");

    if (exp.duration() == 0)
        vec.push_back("The Experiment has not a duration.");

    const ViewList& view_list = exp.views().viewlist();
    ViewList::const_iterator view_it = view_list.begin();
    while (view_it != view_list.end()) {
        if (view_it->second.output() == "") {
            vec.push_back("The view '"+view_it->first+"' has not an output.");
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
