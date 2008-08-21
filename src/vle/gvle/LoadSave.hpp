/**
 * @file vle/gvle/LoadSave.hpp
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


#ifndef GUI_LOADSAVE_HPP
#define GUI_LOADSAVE_HPP

#include <vle/graph/Model.hpp>
#include <list>
#include <map>
#include <string>

namespace vle
{
namespace gvle {

class Modeling;

/**
 * A base class for I/O operation
 *
 */
class IO
{
public:
    typedef std::list < std::string > StringList;
    typedef std::map < std::string, graph::Model* > MapStringModel;

    IO(Modeling* mod = NULL) :
            mModelHead(NULL),
            mGModelHead(NULL),
            mModeling(mod) {}

    virtual ~IO() {}

    /** return top node of devs structure
     * @return graph::Model top node
     */
    inline graph::Model* getStructure() {
        return mModelHead;
    }

    /** return top node of GModel structure
     * @return GModel top node
     */
    inline GModel* getGraphics() {
        return mGModelHead;
    }

protected:
    graph::Model* mModelHead;
    GModel*      mGModelHead;
    Modeling*    mModeling;
};


/** a class to load all XML application file from a GZipped file and
 * return instance for graph::Model (structures) and GModel
 * (graphics). Objects instances are not de-allocated after in
 * destructor
 */
class Load : public IO
{
public:
    /** gunzip GZipped project file and parse all file
     * @param filename
     * @throw Exception::Parse, if problem when parsing
     * @throw Exception::File, if file is not a vle gzip file
     * @throw std::invalid_argument, if filename problem
     */
    explicit Load(const std::string& filename, Modeling* mod);

    /** nothing. graph::Model and GModel are not delete in this
     * destructor. Manage yourself.
     */
    virtual ~Load() {}

    /** project has a graphics xml file ?
     * @return true if project has a graphics xml file, otherwise
     * false
     */
    inline bool hasGraphics() {
        return mHasGraphics;
    }

    /** project has a experiments xml file ?
     * @return true if project has a experiments xml file, otherwise
     * false
     */
    inline bool hasExperiments() {
        return mHasExperiments;
    }

    /** add name to top model in XML structures, graphics and dynamics.
     * @param name string to add in NAME attribue
     * @param structures XML structures
     * @param graphics XML graphics
     * @param dynamics XML dynamics
     * @throw Exception::Parse
     */
    static void addModelName(const std::string& name,
                             std::string& structure,
                             std::string& dynamics,
                             std::string& graphics);

    /** extract XML structure from a string and create GModel tree and
     * attach it under node parent.
     * @param parent parent node to attach new XML structure
     * @param structures XML structure to create
     * @param dynamics XML dynamics to attach for each GModel
     * @param graphics XML graphics to define graphics
     * @throw Exception::Parse if error reading
     * @return a new GModel
     */
    static GModel* extractTreeStructures(GCoupledModel* parent, const
                                         std::string& structures, const
                                         std::string& dynamics, const
                                         std::string& graphics =
                                             std::string());

    /** clone a graph::Model hierarchy to a GModel hierarchy
     * @param top graph::Model hierarchy to clone
     * @param mmm MapModelGModel extract from graph::Model tree
     * @return a ptr to GModel
     * @throw Exception::Parse when connot found a model to clone
     */
    static GModel* cloneModelToGModel(graph::Model* top,
                                      MapModelGModel&
                                      mmm, GCoupledModel* parent);

    /** attach graphics information from string to GModel in MapModelGModel
     * @param mmm MapModelGModel to insert graphics informations
     * @param graphics string XML representation
     * @throw Exception::Parse when error parsing
     */
    static void attachGraphicsToModel(MapModelGModel& mmm, const
                                      std::string& graphics);

    /** attach dynamics tree information from string to GModels in
     * MapModelGModel
     * @param mmm MapModelGModel to insert dynamcis informations
     * @param dynamics string XML representation
     * @throw Exception::Parse when error parsing
     */
    static void attachDynamicsToModel(MapModelGModel& mmm, const
                                      std::string& dynamics);

    /** find a GModel with string name from MapModelGModel
     * @param name name to found in map model
     * @param mmm MapModelGModel to get GModel
     * @return GModel founded or NULL
     */
    static GModel* Load::findModelFromMapModelGModel(const std::string&
            name, MapModelGModel& mmm);

    /** Parse xml structure and attach new port on this model.
     * @param model Model to attach new port or delete old.
     * @param xml vle structures to parse.
     * @throw Exception::Parse when error parsing.
     * @throw Exception::Internal when deleting existing port with a
     * connection.
     */
    static void Load::attachPortToStructure(graph::Model* model,
                                            const Glib::ustring& xml);

private:
    /** fill MapModelModel string and model* with m and its children
     * @param m Model to get information
     */
    void fillMapStringModel(graph::Model* m);

    /** get document from project and parse to create graph::Model
     * hierarchy
     * @throw Exception::Parse when top model is an atomic model
     */
    void loadStructure(xmlpp::Element* root);

    /** get xml data type from xmlpp::Document, parse an create a GModel
     * hierarchy
     */
    void loadGraphics(xmlpp::Element* root);

public:
    /** get from a xmlpp::Element node, information data for a GModel
     * @param node xmlpp::Element node to get information
     * @param model GModel to set information
     */
    static void loadGraphicsData(xmlpp::Element* node, GModel* model);

