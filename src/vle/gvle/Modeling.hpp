/*
 * @file vle/gvle/Modeling.hpp
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


#ifndef GUI_MODELING_HPP
#define GUI_MODELING_HPP

#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/ModelTreeBox.hpp>
#include <vle/gvle/ModelClassBox.hpp>
#include <vle/gvle/CutCopyPaste.hpp>
#include <vle/gvle/Editor.hpp>
#include <vle/gvle/ViewDrawingArea.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/graph/Model.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Rand.hpp>
#include <libglademm/xml.h>
#include <map>
#include <string>
#include <vector>
#include <set>

namespace vle { namespace gvle {

class AtomicModelBox;
class ImportModelBox;
class ImportClassesBox;
class CoupledModelBox;
class View;

/**
 * @brief This class contains all information on the opened VLE project. It
 * store the graph::Model hierarchy, the gui::GModel hierarchy and all
 * information on the project.
 */
class Modeling
{
public:
    /** define associative map from graph::model name and is reference. */
    typedef std::map < std::string, graph::Model * > MapStringModel;

    /** define vector of current view. */
    typedef std::vector < View * > ListView;

    /** define set of string represent current models names. */
    typedef std::set < std::string > SetString;

    /**
     * create a new Modeling with a ptr.
     *
     * @param gvle to get information on graphics interface.
     * @param filename project file to load.
     */
    Modeling(GVLE* gvle, const std::string& filename = std::string());

    /**
     * delete all models and view.
     */
    ~Modeling();

    /**
     * delete all models, and view.
     */
    void clearModeling();

    void setGlade(Glib::RefPtr < Gnome::Glade::Xml > xml);

    Glib::RefPtr < Gnome::Glade::Xml > getGlade() const;


    /********************************************************************
     *
     * START, XML LOADING AND SAVE
     *
     ********************************************************************/


    /**
     * @brief delete all models and views and restart with one view.
     *
     */
    void start();

    /**
     * @brief delete all models and views and restart with one view.
     *
     * @param path path where the vpz file is wanted to be stored
     * @param fileName vpz file name
     *
     */
    void start(const std::string& path, const std::string& fileName);

    /**
     * parse XML project file to get filename of XML files structures and
     * graphics.
     *
     * @param name project file name to parse.
     */
    void parseXML(const std::string& name);

    /**
     * write XML project file and XML files structures and graphics.
     *
     * @param name project file name to write.
     * @param top a ptr to the top GModel of tree.
     * @param compress you want to gzip ?
     */
    void saveXML(const std::string& name);


    /********************************************************************
     *
     * SET / GET TOP GMODEL
     *
     ********************************************************************/


    /**
     * set the top model ; If model already exist, delete it.
     *
     * @param cp
     */
    void setTopModel(graph::CoupledModel* cp);

    /**
     * return the top model.
     *
     * @return top model of tree.
     */
    inline graph::CoupledModel* getTopModel() {
        return mTop;
    }

    /**
     * return list of class models available from Modeling.
     *
     * @return list of small-models.
     */
    inline const vpz::Classes& getClassModel() const {
        return mVpz.project().classes();
    }

    /**
     * get a class model graph::CoupledModel with name.
     *
     * @param std::string name get graph::CoupledModel
     * @return graph::CoupledModel reference to class model, NULL if
     * class model doesn't exist.
     */
    inline graph::Model* getClassModel(const std::string& name)
	{ return mVpz.project().classes().get(name).model(); }

    /**
     * get the AtomicModelList associate to the class
     *
     * @param name of class model to delete
     */
    inline vpz::AtomicModelList& getAtomicModelClass(std::string className);

    /**
     * add a new list of atomic model for a class
     *
     * @param name of the class
     * @param new_atom : the new list
     */
    inline void addAtomicModelClass(std::string className,
				    graph::AtomicModel* new_atom)
	{ vpz().project().classes().get(className).atomicModels().add(
		new_atom, vpz::AtomicModel("", "", "")); }

    void setSelectedClass(const std::string& name)
	{ mSelectedClass = name; }

    /********************************************************************
     *
     * TWO FUNCTIONS TO CREATE A MAP OF STRING MODEL FROM A MODEL GRAPH
     *
     ********************************************************************/


    /**
     * parse graph structure with top is m and return a map string model of
     * all Model in graph ; this function call a recursive function.
     *
     * @param m model represent top of graph.
     * @return a map string model of all model in graph.
     */
    MapStringModel buidMapString(graph::Model* m);

