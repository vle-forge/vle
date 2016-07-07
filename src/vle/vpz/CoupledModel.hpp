/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
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


#ifndef VLE_GRAPH_COUPLED_MODEL_HPP
#define VLE_GRAPH_COUPLED_MODEL_HPP

#include <vle/vpz/BaseModel.hpp>
#include <vle/DllDefines.hpp>
#include <map>
#include <vector>

namespace vle { namespace vpz {

    /**
     * @brief Represent the DEVS coupled model. This class have a list of
     * children models, three list of input, output and state connections.
     */
    class VLE_API CoupledModel : public BaseModel
    {
    public:
        typedef std::vector < std::string > StringList;
        typedef std::map <std::string, ConnectionList> ModelConnections;

        /**
         * @brief Constructor to intialize parent, position (0,0), size (0,0)
         * and name.
         * @param name the new name of this coupled model.
         * @param parent the parent of this coupled model, can be null if parent
         * does not exist.
         */
        CoupledModel(const std::string& name, CoupledModel* parent);

        CoupledModel(const CoupledModel& mdl);

        CoupledModel& operator=(const CoupledModel& mdl);

        virtual BaseModel* clone() const override
        { return new CoupledModel(*this); }

        /**
         * @brief Delete his children.
         */
        virtual ~CoupledModel();

        /**
         * @brief Return true, CoupledModel is a coupled model.
         * @return true.
         */
        virtual bool isCoupled() const override { return true; }

        /**
         * @brief Write the coupled model in the output stream.
         * @param out output stream.
         */
        void writeXML(std::ostream& out) const override;

        ////
        //// Specific functions.
        ////

        /**
         * @brief add a model into model list. Parent is set to this coupled
         * model.
         *
         * @param model model to add.
         *
         * @throw Exception::Internal if model is null or already exist in
         * hierarchy.
         */
        void addModel(BaseModel* model);

        /**
         * @brief Add a model into the model list. Parent is set to thie coupled
         * model and the model name if changed.
         * @param model The model to add.
         * @param name The new name of the model.
         */
        void addModel(BaseModel* model, const std::string& name);

        /**
         * @brief add a new atomic model to the list. Parent is set to this
         * coupled model.
         *
         * @param name atomic model name to add.
         *
         * @return The AtomicModel builded by this function.
         *
         * @throw Exception::Internal if name already exist in this
         * coupledmodel.
         */
        AtomicModel* addAtomicModel(const std::string& name);

        /**
         * @brief add a new coupled model to the list. Parent is set to this
         * coupled model.
         *
         * @param name coupled model name to add.
         *
         * @return The CoupledModel builded by this function.
         *
         * @throw Exception::Internal if name already exist in this
         * coupledmodel.
         */
        CoupledModel* addCoupledModel(const std::string& name);

        /**
         * Delete a model of model list. If connection with this model exist,
         * there is deleted.
         *
         * @param model model to delete.
         *
         * @throw Exception::Internal if model is null.
         */
        void delModel(BaseModel* model);

        /**
         * @brief Delete all model from this coupled model. If connection
         * exist, there are destroy.
         */
        void delAllModel();

        /**
         * @brief Attach an existing model into this coupled model. Parent will
         * be informed and detached of this model.
         * @param model a model to attach.
         */
        void attachModel(BaseModel* model);

        /**
         * @brief Attach a list of models into this coupled model. Parent will
         * be informed and detached of this model.
         * @param models a list of models.
         */
        void attachModels(ModelList& models);

        /**
         * @brief Detach a model (not delete !) of model list, and parent is
         * set to null.
         *
         * @param model a model to detach.
         */
        void detachModel(BaseModel* model);

        /**
         * @brief Detach a list of model from this coupled model. For each
         * model, there parent variable is set to null.
         *
         * @param models a list of model.
         */
        void detachModels(const ModelList& models);

        /**
         * Find a model, atomic or coupled, with a specified name.
         * @param name model name to search.
         * @return model founded, otherwise 0.
         * @deprecated
         */
        BaseModel* findModel(const std::string& modelname) const override;

        /**
         * @brief Gets a model with the specified name
         * @param name the model name
         * @return A reference to the found model otherwise 0.
         */
        BaseModel* getModel(const std::string& name) const;