    /** get from a xmlpp::Element node, information data for a GModel
     * @param node xmlpp::Element node to get information
     * @param model GModel to set information
     */
    static void loadDynamicsData(xmlpp::Element* elt, GModel* mdl);

private:
    /**
     * Attach to each GNoVLEModel a string representation of translatorname
     * and the NoVLE XML.
     *
     * @param lst the list of XMLNoVLEList from projectXML.
     */
    void buildNoVLES(const utils::XMLNoVLEList& lst);

    /** load dynamics file from ProjectXML and affect GAtomicModel with
     * formalism and dynamics
     * @param pj ProjectXML to get xmlpp::Document
     * @throw Exception::Parse when problem in parsing
     */
    void loadDynamics(xmlpp::Element* root);

    /** Load experiment information from ProjectXML and affect all
     * GAtomicModel
     * @param pj ProjectXML to get xmlpp::Document
     * @throw Exception::Parse when problem in parsing
     */
    void loadExperiments(xmlpp::Element* root);

    /** read experimental condition an write in into GAtomicModel
     * @param root node to read
     * @throw Exception::Parse error when parsing
     */
    void readExperimentalCondition(xmlpp::Element* root);

    /** read measures informations from root element and write it into
     * Measure class
     * @param root node to read
     * @throw Exception::Parse error when parsing
     */
    void readMeasures(xmlpp::Element* root);

    /** read measures outputs information from root element and write it
     * into Measure class
     * @param root node to read
     * @throw Exception::Parse error when parsing
     */
    void readOutputs(xmlpp::Element* root);

    /** test existence of a model with name
     * @param name model name to verify
     * @return true if exist, false otherwise
     */
    bool existModel(const std::string& name) const;

    /** get ptr to model with name if exist in list
     * @param name model name to verify
     * @return ptr with name if exist, NULL otherwise
     */
    graph::Model* getModel(const std::string& name);

    /** get ptr to GModel with ptr model if exist in list
     * @param m graph::Model ptr to find GModel
     * @return ptr GModel corresponding, NULL otherwise
     */
    GModel* getModel(graph::Model* m);

private:
    MapStringModel mMsm;
    MapModelGModel mMMM;
    bool           mHasGraphics;
    bool           mHasExperiments;
};



/** A class to save all XML application to a GZipped file
 */
class Save : public IO
{
public:
    /** Make XML application from graph::Model structures and graphics
     * and GZip project file.
     * @param gzFilename project file name to save.
     * @param top tree to save.
    * @param compress true to compress file.
     * @throw Exception::File when error accessing file.
     * @throw Exception::Parse when error generating XML.
     */
    explicit Save(const std::string& gzFilename, GModel* top,
                  Modeling* m, bool compress);

    /** nothing. graph::Model and GModel are not delete in this
     * destructor. Manage yourself.
     */
    virtual ~Save() {}

private:
    /** Define a translator item in TranslatorList. */
    typedef struct TranslatorItem {
        std::string modelname;
        std::string translatorname;
        std::string xml;
    };

    /** Define a list of TranslatorItem. */
    typedef std::list < TranslatorItem > TranslatorList;

public:

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Structures save part.
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
    * Write XML structures from devs models into XML buffer.
     *
    * @param model root node of devs models.
     * @param top root node of GModel to find translator.
     * @param modeling to get translator information.
     *
    * @return XML structure.
     */
    static Glib::ustring saveStructures(graph::Model* model);


    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Graphics save part.
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


    /** Write XML graphics from devs models into XML buffer.
     * @param model root node of devs models.
     * @return XML graphics.
     */
    static Glib::ustring saveGraphics(GModel* model);

    /** recursively save XML graphics data on each GModel.
     * @param root a node to insert element.
     * @param m a ptr to GModel to save.
     */
    static void saveGraphics(xmlpp::Element* root, GModel* m);

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Dynamics save part.
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /** Write XML dynamics from devs models into XML buffer.
     * @param model root node of devs models.
     * @param out parameter to add translator information.
     * @return XML dynamics.
     */
    static Glib::ustring saveDynamics(GModel* model,
                                      Modeling* modeling,
                                      TranslatorList& translatorlist);

    /** write XML dynamics file into xml file filename.
     * @param root a node to insert element.
     * @param out parameter to add translator information.
     * @param filename of xml file to write.
     */
    static void saveDynamics(xmlpp::Element* root,
                             GModel* m,
                             Modeling* modeling,
                             TranslatorList& translatorlist);

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Experiments save part.
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /** Write XML experiments from devs models into XML buffer.
     * @param mdl root node of devs models.
     * @param modeling to get experimental condition.
     * @return XML experiments.
     */
    static Glib::ustring saveExperiments(GModel* mdl,
                                         Modeling* modeling);

    /** recursive function. Save ExperimentalCondition under xmlpp::Node
     * top for Model m. If m is a GCoupledModel call recursively
     * function to find all GAtomicModel.
     * @param top xmlpp::Node to write under.
     * @param m Model to get ExperimentalCondition informations.
     */
    static void saveExperimentalConditions(xmlpp::Element* top,
                                           GModel* m);

    /** save Output information from Modeling class under xmlpp::Node top.
     * @param top xmlpp::Node to write under.
     * @param Modeling to get Outputs.
     * @throw Exception::Internal if parsing error output.
     */
    static void saveOutputs(xmlpp::Element* top, Modeling* modeling);

    /** save Measures information from Modeling class under xmlpp::Node
     * top.
     * @param top xmlpp::Node to write under.
     * @param modeling to get measures.
     * @throw Exception::Internal if parsing error Measure.
     */
    static void Save::saveMeasures(xmlpp::Element* top, Modeling* modeling);

private :
    TranslatorList      mTranslatorList;
};

}
} // namespace vle gvle

#endif