    /**
     * This recursive function is call by parseStructures ; It parse graph
     * structure with m is current node and add each node in msm.
     *
     * @param m model node.
     * @param msm string model map.
     */
    void parseStructures(graph::Model* m, MapStringModel& msm);


    /********************************************************************
     *
     * VIEW
     *
     ********************************************************************/


    /**
     * show dynamics for a plugin
     *
     * @param name the name of GModel dynamics to show.
     */
    void showDynamics(const std::string& name);

    /**
     * add a view to modeling for the GModel model.
     *
     * @param model a ptr to the GModel to show.
     */
    void addView(graph::Model* model);

    /**
     * find a view in list view that reference a GCoupledModel.
     * @param model reference to GCoupledModel to find.
     * @return int index in list view for founded view or first index with
     * null value
     */
    void addView(graph::CoupledModel* model);

    /**
     * add a view to the current class
     *
     * @param model to ptr to the GModel assiociate to the class
     */
    void addViewClass(graph::Model* model, std::string name);

    /**
     * find a view in list view that reference a GCoupledModel.
     * @param model reference to GCoupledModel to find.
     */

    void addViewClass(graph::CoupledModel* model, std::string name);

    /**
     * Return True if a View of the parameter already exist.
     * @param model reference to GCoupledModel to find.
     * @return Return True if the View exist , False otherwise
     */
    bool existView(graph::CoupledModel* model);

    /**
     * Return the View that reference the model.
     * @param model reference to GCoupledModel to find.
     * @return Return the View if exist , NULL otherwise
     */
    View* findView(graph::CoupledModel* model);

    /**
     * delete a view from model view.
     *
     * @param index num of window to delete.
     */
    void delViewIndex(size_t index);

    /**
     * delete all view where coupled model is equal to cm.
     *
     * @param cm a ptr to graph::CoupledModel to delete view.
     */
    void delViewOnModel(const graph::CoupledModel* cm);

    /**
     * delete all view of modeling.
     *
     */
    void delViews();


    void refreshViews();

    void EditCoupledModel(graph::CoupledModel* model);

    /**
     * @brief Assign a new titles to all gvle::View.
     */
    void setTitles();

    /**
     * @brief Assign a new titles to all gvle::View and an asterisk to inform
     * that this model is modified.
     */
    void setModifiedTitles();


    /********************************************************************
     *
     * MANAGE GVLE INTERFACE
     *
     ********************************************************************/


    /**
     * get current button selected in GVLE panel.
     *
     * @return selected button.
     */
    inline GVLE::ButtonType getCurrentButton() const
	{ return mGVLE->getCurrentButton(); }

    /**
     * return a ptr to application GVLE.
     *
     * @return ptr to application GVLE.
     */
    inline const GVLE* getGVLE() const
	{ return mGVLE; }

    inline GVLE* getGVLE()
	{ setModified(true); return mGVLE; }

    inline void hideGVLE()
	{ mGVLE->hide(); }

    inline const Editor::Documents& getDocuments() const
	{ return mGVLE->getEditor()->getDocuments(); }

    inline int runConditionsBox(const vpz::Conditions& conditions)
	{ return mGVLE->runConditionsBox(conditions); }

    inline void applyConditionsBox(vpz::Conditions& conditions)
	{ mGVLE->applyConditionsBox(conditions); }

    /********************************************************************
     *
     * OTHER USEFULL FUNCTION
     *
     ********************************************************************/


    /**
     * return true if a model with name 'name' already exist in tree.
     *
     * @param name string to search.
     * @return true if a model with name 'name' is found.
     */
    bool exist(const std::string& name) const;

    /**
     * return true if a model with ptr = m exist in tree.
     *
     * @param m model to find.
     * @return true if a model with ptr equal to m is found.
     */
    bool exist(const graph::Model* m) const;


    /********************************************************************
     *
     * CUT COPY AND PASTE
     *
     ********************************************************************/


    /**
     * clone the ListGModel if has no connection with external model.
     *
     * @param lst list of selected GModel.
     * @param gc parent of selected GModel.
     */
    void cut(graph::ModelList& lst, graph::CoupledModel* gc, std::string className);

    /**
     * detach the list of GModel of GCoupledModel parent.
     *
     * @param lst list of selected GModel.
     * @param gc parent of selected GModel.
     */
    void copy(graph::ModelList& lst, graph::CoupledModel* gc, std::string className);

    /**
     * paste the current list GModel into GCoupledModel ; rename
     * models to elimiante duplicated name.
     *
     * @param gc paste selected GModel under this GCoupledModel.
     */
    void paste(graph::CoupledModel* gc, std::string className);


    /********************************************************************
     *
     * COMPRESSED, MODIFIED AND SAVED
     *
     ********************************************************************/