        void addInputConnection(const std::string& portSrc,
                                BaseModel* dst, const std::string& portDst);

        void addOutputConnection(BaseModel* src, const std::string& portSrc,
                                 const std::string& portDst);

        void addInternalConnection(BaseModel* src, const std::string& portSrc,
                                   BaseModel* dst, const std::string& portDst);

        void addInputConnection(const std::string& portSrc,
                                const std::string& dst,
                                const std::string& portDst);

        void addOutputConnection(const std::string& src,
                                 const std::string& portSrc,
                                 const std::string& portDst);

        void addInternalConnection(const std::string& src,
                                   const std::string& portSrc,
                                   const std::string& dst,
                                   const std::string& portDst);

        void delInputConnection(const std::string& portSrc,
                                BaseModel* dst, const std::string& portDst);
        void delOutputConnection(BaseModel* src, const std::string& portSrc,
                                 const std::string& portDst);
        void delInternalConnection(BaseModel* src, const std::string& portSrc,
                                   BaseModel* dst, const std::string& portDst);
        void delInputConnection(const std::string& portSrc,
                                const std::string& dst,
                                const std::string& portDst);
        void delOutputConnection(const std::string& src,
                                 const std::string& portSrc,
                                 const std::string& portDst);
        void delInternalConnection(const std::string& src,
                                   const std::string& portSrc,
                                   const std::string& Model,
                                   const std::string& portDst);

        bool existInputConnection(const std::string& portsrc,
                                  const std::string& dst,
                                  const std::string& portdst) const;

        bool existOutputConnection(const std::string& src,
                                   const std::string& portsrc,
                                   const std::string& portdst) const;

        bool existInternalConnection(const std::string& src,
                                     const std::string& portsrc,
                                     const std::string& dst,
                                     const std::string& portdst) const;

        bool existInternalInputPort(const std::string& name) const
        { return m_internalInputList.find(name) != m_internalInputList.end(); }

        bool existInternalOutputPort(const std::string& name) const
        { return m_internalOutputList.find(name) != m_internalOutputList.end(); }

        int nbInputConnection(const std::string& portsrc,
                              const std::string& dst,
                              const std::string& portdst);

        int nbOutputConnection(const std::string& src,
                               const std::string& portsrc,
                               const std::string& portdst);

        int nbInternalConnection(const std::string& src,
                                 const std::string& portsrc,
                                 const std::string& dst,
                                 const std::string& portdst);
        /**
         * @brief Delete all connection around model m.
         *
         * @param m model to delete connection.
         */
        void delAllConnection(BaseModel* m);

        /**
         * @brief delete all connection (input, output and internal) into
         * current CoupledModel.
         *
         */
        void delAllConnection();

        /**
         * @brief Replace the old model (Atomic or Coupled) into the new model
         * mdl (Atomic or Coupled). This function delete the model old and set
         * the new mdl, no clone is build.
         *
         * @param old the model to delete from hierarchy.
         * @param mdl the new model to set.
         *
         * @throw Exception::Internal if old does not exist or if old or mdl
         * are null.
         */
        void replace(BaseModel* old, BaseModel* mdl);

        /**
         * @brief Return a string representation of internal connection for the
         * specified model list. Each connection is represented by 4-uples
         * (modelsource, portsource, modeldestination, portdestination). For
         * instance:
         * @code
         * AtomicModel* a = addAtomicModel("a");
         * a->addOutputPort("out");
         * AtomicModel* b = addAtomicModel("b");
         * b->addOutputPort("in");
         *
         * ModelList lst;
         * lst["a"] = a;
         * lst["b"] = b;
         *
         * StringList lst;
         * getBasicConnections(lst);
         * // This function returns four strings: "a", "out", "b", "in";
         * @endcode
         * @param models The list of the models to get internal connections.
         * @return the string representation of the internal connections.
         * @throw vpz::DevsGraphError if a model does not belong this coupled
         * model.
         */
        StringList getBasicConnections(const ModelList& models) const;

