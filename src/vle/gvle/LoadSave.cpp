/**
 * @file vle/gvle/LoadSave.cpp
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


#include <vle/gvle/LoadSave.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/graph/Model.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/NoVLEModel.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Tools.hpp>
#include <libxml++/libxml++.h>

using std::string;
using namespace vle;

namespace vle
{
namespace gvle {

gui::Load::Load(const string& filename, Modeling* mod) :
        IO(mod),
        mHasGraphics(false),
        mHasExperiments(false)
{
    utils::Load xml(filename, false, false);

    loadStructure(xml.getProjectStructures());

    if (xml.getProjectGraphics()) {
        loadGraphics(xml.getProjectGraphics());
    } else {
        mGModelHead = cloneModelToGModel(mModelHead, mMMM, 0);
    }

    buildNoVLES(xml.getProjectNoVLE());

    loadDynamics(xml.getProjectDynamics());

    if (xml.getProjectExperiments()) {
        loadExperiments(xml.getProjectExperiments());
    }
}

void gui::Load::addModelName(const std::string& name, std::string& structure,
                             std::string& dynamics, std::string& graphics)
{
    if (structure.empty() == false) {
        xmlpp::DomParser dom;
        try {
            dom.parse_memory(structure);
        } catch (const xmlpp::exception& e) {
            Assert(Exception::Parse, false, "Bad XML structures");
        }
        xmlpp::Element* root = utils::XML::get_root_node(dom, "MODEL");
        root->set_attribute("NAME", name);
        structure.assign(dom.get_document()->write_to_string());
    }

    if (dynamics.empty() == false) {
        xmlpp::DomParser dom;
        try {
            dom.parse_memory(dynamics);
        } catch (const xmlpp::exception& e) {
            Assert(Exception::Parse, false, "Bad XML dynamics");
        }
        xmlpp::Element* root = utils::XML::get_root_node(dom, "MODEL");
        root->set_attribute("NAME", name);
        dynamics.assign(dom.get_document()->write_to_string());
    }

    if (graphics.empty() == false) {
        xmlpp::DomParser dom;
        try {
            dom.parse_memory(graphics);
        } catch (const xmlpp::exception& e) {
            Assert(Exception::Parse, false, "Bad XML dynamics");
        }
        xmlpp::Element* root = utils::XML::get_root_node(dom, "MODEL");
        root->set_attribute("NAME", name);
        graphics.assign(dom.get_document()->write_to_string());
    }
}

GModel* Load::extractTreeStructures(GCoupledModel* parent, const string&
                                    structures, const string& dynamics, const string& graphics)
{
    xmlpp::DomParser dom;
    try {
        dom.parse_memory(structures);
    } catch (const xmlpp::exception& e) {
        Assert(Exception::Parse, false, "Bad XML structure");
    }
    MapModelGModel mmm_models;

    graph::Model* d_models = graph::Model::parseXMLmodel
                             (dom.get_document()->get_root_node(), 0);
    GModel* g_models = cloneModelToGModel(d_models, mmm_models, parent);

    if (graphics.empty() == false) {
        Load::attachGraphicsToModel(mmm_models, graphics);
    }

    if (dynamics.empty() == false) {
        Load::attachDynamicsToModel(mmm_models, dynamics);
    }

    parent->attachChildrenModel(g_models);
    return g_models;
}

GModel* Load::cloneModelToGModel(graph::Model* top, MapModelGModel& mmm,
                                 GCoupledModel* parent)
{
    GModel* result = NULL;

    if (top) {
        if (top->isCoupled()) {
            graph::CoupledModel* topCoupled = graph::Model::toCoupled(top);

            if (mmm.find(topCoupled) != mmm.end()) {
                throw Exception::Parse("Cannot found a model to clone");
            }

            result = new GCoupledModel(parent, topCoupled, 0, 0);
            graph::VectorModel children_list = topCoupled->getModelList();
            for (graph::VectorModel::const_iterator it = children_list.begin();
                    it != children_list.end(); ++it) {
                GModel* child = cloneModelToGModel(*it, mmm, parent);
                GModel::toGCoupledModel(result)->attachChildrenModel(child);
            }
        } else if (top->isAtomic()) {
            result = new GAtomicModel(parent, graph::Model::toAtomic(top), 0, 0);
        } else if (top->isNoVLE()) {
            result = new GNoVLEModel(parent, graph::Model::toNoVLE(top), 0, 0);
        }
        mmm[top] = result;
    }
    return result;
}

void Load::attachGraphicsToModel(MapModelGModel& mmm, const string& graphics)
{
    xmlpp::DomParser dom;
    try {
        dom.parse_memory(graphics);
    } catch (const xmlpp::exception& e) {
        Assert(Exception::Parse, false, "Bad XML graphics");
    }
    xmlpp::Element* root = dom.get_document()->get_root_node();
    if (root->get_name() == "MODEL") {
        const string name = utils::XML::get_attribute(root, "NAME");
        GModel* current = findModelFromMapModelGModel(name, mmm);
        Assert(Exception::Parse, current, "Model not found in structures");
        loadGraphicsData(root, current);
    } else {
        xmlpp::Node::NodeList lst_model = root->get_children("MODEL");
        xmlpp::Node::NodeList::iterator it = lst_model.begin();
        while (it != lst_model.end()) {
            xmlpp::Element* mdl = utils::XML::cast_node_element(*it);
            const string name = utils::XML::get_attribute(mdl, "NAME");
            GModel* current = findModelFromMapModelGModel(name, mmm);
            Assert(Exception::Parse, current, "Model not found in structures");
            loadGraphicsData(mdl, current);
            ++it;
        }
    }
}

void Load::attachDynamicsToModel(MapModelGModel& mmm, const string& dynamics)
{
    xmlpp::DomParser dom;
    try {
        dom.parse_memory(dynamics);
    } catch (const xmlpp::exception& e) {
        Assert(Exception::Parse, false, "Bad XML dynamics");
    }
    xmlpp::Element* root = dom.get_document()->get_root_node();
    if (root->get_name() == "MODEL") {
        const string name = utils::XML::get_attribute(root, "NAME");
        GModel* current = findModelFromMapModelGModel(name, mmm);
        Assert(Exception::Parse, current, "Model not found in structures");
        loadDynamicsData(root, current);
    } else {
        xmlpp::Node::NodeList lst_model = root->get_children("MODEL");
        xmlpp::Node::NodeList::iterator it = lst_model.begin();
        while (it != lst_model.end()) {
            xmlpp::Element* mdl = utils::XML::cast_node_element(*it);
            const string name = utils::XML::get_attribute(mdl, "NAME");
            GModel* current = findModelFromMapModelGModel(name, mmm);
            Assert(Exception::Parse, current, "Model not found in structures");
            loadDynamicsData(mdl, current);
            ++it;
        }
    }
}

GModel* Load::findModelFromMapModelGModel(const std::string& name,
        MapModelGModel& mmm)
{
    GModel* founded = NULL;
    MapModelGModel::iterator jt = mmm.begin();
    while (jt != mmm.end()) {
        if ((*jt).first->getName() == name) {
            founded = (*jt).second;
            break;
        }
        ++jt;
    }
    return founded;
}

void Load::attachPortToStructure(graph::Model* model,
                                 const Glib::ustring& xml)
{
    if (model) {
        xmlpp::DomParser dom;
        xmlpp::Element* root = 0;
        xmlpp::Element* elt = 0;
        std::list < std::string > inlist, outlist, statelist, initlist;
        xmlpp::Node::NodeList::iterator it;

        dom.parse_memory(xml);
        root = utils::XML::get_root_node(dom, "MODEL");

        if (utils::XML::exist_children(root, "IN")) {
            elt = utils::XML::get_children(root, "IN");
            xmlpp::Node::NodeList lst = elt->get_children("PORT");
            for (it = lst.begin(); it != lst.end(); ++it)
                inlist.push_back(utils::XML::get_attribute((*it), "NAME"));
        }
        if (utils::XML::exist_children(root, "OUT")) {
            elt = utils::XML::get_children(root, "OUT");
            xmlpp::Node::NodeList lst = elt->get_children("PORT");
            for (it = lst.begin(); it != lst.end(); ++it)
                outlist.push_back(utils::XML::get_attribute((*it), "NAME"));
        }
        if (utils::XML::exist_children(root, "STATE")) {
            elt = utils::XML::get_children(root, "STATE");
            xmlpp::Node::NodeList lst = elt->get_children("PORT");
            for (it = lst.begin(); it != lst.end(); ++it)
                statelist.push_back(utils::XML::get_attribute((*it), "NAME"));
        }
        if (utils::XML::exist_children(root, "INIT")) {
            elt = utils::XML::get_children(root, "INIT");
            xmlpp::Node::NodeList lst = elt->get_children("PORT");
            for (it = lst.begin(); it != lst.end(); ++it)
                initlist.push_back(utils::XML::get_attribute((*it), "NAME"));
        }
        model->mergePort(inlist, outlist, statelist, initlist);
    }
}

void Load::fillMapStringModel(graph::Model* m)
{
    if (m) {
        mMsm[m->getName()] = m;
        if (m->isCoupled()) {
            graph::CoupledModel* coupled = graph::Model::toCoupled(m);
            graph::VectorModel children_list = coupled->getModelList();
            for (graph::VectorModel::const_iterator it = children_list.begin();
                    it != children_list.end(); ++it) {
                if ((*it)->isCoupled()) {
                    fillMapStringModel(*it);
                } else {
                    mMsm[(*it)->getName()] = *it;
                }
            }
        }
    }
}

void Load::loadStructure(xmlpp::Element* root)
{
    std::cout << "loadStructure\n";

    mModelHead = graph::Model::parseXMLmodel(root, NULL);
    if (mModelHead->isAtomic() or mModelHead->isNoVLE()) {
        if (mModelHead->getName() != "Top model") {
            graph::CoupledModel* newc = new graph::CoupledModel(0);
            newc->setName("Top model");
            newc->attachModel(mModelHead);
            mModelHead = newc;
        } else {
            Assert(Exception::Parse, false,
                   "Top model is an atomic model with Top model has name");
        }
    }
    fillMapStringModel(mModelHead);
}

void Load::loadGraphics(xmlpp::Element* root)
{
    GModel* result = NULL;

    result = cloneModelToGModel(mModelHead, mMMM, 0);

    xmlpp::Node::NodeList lst_model = root->get_children("MODEL");
    xmlpp::Node::NodeList::iterator it = lst_model.begin();
    while (it != lst_model.end()) {
        xmlpp::Element* mdl = utils::XML::cast_node_element(*it);
        const string name = utils::XML::get_attribute(mdl, "NAME");
        const string type = utils::XML::get_attribute(mdl, "TYPE");

        graph::Model* current = getModel(name);
        Assert(Exception::Parse, current,
               "graph::Model name not found in structures");

        GModel* gcurrent = getModel(current);
        Assert(Exception::Parse, gcurrent, "GModel not found in structures");

        loadGraphicsData(mdl, gcurrent);
        ++it;
    }
    mGModelHead = result;
}

void Load::loadGraphicsData(xmlpp::Element* node, GModel* model)
{
    if (node != NULL and model != NULL) {
        xmlpp::Element* position = utils::XML::get_children(node, "POSITION");
        const string x(utils::XML::get_attribute(position, "X"));
        const string y(utils::XML::get_attribute(position, "Y"));
        model->setPosition(atoi(x.c_str()), atoi(y.c_str()));

        if (model->isGCoupledModel()) {
            GCoupledModel* gmodel = GModel::toGCoupledModel(model);
            xmlpp::Element* size = utils::XML::get_children(node, "SIZE");
            const string width(utils::XML::get_attribute(size, "WIDTH"));
            const string height(utils::XML::get_attribute(size, "HEIGHT"));
            gmodel->setSizeView(atoi(width.c_str()), atoi(height.c_str()));
        }
    }
}

void Load::loadDynamics(xmlpp::Element* root)
{
    AssertS(Exception::Parse, root->get_name() == "MODELS");

    xmlpp::Element::NodeList lst = root->get_children("MODEL");
    for (xmlpp::Element::NodeList::iterator it = lst.begin();
            it != lst.end(); ++it) {
        xmlpp::Element* model = utils::XML::cast_node_element(*it);

        string name(utils::XML::get_attribute(model, "NAME"));

        MapStringModel::const_iterator it = mMsm.find(name);
        Assert(Exception::Parse, it != mMsm.end(),
               "Error MODEL not exist in MODEL list");

        Assert(Exception::Parse, (*it).second->isAtomic(),
               "Not apply Dynamics to CoupledModel");

        GAtomicModel* m = GModel::toGAtomicModel(mMMM[(*it).second]);

        xmlpp::Element* dyn = utils::XML::get_children(model, "DYNAMICS");
        string s_dynamic(utils::XML::write_to_string(model));
        string s_formalism(utils::XML::get_attribute(dyn, "FORMALISM"));

        m->setDynamic(s_dynamic);
        m->setFormalism(s_formalism);

        Plugin* p = mModeling->getPlugin(s_formalism);
        if (p == NULL) {
            g_warning("Be carrefull, no formalism for model %s.",
                      name.c_str());
        } else {
            m->setPixBuf(p->getImage());
        }
    }
}

void Load::loadDynamicsData(xmlpp::Element* elt, GModel* mdl)
{
    xmlpp::Element* dyn = utils::XML::get_children(elt, "DYNAMICS");
    string s_dynamic(utils::XML::write_to_string(elt));
    string s_formalism(utils::XML::get_attribute(dyn, "FORMALISM"));

    if (mdl->isGAtomicModel()) {
        GAtomicModel* atomic = GModel::toGAtomicModel(mdl);
        atomic->setDynamic(s_dynamic);
        atomic->setFormalism(s_formalism);
    }
}

void Load::buildNoVLES(const utils::XMLNoVLEList& lst)
{
    std::cout << "buildNoVLES: " <<  lst.size() << "\n";

    for (utils::XMLNoVLEList::const_iterator it = lst.begin();
            it != lst.end(); ++it) {

        AssertI(mGModelHead->isGCoupledModel());
        GModel* found = ((GCoupledModel*)mGModelHead)->findModel((*it).modelname);

        Assert(Exception::Parse, found->isGNoVLEModel(),
               utils::ucompose("Model %1 is not a NoVLE model.", (*it).modelname));

        ((GNoVLEModel*)found)->setTranslatorName((*it).translatorname);
        ((GNoVLEModel*)found)->setXML((*it).xml);
    }
}

void Load::loadExperiments(xmlpp::Element* root)
{
    AssertS(Exception::Parse, root->get_name() == "EXPERIMENT");

    xmlpp::Element* experimentalconditions = 0;
    xmlpp::Element* measures = 0;
    xmlpp::Element* outputs = 0;

    if (utils::XML::exist_children(root, "EXPERIMENTAL_CONDITIONS")) {
        experimentalconditions = utils::XML::get_children(root, "EXPERIMENTAL_CONDITIONS");
        readExperimentalCondition(experimentalconditions);
    }

    mModeling->setDuration(
        utils::to_double(utils::XML::get_attribute(root, "DURATION")));
    mModeling->setExperimentName(
        utils::XML::get_attribute(root, "NAME"));

    if (utils::XML::exist_children(root, "MEASURES")) {
        measures = utils::XML::get_children(root, "MEASURES");
        readMeasures(measures);
    }

    if (measures and utils::XML::exist_children(measures, "OUTPUTS")) {
        outputs = utils::XML::get_children(measures, "OUTPUTS");
        readOutputs(outputs);
    }
}

void Load::readExperimentalCondition(xmlpp::Element* root)
{
    const xmlpp::Node::NodeList& lst = root->get_children("CONDITION");
    xmlpp::Node::NodeList::const_iterator it = lst.begin();
    while (it != lst.end()) {
        xmlpp::Element* current = utils::XML::cast_node_element(*it);
        const xmlpp::Node::NodeList& lstData = current->get_children();

        xmlpp::Element* result = NULL;
        for (xmlpp::Node::NodeList::const_iterator jt = lstData.begin();
                jt != lstData.end(); ++jt) {
            if (utils::XML::is_element(*jt)) {
                result = utils::XML::cast_node_element(*jt);
                break;
            }
        }
        AssertS(Exception::Parse, result != NULL);
        string name = utils::XML::get_attribute(current, "MODEL_NAME");
        string port = utils::XML::get_attribute(current, "PORT_NAME");
        string xml = utils::XML::write_to_string(result);
        AssertI(mGModelHead->isGCoupledModel());
        GModel* founded = ((GCoupledModel*)mGModelHead)->findModel(name);

        AssertS(Exception::Parse, founded != NULL);
        AssertS(Exception::Parse, founded->isGAtomicModel());
        AssertS(Exception::Parse, founded->existInitPort(port));

        GModel::toGAtomicModel(founded)->addExperimentalCondition(port, xml);

        ++it;
    }
}

void Load::readMeasures(xmlpp::Element* root)
{
    const xmlpp::Node::NodeList& lst = root->get_children("MEASURE");
    xmlpp::Node::NodeList::const_iterator it = lst.begin();
    while (it != lst.end()) {
        xmlpp::Element* current = utils::XML::cast_node_element(*it);

        string name = utils::XML::get_attribute(current, "NAME");
        mModeling->addMeasure(name);

        string xml = utils::XML::write_to_string(current);
        mModeling->addXMLToMeasure(name, xml);

        const xmlpp::Node::NodeList lstObs =current->get_children("OBSERVABLE");
        xmlpp::Node::NodeList::const_iterator jt = lstObs.begin();
        while (jt != lstObs.end()) {
            xmlpp::Element* obs = utils::XML::cast_node_element(*jt);

            string obsname = utils::XML::get_attribute(obs, "MODEL_NAME");
            string obsport = utils::XML::get_attribute(obs, "PORT_NAME");
            string obsgroup = "";
            string obsindex = "";

            if (utils::XML::exist_attribute(obs,"GROUP"))
                obsgroup = utils::XML::get_attribute(obs, "GROUP");
            if (utils::XML::exist_attribute(obs,"INDEX"))
                obsindex = utils::XML::get_attribute(obs, "INDEX");

            AssertI(mGModelHead->isGCoupledModel());
            GModel* founded = ((GCoupledModel*)mGModelHead)->findModel(obsname);
            AssertS(Exception::Parse, founded != NULL);
            AssertS(Exception::Parse, founded->isGAtomicModel());
            AssertS(Exception::Parse, founded->existStatePort(obsport));

            mModeling->addObservableToMeasure(name, obsname, obsport,
                                              obsgroup, obsindex);
            ++jt;
        }
        ++it;
    }
}

void Load::readOutputs(xmlpp::Element* root)
{
    const xmlpp::Node::NodeList& lst = root->get_children("OUTPUT");
    xmlpp::Node::NodeList::const_iterator it = lst.begin();
    while (it != lst.end()) {
        xmlpp::Element* current = utils::XML::cast_node_element(*it);
        string name = utils::XML::get_attribute(current, "NAME");
        mModeling->addMeasuresOutputs(name, utils::XML::write_to_string(current));
        ++it;
    }
}

bool Load::existModel(const string& name) const
{
    MapStringModel::const_iterator it = mMsm.find(name);
    return it != mMsm.end();
}

graph::Model* Load::getModel(const string& name)
{
    MapStringModel::const_iterator it = mMsm.find(name);
    return (it != mMsm.end()) ? (*it).second : NULL;
}

GModel* Load::getModel(graph::Model* m)
{
    MapModelGModel::const_iterator it = mMMM.find(m);
    return (it != mMMM.end()) ? (*it).second : NULL;
}

Save::Save(const string& gzFilename, GModel* top, Modeling* m, bool compress) :
        IO(m)
{
    AssertI(top);
    mGModelHead = top;
    mModelHead = top->getModel();

    utils::Save xml(gzFilename);
    xml.setProjectStructures(Save::saveStructures(mModelHead));
    xml.setProjectDynamics(Save::saveDynamics(mGModelHead, mModeling,
                           mTranslatorList));
    xml.setProjectGraphics(Save::saveGraphics(mGModelHead));
    xml.setProjectExperiments(Save::saveExperiments(mGModelHead, mModeling));

    for (TranslatorList::iterator it = mTranslatorList.begin();
            it != mTranslatorList.end(); ++it) {
        xml.addProjectNoVLE((*it).modelname, (*it).translatorname, (*it).xml);
    }

    xml.save(compress);
}

Glib::ustring Save::saveStructures(graph::Model* model)
{
    xmlpp::Document doc;
    xmlpp::Element* elt = doc.create_root_node("MODEL");
    model->writeXML(elt);
    return doc.write_to_string_formatted();
}

Glib::ustring Save::saveGraphics(GModel* model)
{
    xmlpp::Document doc;
    xmlpp::Element* first = doc.create_root_node("MODELS");
    saveGraphics(first, model);
    return doc.write_to_string_formatted();
}

void Save::saveGraphics(xmlpp::Element* root, GModel* m)
{
    AssertI(root and m);
    xmlpp::Element* model = root->add_child("MODEL");
    xmlpp::Element* position = model->add_child("POSITION");

    model->set_attribute("NAME", m->getName());
    position->set_attribute("X", utils::to_string(m->getX()));
    position->set_attribute("Y", utils::to_string(m->getY()));

    if (m->isGAtomicModel()) {
        model->set_attribute("TYPE", "atomic");
    } else {
        model->set_attribute("TYPE", "coupled");

        xmlpp::Element* size = model->add_child("SIZE");
        size->set_attribute("WIDTH",
                            utils::to_string(((GCoupledModel*)m)->getWidthView()));
        size->set_attribute("HEIGHT",
                            utils::to_string(((GCoupledModel*)m)->getHeightView()));
        GModel::MapModel lst = ((GCoupledModel*)m)->getModelList();
        GModel::MapModel::iterator it = lst.begin();
        while (it != lst.end()) {
            saveGraphics(root, (*it).second);
            it++;
        }
    }
}

Glib::ustring Save::saveDynamics(GModel* model, Modeling* modeling,
                                 TranslatorList& translatorList)
{
    xmlpp::Document doc;
    xmlpp::Element* first = doc.create_root_node("MODELS");
    saveDynamics(first, model, modeling, translatorList);
    return doc.write_to_string_formatted();
}

void Save::saveDynamics(xmlpp::Element* root, GModel* m, Modeling* modeling,
                        TranslatorList& translatorList)
{
    AssertI(root and m);

    if (m->isGAtomicModel()) {
        GAtomicModel* ma = GModel::toGAtomicModel(m);
        string dynamics_xml = ma->getDynamic();
        if (dynamics_xml.empty() == false) {
            utils::XML::add_child_string(root, dynamics_xml);
        }
    } else if (m->isGCoupledModel()) {
        GCoupledModel* mc = GModel::toGCoupledModel(m);
        const GModel::MapModel& mm = mc->getModelList();
        for (GModel::MapModel::const_iterator it = mm.begin(); it != mm.end();
                ++it) {
            saveDynamics(root, (*it).second, modeling, translatorList);
        }
    }
}

Glib::ustring Save::saveExperiments(GModel* mdl, Modeling* modeling)
{
    xmlpp::Document doc;
    xmlpp::Element* root = doc.create_root_node("EXPERIMENT");

    root->set_attribute("NAME", modeling->experimentName());
    root->set_attribute("DURATION", utils::to_string(modeling->duration()));
    root->set_attribute("DATE", utils::get_current_date());

    xmlpp::Element* experimental_conditions =
        root->add_child("EXPERIMENTAL_CONDITIONS");
    saveExperimentalConditions(experimental_conditions, mdl);

    xmlpp::Element* measures = root->add_child("MEASURES");
    xmlpp::Element* outputs = measures->add_child("OUTPUTS");
    saveOutputs(outputs, modeling);
    saveMeasures(measures, modeling);

    return doc.write_to_string_formatted();
}

void Save::saveExperimentalConditions(xmlpp::Element* top, GModel* m)
{
    AssertI(top and m);

    if (m->isGAtomicModel()) {
        GAtomicModel* atom = GModel::toGAtomicModel(m);
        const GAtomicModel::MapStringString lst =
            atom->getExperimentalCondition();
        GAtomicModel::MapStringString::const_iterator it = lst.begin();
        while (it != lst.end()) {
            if (it->second != "") {
                xmlpp::Element* newexp = top->add_child("CONDITION");
                newexp->set_attribute("MODEL_NAME", atom->getName());
                newexp->set_attribute("PORT_NAME", (*it).first);
                utils::XML::add_child_string(newexp, (*it).second);
            }
            ++it;
        }
    } else {
        GCoupledModel* mc = GModel::toGCoupledModel(m);
        const GModel::MapModel& mm = mc->getModelList();
        GModel::MapModel::const_iterator it = mm.begin();
        while (it != mm.end()) {
            saveExperimentalConditions(top, (*it).second);
            ++it;
        }
    }
}

void Save::saveOutputs(xmlpp::Element* top, Modeling* modeling)
{
    AssertI(top);

    const Measures::Outputs_t& out = modeling->getOutputs();
    Measures::Outputs_t::const_iterator it = out.begin();

    while (it != out.end()) {
        xmlpp::Element* output = top->add_child("OUTPUT");
        output->set_attribute("NAME", (*it).first);

        try {
            xmlpp::DomParser dom;
            dom.parse_memory((*it).second);

            xmlpp::Element* root = utils::XML::get_root_node(dom, "OUTPUT");
            output->set_attribute("TYPE",
                                  utils::XML::get_attribute(root, "TYPE"));
            output->set_attribute("FORMAT",
                                  utils::XML::get_attribute(root, "FORMAT"));
            if (utils::XML::exist_attribute(root, "PLUGIN"))
                output->set_attribute("PLUGIN",
                                      utils::XML::get_attribute(root,
                                                                "PLUGIN"));
            if (utils::XML::exist_attribute(root, "LOCATION"))
                output->set_attribute("LOCATION",
                                      utils::XML::get_attribute(root,
                                                                "LOCATION"));
            utils::XML::import_children_nodes(output,root);
        } catch (const std::exception& e) {
            ThrowInternal();
        }
        ++it;
    }
}

void Save::saveMeasures(xmlpp::Element* top, Modeling* modeling)
{
    AssertI(top);

    const Measures::Measures_t& measures = modeling->getMeasures();
    Measures::Measures_t::const_iterator it = measures.begin();
    while (it != measures.end()) {
        xmlpp::Element* xml_measure = top->add_child("MEASURE");
        xml_measure->set_attribute("NAME", (*it).first);

        string xml((*it).second.first);
        const std::list < Measures::Observable_t >& lst((*it).second.second);
        std::list < Measures::Observable_t >::const_iterator jt = lst.begin();

        try {
            xmlpp::DomParser dom;
            dom.parse_memory(xml);
            xmlpp::Element* root = utils::XML::get_root_node(dom, "MEASURE");

            const string type = utils::XML::get_attribute(root, "TYPE");
            xml_measure->set_attribute("TYPE", type);
            if (type == "timed") {
                xml_measure->set_attribute(
                    "TIME_STEP", utils::XML::get_attribute(root, "TIME_STEP"));
            }
            xml_measure->set_attribute(
                "OUTPUT", utils::XML::get_attribute(root, "OUTPUT"));

            while (jt != lst.end()) {
                xmlpp::Element* xml_observable;
                xml_observable = xml_measure->add_child("OBSERVABLE");
                xml_observable->set_attribute("MODEL_NAME", jt->model);
                xml_observable->set_attribute("PORT_NAME", jt->port);
                if (jt->group != "")
                    xml_observable->set_attribute("GROUP", jt->group);
                if (jt->index != "")
                    xml_observable->set_attribute("INDEX", jt->index);
                ++jt;
            }
        } catch (const std::exception& e) {
            ThrowInternal();
        }
        ++it;
    }
}

}
} // namespace vle gvle