    /**
     * Return true if model is modified since last new or loading.
     *
     * @return true if model is modified, otherwise false.
     */
    inline bool isModified() const {
        return mIsModified;
    }

    /**
     * Return true if model is saved.
     *
     * @return true if model is saved, otherwise.
     */
    inline bool isSaved() const {
        return mIsSaved;
    }

    /**
     * Set document modification.
     *
     * @param modified true if document is modified, otherwise false.
     */
    void setModified(bool modified);

    /**
     * Set document save.
     *
     * @param saved true if document is saved, otherwise false.
     */
    inline void setSaved(bool saved) {
        mIsSaved = saved;
    }

    /**
     * Get file name of current document.
     *
     * @return file name of current document.
     */
    inline const std::string& getFileName() const {
        return mFileName;
    }

    /**
     * Set current file name for document.
     *
     * @param name filename of current document.
     */
    inline void setFileName(const std::string& name) {
        mFileName.assign(name);
    }

    /**
     * Get compressed vpz format.
     *
     * @return true if vpz is compressed.
     */
    inline bool isCompressed() const {
        return mIsCompressed;
    }

    /**
     * Set compressed vpz format, true for gzip, false to text.
     *
     * @param compress true to compress vpz file, false otherwise.
     */
    inline void setCompressed(bool compress) {
        mIsCompressed = compress;
    }

    void exportCoupledModel(graph::CoupledModel* model, vpz::Vpz* dst, std::string className);

    void importModel(graph::CoupledModel* parent, vpz::Vpz* src);

    void importModelToClass(vpz::Vpz* src, std::string& className);

    void importClasses(vpz::Vpz* src);

    void exportClass(graph::Model* model, vpz::Class classe, vpz::Vpz* dst);

    /********************************************************************
     *
     * MANAGE MODELS NAME
     *
     ********************************************************************/


    /**
     * return true if model name exist in models names list.
     *
     * @param name name to test existing.
     * @return true if model exist in list, otherwise false.
     */
    bool existModelName(const std::string& name) const;

    /**
     * test if name is a correct name i.e. only ascii char, only char:
     * [[A-Z][a-z]]* and not exist in model name.
     *
     * @param name std::string to test validity.
     * @return true if name is correct, false otherwise.
     */
    bool isCorrectName(const Glib::ustring& name) const;

    /**
     * get a new string using Gtk::Dialog class.
     *
     * @param name output parameter fill by correct new name.
     * @return true if name is ok, false otherwise.
     */
    bool getNewName(std::string& name) const;

    /**
     * delete a string from list.
     *
     * @param name model name to delete.
     */
    void delName(const std::string& name);

    /**
     * delete all string from list.
     */
    void delNames();

    /**
     * add a model name into list.
     *
     * @param name a new name to add.
     */
    void addName(const std::string& name);

    /**
     * Get a model name with prefix name ; Name must include only ascii
     * char.
     * @code
     * if name equal :
     *    "toto" then, return "toto_"
     *    "toto" and "toto_" exist, return "toto[A-z]_"
     * if name is empty,
     *    return "[A-z]*8_" minimum, add [A-z] if model exist
     * @endcode
     *
     * @param name a prefix for model name.
     * @return a prefix for a model like "toto_".
     */
    std::string getNameWithPrefix(const std::string& name);

    /**
     * return list of model name.
     *
     * @return list of model name.
     */
    const SetString& getNames() const;



    /********************************************************************
     *
     * MANAGE MEASURE - INTERFACE FOR MEASURES CLASS
     *
     ********************************************************************/

    const vpz::Experiment& experiment() const
    { return mVpz.project().experiment(); }

    vpz::Experiment& experiment()
    { setModified(true); return mVpz.project().experiment(); }

    const vpz::Views& views() const
    { return mVpz.project().experiment().views(); }

    vpz::Views& views()
    { setModified(true); return mVpz.project().experiment().views(); }

    const vpz::Outputs& outputs() const
    { return mVpz.project().experiment().views().outputs(); }

    vpz::Outputs& outputs()
    { setModified(true); return mVpz.project().experiment().views().outputs(); }

    /********************************************************************
     *
     * DYNAMICS - CONDITIONS - OBSERVABLES
     *
     ********************************************************************/

    const vpz::Dynamics& dynamics() const
    { return mVpz.project().dynamics();  }

    vpz::Dynamics& dynamics()
    {
	setModified(true);
	return mVpz.project().dynamics();
    }

    const vpz::Observables& observables() const
    { return mVpz.project().experiment().views().observables(); }