        /**
         * @brief Set a list of connection to this coupled model.Each connection
         * is represented by 4-uples (modelsource, portsource, modeldestination,
         * portdestination). For instance:
         * @code
         * AtomicModel* a = addAtomicModel("a");
         * a->addOutputPort("out");
         * AtomicModel* b = addAtomicModel("b");
         * b->addOutputPort("in");
         *
         * StringList cnts;
         * cnts.push_back("a");
         * cnts.push_back("out");
         * cnts.push_back("b");
         * cnts.push_back("in");
         * setBasicConnections(cnts);
         * @endcode
         * @param lst The list of 4-uples basics connections.
         * @throw DevsGraphError if a model not exist, if a port of a model not
         * exist or if a connection already exist.
         */
        void setBasicConnections(const StringList& lst);

        void displace(ModelList& models, CoupledModel* destination);

        /**
         * @brief save all the input connections in relation with
         * selected models
         * @param models the selected models
         * @return a list of the connections
         *
         */
        ModelConnections saveInputConnections(
            ModelList& models);

        /**
         * @brief save all the output connections in relation with
         * selected models
         * @param models the selected models
         * @return a list of the connections
         *
         */
        ModelConnections  saveOutputConnections(
            ModelList& models);

        /**
         * @brief restore the input connections in relation with the
         * selected models, with a new Coupled model
         * @param models the selected models
         * @param destination the new coupled model
         * @param conenction the old connections
         *
         */
        void restoreInputConnections(ModelList& models,
                                     CoupledModel* destination,
                                     ModelConnections connection);
        /**
         * @brief restore the output connections in relation with the
         * selected models, with a new Coupled model
         * @param models the selected models
         * @param destination the new coupled model
         * @param conenction the old connections
         *
         */
        void restoreOutputConnections(CoupledModel* destination,
                                      ModelConnections connections);

        /**
         * @brief init the connections models
         */
        void initConnections();

        /**
         * @brief init the internal input connections
         */
        void initInternalInputConnections();

        /**
         * @brief init the internal output connections
         */
        void initInternalOutputConnections();

        /**
         * @brief calculate the distance between two models
         * @param src the first model
         * @param dst the second model
         */
        float distanceModels(BaseModel* src, BaseModel* dst);

        /**
         * @brief calculate the repulsion force for all models
         */
        void repulsionForce();

        /**
         * @brief calculate the atttraction force of all models
         */
        void attractionForce();

        /**
         * @brief set the new position
         */
        bool newPosition();

        /**
         * @brief order the models
         */
        void order();

        /**
         * @brief return true if the model list has no connection with another
         * model which are no part of the model list.
         * @param lst list of models to test
         * @return true if model is found, else false
         */
        bool hasConnectionProblem(const ModelList& lst) const;

        /**
         * @brief Return true if the connection list have connection with models
         * not added in model lists provided.
         * @param cnts the list of connection to check.
         * @param mdls the list of models.
         * @return true if a connection with another model is founded,
         * otherwise, return false.
         */
        bool haveConnectionWithOtherModel(const ConnectionList& cnts,
                                          const ModelList& mdls) const;


        void writeConnections(std::ostream& out) const;

        /**
         * @brief Return a reference to the children Model under the
         * specified position.
         *
         * @param x the position of Model.
         * @param y the position of Model.
         *
         * @return A reference to the founded model otherwise null.
         */
        BaseModel* find(int x, int y) const;

        /**
         * @brief Return a reference to the children Model under the
         * specified position.
         *
         * @param x the position of Model.
         * @param y the position of Model.
         * @param width the width of the simple Model
         * @param height the height of the simple Model
         *
         * @return A reference to the founded model otherwise null.
         */
        BaseModel* find(int x, int y, int width, int height) const;

        /**
         * @brief Build a new string for model name. This function assert that
         * no name is already defined in this coupled model.
         *
         * @param prefix a prefix of model.
         *
         * @return a new model name.
         */
        std::string buildNewName(const std::string& prefix) const;

        /**
         * @brief Test if the name already exist in model list.
         *
         * @param name the model name to test.
         *
         * @return true if model name exist, false otherwise.
         */
        inline bool exist(const std::string& name) const
        { return m_modelList.find(name) != m_modelList.end(); }

        //
        /// Get functions.
        //

        inline const ModelList& getModelList() const
        { return m_modelList; }

        inline ModelList& getModelList()
        { return m_modelList; }

        inline const ConnectionList& getInternalInputPortList() const
        { return m_internalInputList; }

