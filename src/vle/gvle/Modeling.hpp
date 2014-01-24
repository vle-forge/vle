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


#ifndef GUI_MODELING_HPP
#define GUI_MODELING_HPP

#include <vle/vpz/Vpz.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/utils/Rand.hpp>
#include <map>
#include <string>
#include <vector>
#include <set>

#include <sigc++/sigc++.h>

namespace vle { namespace gvle {

class View;

/**
 * @brief This class contains all information on the opened VLE project. It
 * store the vpz::Model hierarchy, the gui::GModel hierarchy and all
 * information on the project.
 */
class Modeling
{
private:
    typedef std::vector < std::pair < std::string,
                                      std::string > > renameList;
public:
    /** define associative map from Model name and is reference. */
    typedef std::map < std::string, vpz::BaseModel* > MapStringModel;

    /** define set of string represent current models names. */
    typedef std::set < std::string > SetString;

    /** signal that something has been modified*/
    typedef sigc::signal<void> SignalModified;

    /**
     * create a new Modeling with a ptr.
     *
     * @param gvle to get information on graphics interface.
     * @param filename project file to load.
     */
    Modeling(const std::string& filename = std::string());

    /**
     * delete all models and view.
     */
    ~Modeling();

    /**
     * signal accessor.
     */
    inline SignalModified signalModified()
    { return mSignalModified; }

    /**
     * to signal that a modification to GVLE.
     */
    inline void sendSignalModified()
    { mSignalModified.emit(); }


    /**
     * delete all models, and view.
     */
    void clearModeling();

    /********************************************************************
     *
     * XML LOADING AND SAVE
     *
     ********************************************************************/

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
    void setTopModel(vpz::CoupledModel* cp);

    /**
     * return the top model.
     *
     * @return top model of tree.
     */
    inline vpz::CoupledModel* getTopModel() {
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
     * get a class model vpz::CoupledModel with name.
     *
     * @param std::string name get vpz::CoupledModel
     * @return vpz::CoupledModel reference to class model, NULL if
     * class model doesn't exist.
     */
    inline vpz::BaseModel* getClassModel(const std::string& name)
        { return mVpz.project().classes().get(name).model(); }

    std::string getSelectedClass()
        { return mSelectedClass; }

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
    MapStringModel buidMapString(vpz::BaseModel* m);

    /**
     * This recursive function is call by parseStructures ; It parse graph
     * structure with m is current node and add each node in msm.
     *
     * @param m model node.
     * @param msm string model map.
     */
    void parseStructures(vpz::BaseModel* m, MapStringModel& msm);


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
     * Set the vpz modification status
     *
     * Also emit a correspondind  event if needed.
     *
     * @param modified a boolean status of the vpz modification
     */
    inline void setModified(bool modified) {
        if (mIsModified != modified)
        {
            mIsModified = modified;
            sendSignalModified();
        }
    }

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

    void exportCoupledModel(vpz::CoupledModel* model, vpz::Vpz* dst, std::string className);

    void importModel(vpz::Vpz* src);

    void importModelToClass(vpz::Vpz* src, std::string& className);

    void exportClass(vpz::BaseModel* model, vpz::Class classe, vpz::Vpz* dst);

    /********************************************************************
     *
     * MANAGE MODELS NAME
     *
     ********************************************************************/

    /**
     * delete all string from list.
     */
    void delNames();

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
    { setModified(true); return mVpz.project().dynamics();
    }

    const vpz::Observables& observables() const
    { return mVpz.project().experiment().views().observables(); }

    vpz::Observables& observables()
    { setModified(true); return mVpz.project().experiment().views().observables();
    }

    const vpz::Conditions& conditions() const
    { return mVpz.project().experiment().conditions(); }

    vpz::Conditions& conditions()
    { setModified(true); return mVpz.project().experiment().conditions();
    }

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

    vpz::CoupledModel* newCoupledModel(
        vpz::CoupledModel* parent,
        const std::string& name,
        const std::string& description,
        int x, int y);

    vpz::AtomicModel* newAtomicModel(
        vpz::CoupledModel* parent,
        const std::string& name,
        const std::string& description,
        int x, int y);

    vpz::BaseModel* newPluginModel(
        vpz::CoupledModel* parent,
        const std::string& name,
        const std::string& description,
        int x, int y);

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

    inline utils::Rand* getRand() const
    { return mRand; }

    /********************************************************************
     *
     * GET PIECES OF INFORMATION
     *
     ********************************************************************/

    std::string getDynamicInfo(std::string dynamicName) const;

    std::string getIdCard(std::string className) const;

    std::string getIdCard(vpz::BaseModel* model) const;

    std::string getIdCard(vpz::CoupledModel* model) const;

    std::string getInfoCard(std::string cond) const;

    std::string getClassIdCard(vpz::BaseModel* model) const;

    std::string getClassIdCard(vpz::CoupledModel* model) const;

    std::string getIdCardConnection(vpz::BaseModel* src,
                                    vpz::BaseModel* dest,
                                    vpz::CoupledModel* mTop) const;

    std::string getIdCardConnection(vpz::BaseModel* src,
                                    std::string srcport,
                                    vpz::BaseModel* dest,
                                    std::string destport,
                                    vpz::CoupledModel* mTop) const;

private:

    vpz::Vpz                       mVpz;
    vpz::CoupledModel*             mTop;
    std::string                    mCurrentClass;
    std::string                    mSelectedClass;
    utils::Rand*                   mRand;
    bool                           mIsModified;
    bool                           mIsSaved;
    bool                           mIsCompressed;
    std::string                    mFileName;
    SetString                      mModelsNames;
    int                            mSocketPort;

    void export_atomic_model(vpz::Vpz* dst,
                             vpz::AtomicModel* atom);

    void export_coupled_model(vpz::Vpz* dst,
                              vpz::CoupledModel* atom,
                              std::string className = "");

protected:
        SignalModified mSignalModified;

};

} } // namespace vle gvle

#endif