    vpz::Observables& observables()
    {
        setModified(true);
        return mVpz.project().experiment().views().observables();
    }

    const vpz::Conditions& conditions() const
    { return mVpz.project().experiment().conditions(); }

    vpz::Conditions& conditions()
    {
        setModified(true);
        return mVpz.project().experiment().conditions();
    }

    vpz::AtomicModel& get_model(const graph::AtomicModel* atom,
				const std::string& className = "")
    {
	if (className.empty()) {
	    return mVpz.project().model().atomicModels().get(atom);
	} else {
	    return mVpz.project().classes().get(className)
		.atomicModels().get(atom);
	}
    }

    const std::vector < std::string >* get_conditions(graph::AtomicModel* atom);

    /********************************************************************
     *
     * SOCKET INFORMATION
     *
     ********************************************************************/


    /**
     * Return currently use port to connect VLE and GVLE.
     *
     * @return a port number between [0-65536], default is 8000.
     */
    inline int getSocketPort() const {
        return mSocketPort;
    }

    /**
     * Set a new port to connect VLE and GVLE.
     *
     * @param socket change used port by a new if port number [0-65535].
     */
    inline void setSocketPort(int socket) {
        if (socket > 0 and socket < 65535) mSocketPort = socket;
    }


    /********************************************************************
     *
     * CREATE GATOMICMODEL AND GCOUPLEDMODEL
     *
     ********************************************************************/

    graph::CoupledModel* newCoupledModel(
        graph::CoupledModel* parent,
        const std::string& name,
        const std::string& description,
        int x, int y);

    graph::AtomicModel* newAtomicModel(
        graph::CoupledModel* parent,
        const std::string& name,
        const std::string& description,
        int x, int y);

    graph::Model* newPluginModel(
        graph::CoupledModel* parent,
        const std::string& name,
        const std::string& description,
        int x, int y);

    void renameModel(graph::Model* model,
                     const std::string& name,
                     const std::string& description);

    void delModel(graph::Model* model, std::string className);

    inline const vpz::Vpz& vpz() const {
        return mVpz;
    }

    inline vpz::Vpz& vpz()
    { setModified(true); return mVpz; }

    void vpz_is_correct(std::vector<std::string>& vec);

    /********************************************************************
     *
     * MANAGE THE DRAWING SETTINGS
     *
     ********************************************************************/

    inline void updateAdjustment(double h, double v)
    { return mGVLE->updateAdjustment(h, v); }

    inline utils::Rand* getRand() const
    { return mRand; }

    /********************************************************************
     *
     * GET PIECES OF INFORMATION
     *
     ********************************************************************/

    std::string getDynamicInfo(std::string dynamicName) const;

    std::string getIdCard(std::string className) const;

    std::string getIdCard(graph::Model* model) const;

    std::string getIdCard(graph::CoupledModel* model) const;

    std::string getClassIdCard(graph::Model* model,
                               std::string className) const;

    std::string getClassIdCard(graph::CoupledModel* model) const;

    std::string getIdCardConnection(graph::Model* src,
                                    graph::Model* dest,
                                    graph::CoupledModel* mTop) const;

    std::string getIdCardConnection(graph::Model* src, std::string srcport,
                                    graph::Model* dest,
                                    std::string destport,
                                    graph::CoupledModel* mTop) const;

private:
    vpz::Vpz                    mVpz;
    graph::CoupledModel*        mTop;
    std::string                 mCurrentClass;
    std::string                 mSelectedClass;
    GVLE*                       mGVLE;
    utils::Rand*                mRand;
    ListView                    mListView;
    CutCopyPaste                mCutCopyPaste;
    bool                        mIsModified;
    bool                        mIsSaved;
    bool                        mIsCompressed;
    std::string                 mFileName;
    SetString                   mModelsNames;
    int                         mSocketPort;
    AtomicModelBox*             mAtomicBox;
    ImportModelBox*             mImportModelBox;
    ImportClassesBox*           mImportClassesBox;
    CoupledModelBox*            mCoupledBox;

    Glib::RefPtr < Gnome::Glade::Xml >  mRefXML;

    void export_atomic_model(vpz::Vpz* dst, graph::AtomicModel* atom, std::string className = "");
    void export_coupled_model(vpz::Vpz* dst, graph::CoupledModel* atom, std::string className = "");

    void import_atomic_model(vpz::Vpz* src, graph::AtomicModel* atom, std::string className = "");
    void import_coupled_model(vpz::Vpz* src, graph::CoupledModel* atom, std::string className = "");
};

void parse_model(vpz::AtomicModelList& list);

} } // namespace vle gvle

#endif