        inline const ConnectionList& getInternalOutputPortList() const
        { return m_internalOutputList; }

        inline ConnectionList& getInternalInputPortList()
        { return m_internalInputList; }

        inline ConnectionList& getInternalOutputPortList()
        { return m_internalOutputList; }

        ModelPortList& getInternalInPort(const std::string& name);

        const ModelPortList& getInternalInPort(const std::string& name) const;

        ModelPortList& getInternalOutPort(const std::string& name);

        const ModelPortList& getInternalOutPort(const std::string& name) const;

        ////
        //// Functors
        ////

        /**
         * @brief A functor to easily attach ModelList::value_type. Use it with
         * std::for_each.
         */
        struct AttachModel
        {
            AttachModel(CoupledModel* model) : model(model) { }

            inline void operator()(const ModelList::value_type& val)
            { model->attachModel(val.second); }

            CoupledModel*   model;
        };

        /**
         * @brief A functor to easily updateDynamics of ModelList::value_type. Use it with
         * std::for_each.
         */
        struct UpdateDynamics
        {
            UpdateDynamics(const std::string& oldname,
                           const std::string& newname) :
                oldname(oldname), newname(newname)
            { }

            inline void operator()(const ModelList::value_type& value)
            { value.second->updateDynamics(oldname, newname); }

            std::string oldname;
            std::string newname;
        };

        /**
         * @brief A functor to easily purgeDynamics of ModelList::value_type. Use it with
         * std::for_each.
         */
        struct PurgeDynamics
        {
            PurgeDynamics(const std::set < std::string > & dynamics) :
                dynamics(dynamics)
            { }

            inline void operator()(const ModelList::value_type& value)
            { value.second->purgeDynamics(dynamics); }

            std::set < std::string > dynamics;
        };

        /**
         * @brief A functor to easily updateObservable of ModelList::value_type. Use it with
         * std::for_each.
         */
        struct UpdateObservable
        {
            UpdateObservable(const std::string& oldname,
                             const std::string& newname) :
                oldname(oldname), newname(newname)
            { }

            inline void operator()(const ModelList::value_type& value)
            { value.second->updateObservable(oldname, newname); }

            std::string oldname;
            std::string newname;
        };

        /**
         * @brief A functor to easily purgeObservable of ModelList::value_type. Use it with
         * std::for_each.
         */
        struct PurgeObservable
        {
            PurgeObservable(const std::set < std::string > & observables) :
                observables(observables)
            { }

            inline void operator()(const ModelList::value_type& value)
            { value.second->purgeObservable(observables); }

            std::set < std::string > observables;
        };

        /**
         * @brief A functor to easily updateConditions of ModelList::value_type. Use it with
         * std::for_each.
         */
        struct UpdateConditions
        {
            UpdateConditions(const std::string& oldname,
                             const std::string& newname) :
                oldname(oldname), newname(newname)
            { }

            inline void operator()(const ModelList::value_type& value)
            { value.second->updateConditions(oldname, newname); }

            std::string oldname;
            std::string newname;
        };

        /**
         * @brief A functor to easily purgeConditions of ModelList::value_type. Use it with
         * std::for_each.
         */
        struct PurgeConditions
        {
            PurgeConditions(const std::set < std::string > & conditions) :
                conditions(conditions)
            { }

            inline void operator()(const ModelList::value_type& value)
            { value.second->purgeConditions(conditions); }

            std::set < std::string > conditions;
        };

        /**
         * @brief A functor to easily detach ModelList::value_type. Use it with
         * std::for_each.
         */
        struct DetachModel
        {
            DetachModel(CoupledModel* model) : model(model) { }

            inline void operator()(const ModelList::value_type& value)
            { model->detachModel(value.second); }

            CoupledModel*   model;
        };

        /**
         * @brief A functor to easily delete ModelList::value_type. Use it with
         * std::for_each.
         */
        struct DeleteModel
        {
            DeleteModel(CoupledModel* model) : model(model) { }

            void operator()(ModelList::value_type& value)
            {
                model->delAllConnection(value.second);
                delete value.second;
                value.second = 0;
            }

            CoupledModel*   model;
        };

