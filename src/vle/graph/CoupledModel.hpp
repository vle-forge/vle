/**
 * @file graph/CoupledModel.hpp
 * @author The VLE Development Team.
 * @brief Represent the DEVS coupled model. This class have a list of children
 * models, three list of input, output and state connections.
 */

/*
 * Copyright (C) 2004, 05, 06 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef VLE_GRAPH_COUPLED_MODEL_HPP
#define VLE_GRAPH_COUPLED_MODEL_HPP

#include <vle/graph/Model.hpp>
#include <vle/graph/Connection.hpp>
#include <vector>
#include <list>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>



namespace vle { namespace graph {

    class Connection;
    class TargetPort;
    class CoupledModel;

    /**
     * @brief Represent the DEVS coupled model. This class have a list of
     * children models, three list of input, output and state connections.
     *
     */
    class CoupledModel : public Model
    {
    public:
        CoupledModel(CoupledModel* parent);

        CoupledModel(const std::string& name, CoupledModel* parent);

	CoupledModel(const CoupledModel& model);

	virtual ~CoupledModel();

	virtual Model* clone() const;

        /**
         * @brief add a model into model list. Parent is set to this coupled
         * model.
         *
         * @param model model to add.
         *
         * @throw Exception::Internal if model is null or already exist in
         * hierarchy.
         */
        void addModel(Model* model);

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
         * @brief add a new novle model to the list. Parent is set to this
         * coupled model.
         * 
         * @param name novle model name to add.
         * 
         * @return The NoVLE builded by this function.
         *
         * @throw Exception::Internal if name already exist in this
         * coupledmodel.
         */
        NoVLEModel* addNoVLEModel(const std::string& name);
        
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
        void delModel(Model* model);

        /** 
         * @brief Delete all model from this coupled model. If connection
         * exist, there are destroy.
         */
        void delAllModel();

        /**
         * attach an existing model into this coupled model. Parent will be
         * informed and detached of this model.
         *
         * @param model a model to attach
         */
        void attachModel(Model* model);

        /** 
         * @brief Attach a list of models into this coupled model. Parent will
         * be informed and detached of this model.
         * 
         * @param models a list of models.
         */
        void attachModels(VectorModel& models);

        /**
         * @brief Detach a model (not delete !) of model list, and parent is
         * set to null.
         *
         * @param model a model to detach.
         */
        void detachModel(Model* model);

        /** 
         * @brief Detach a list of model from this coupled model. For each
         * model, there parent variable is set to null.
         * 
         * @param models a list of model.
         */
        void detachModels(const VectorModel& models);

        Model* getModel(const std::string& modelName);

        /**
         * @brief Return pointer to model if model found into childrens models.
         *
         * @param model to find into childrens models
         *
         * @return pointer to model if found into childrens models
         */
        Model* getModel(Model* model);

        inline const VectorModel& getModelList() const
        { return m_modelList; }
        
        inline VectorModel& getModelList()
        { return m_modelList; }

        const VectorConnection& getInputConnectionList() const;

        const VectorConnection& getOutputConnectionList() const;

        const VectorConnection& getInternalConnectionList() const;

	void addInputConnection(Model* src, const std::string& portSrc,
                                Model* dst, const std::string& portDst);

	void addOutputConnection(Model* src, const std::string& portSrc,
                                 Model* dst, const std::string& portDst);

	void addInternalConnection(Model* src, const std::string& portSrc,
                                   Model* dst, const std::string& portDst);

        void addInputConnection(const std::string& src,
                                const std::string& portSrc,
                                const std::string& dst,
                                const std::string& portDst);

        void addOutputConnection(const std::string& src,
                                 const std::string& portSrc,
                                 const std::string& dst,
                                 const std::string& portDst);

        void addInternalConnection(const std::string& src,
                                   const std::string& portSrc,
                                   const std::string& dst,
                                   const std::string& portDst);

        /**
         * return a pointer to input connection with parameter srcModel
         * srcPort dstModel and dstPort
         *
         * @param src
         * @param portSrc
         * @param dst
         * @param portDst
         *
         * @return pointer to input connection or 0 if not found
         */
        Connection* getInputConnection(Model* src, const std::string& portSrc,
                                       Model* dst, const std::string& portDst);

        /** 
         * @brief Convenient function to get a input connection between to models.
         * 
         * @param portSrc name of the input port of coupled model.
         * @param dst name of the output model.
         * @param portDst name of the output port.
         * 
         * @return A reference to the connection or 0 otherwise.
         */
        Connection* getInputConnection(const std::string& portSrc,
                                       const std::string& dst,
                                       const std::string& portDst);

        /** 
         * @brief Convenient function to get a output connection between to models.
         * 
         * @param src name of the input model.
         * @param portSrc name of the input port.
         * @param portDst name of the output port of coupled model.
         * 
         * @return A reference to the connection or 0 otherwise.
         */
        Connection* getOutputConnection(const std::string& src,
                                        const std::string& portSrc,
                                        const std::string& portDst);

        /** 
         * @brief Convenient function to get a internal connection between to
         * models.
         * 
         * @param src name of the input model.
         * @param portSrc name of the input port.
         * @param dst name of the output model.
         * @param portDst name of the output port.
         * 
         * @return A reference to the connection or 0 otherwise.
         */
        Connection* getInternalConnection(const std::string& src,
                                          const std::string& portSrc,
                                          const std::string& dst,
                                          const std::string& portDst);
        /**
         * return a pointer to output connection with parameter srcModel
         * srcPort dstModel and dstPort
         *
         * @param src
         * @param portSrc
         * @param dst
         * @param portDst
         *
         * @return pointer to ouput connection or 0 if not found
         */
        Connection* getOutputConnection(Model* src, const std::string& portSrc,
                                        Model* dst, const std::string& portDst);

        /**
         * return a pointer to internal connection with parameter srcModel
         * srcPort dstModel and dstPort
         *
         * @param src
         * @param portSrc
         * @param dst
         * @param portDst
         *
         * @return pointer to internal connection or 0 if not found
         */
        Connection* getInternalConnection(Model* src, const std::string&
                                          portSrc, Model* dst, const
                                          std::string& portDst);

	void delInputConnection(Model * src, const std::string & portSrc,
				Model * dst, const std::string & portDst);
	void delOutputConnection(Model * src, const std::string & portSrc,
				 Model * dst, const std::string & portDst);
	void delInternalConnection(Model * src, const std::string & portSrc,
				   Model * dst, const std::string & portDst);
	void delInputConnection(Connection * connect);
	void delOutputConnection(Connection * connect);
	void delInternalConnection(Connection * connect);
	void delConnection(Connection * connect);
        
	void attachInternalConnection(const std::list < Connection * > & c);
        
        /**
         * @brief Delete all connection around model m.
         *
         * @param m model to delete connection.
         */
	void delAllConnection(Model* m);
        
        /**
         * @brief delete all connection (input, output and internal) into
         * current CoupledModel.
         *
         */
        void delAllConnection();

        /** 
         * @brief Replace the old model (Atomic, Coupled or NoVLE) into the new
         * model mdl (Atomic, Coupled or NoVLE). This function delete the model
         * old and set the new mdl, no clone is build.
         * 
         * @param old the model to delete from hierarchy.
         * @param mdl the new model to set.
         *
         * @throw Exception::Internal if old does not exist or if old or mdl
         * are null.
         */
        void replace(Model* old, Model* mdl);

        /**
         * Test if model has a connection with port name in input, output or
         * internal with anoter model.
         *
         * @param model graph::model to test port connection.
         * @param name port name of model to test.
         *
         * @return true if connection exist, false otherwise.
         */
        Connection* hasConnection(Model* model, const std::string& name);

        /**
         * Test if model has a connection with port name in input, output or
         * internal with anoter model.
         *
         * @param model graph::model to test port connection.
         * @param name port name of model to test.
         *
         * @return Connection found or NULL if no connection found.
         */
        bool hasConnection(Model* model, const std::string& name) const;

        /**
         * return true if Model list has no conflict with other Model to make
         * a CoupledModel
         *
         * @param lst list of gmodel to test
         *
         * @return true if model is found, else false
         */
        bool hasConnectionProblem(const VectorModel& lst) const;

	std::vector < TargetPort* > getTargetPortList(Port* port);

        virtual bool isAtomic() const;

        virtual bool isCoupled() const;

	virtual bool isNoVLE() const;

        /** 
         * @brief Recursive function to find the first model with the specified
         * name.
         * 
         * @param name model name to find.
         * 
         * @return A reference to the founded model, or 0.
         */
        virtual Model* findModel(const std::string& name) const;

        void writeXML(std::ostream& out) const;

        /** 
         * @brief Return a reference to the direct children model with
         * specified name.
         * 
         * @param name model name to find.
         * 
         * @return A reference to the founded model, or 0.
         */
        Model* find(const std::string& name) const;

        /** 
         * @brief Return a reference to the children graph::Model under the
         * specified position.
         * 
         * @param x the position of graph::Model.
         * @param y the position of graph::Model.
         * 
         * @return A reference to the founded model otherwise null.
         */
        Model* find(int x, int y) const;
        
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

    private:
	VectorModel      m_modelList;
	VectorConnection m_inputConnectionList;
	VectorConnection m_outputConnectionList;
	VectorConnection m_internalConnectionList;
    };

}} // namespace vle graph

#endif