        /**
         * @brief A functor to easily test a position (x, y) in a range of a
         * ModelList::value_type. Use it with std::find_if, etc.
         */
        struct IsInModelList
        {
            IsInModelList(int x, int y) : x(x), y(y) { }

            inline bool operator()(const ModelList::value_type& value) const
            { return value.second->x() <= x and x <= value.second->x() +
                value.second->width() and value.second->y() <= y and y <=
                    value.second->y() + value.second->height(); };

                int x, y;
        };

        /**
         * @brief A functor to easily clone a ModelList. To use with the
         * for_each algorith.
         */
        struct CloneModel
        {
            CoupledModel* parent;

            CloneModel(CoupledModel* parent) : parent(parent) { }

            inline void operator()(ModelList::value_type& x) const
            { x.second = x.second->clone(); x.second->setParent(parent); }
        };

        /**
         * @brief Write the hierarchy of models into the output stream.
         * @param out The output stream.
         */
        void write(std::ostream& out) const override;

        /**
	 * @brief Update the dynamics of each AtomicModel
	 * where an oldname became newname.
	 * @param oldname the old name of the dynamics.
	 * @param newname the new name of the dynamics.
	 */
        virtual void updateDynamics(const std::string& oldname,
                                    const std::string& newname) override;

        /**
	 * @brief purge the dymamics not present in the list
	 * @param dynamicslist a list of dynamics name
	 */
	virtual void purgeDynamics(const std::set < std::string >& dynamicslist) override;

        /**
	 * @brief Update the Observable of each AtomicModels where an
	 * oldname became newname, for each model.
	 * @param oldname the old name of the observable.
	 * @param newname the new name of the observable.
	 */
	virtual void updateObservable(const std::string& oldname,
                                      const std::string& newname) override;

        /**
	 * @brief purge the observables references of the AtomicModels
	 * where the observable is not present in the list, for each model.
	 * @param observablelist a list of observable name
	 */
	virtual void purgeObservable(const std::set < std::string >& observablelist) override;

        /**
	 * @brief Update the Conditions of the AtomicModel where an
	 * oldname became newname, for each model.
	 * @param oldname the old name of the observable.
	 * @param newname the new name of the observable.
	 */
	virtual void updateConditions(const std::string& oldname,
                                      const std::string& newname) override;

        /**
	 * @brief purge the Conditions references of the model where the
	 * Condition is not present in the list, for each model.
	 * @param conditionlist a list of condition name
	 */
	virtual void purgeConditions(const std::set < std::string >& conditionlist) override;

    private:
        void delConnection(BaseModel* src, const std::string& portSrc,
                           BaseModel* dst, const std::string& portDst);

        /**
         * @brief Copy input and output connections list from src to dst. dst.
         * @param src The source of the copy.
         * @param dst The destination of the copy.
         */
        void copyConnection(const ConnectionList& src, ConnectionList& dst);

        /**
         * @brief Copy the connection from ModelPortList src to the
         * ModelPortList dst.
         * @param src The source of the copy.
         * @param dst The destination of the copy.
         */
        void copyPort(const ModelPortList& src, ModelPortList& dst);

        /**
         * @brief Copy internal connections list from src to dst.
         * @param src The source of the copy.
         * @param dst The destination of the copy.
         * @param parentSrc Parent of src's ConnectionList.
         * @param parentDst Parent of dst's ConnectionList.
         */
        void copyInternalConnection(const ConnectionList& src,
                                    ConnectionList& dst,
                                    const BaseModel& parentSrc,
                                    BaseModel& parentDst);

        /**
         * @brief Copy the connection from ModelPortList src to the
         * ModelPortList dst.
         * @param src The source of the copy.
         * @param dst The destination of the copy.
         * @param parentSrc Parent of src's ModelPortList.
         * @param parentDst Parent of dst's ModelPortList.
         */
        void copyInternalPort(const ModelPortList& src, ModelPortList& dst,
                              const BaseModel& parentSrc, BaseModel& parentDst);

        ModelList       m_modelList;
        ConnectionList  m_internalInputList;
        ConnectionList  m_internalOutputList;

        /* Connections */
        std::vector < BaseModel* > m_srcConnections;
        std::vector < BaseModel* > m_dstConnections;

        void writeConnection(std::ostream& out) const;
    };

}} // namespace vle vpz

#endif
