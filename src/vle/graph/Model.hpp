/**
 * @file vle/graph/Model.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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


#ifndef VLE_GRAPH_MODEL_HPP
#define VLE_GRAPH_MODEL_HPP

#include <vle/graph/ModelPortList.hpp>
#include <vle/graph/DllDefines.hpp>
#include <ostream>
#include <vector>
#include <map>
#include <list>
#include <set>

namespace vle { namespace graph {

    class Model;
    class AtomicModel;
    class CoupledModel;

    typedef std::map < std::string, ModelPortList > ConnectionList;
    typedef std::set < std::string > PortList;
    typedef std::vector < AtomicModel * > AtomicModelVector;
    typedef std::vector < CoupledModel* > CoupledModelVector;
    typedef std::map < std::string, Model* > ModelList;

    /**
     * @brief The DEVS model base class.
     *
     */
    class VLE_GRAPH_EXPORT Model
    {
    public:
        /**
         * @brief Constructor to intialize parent, position (0,0), size (0,0)
         * and name.
         * @param name the new name of this model.
         * @param parent the parent of this model, can be null if parent does
         * not exist.
         */
        Model(const std::string& name, CoupledModel* parent);

        Model(const Model& mdl);

        void swap(Model& mdl);

        virtual Model* clone() const = 0;

        virtual ~Model() { }

        ////
        //// Base class.
        ////

        /**
         * @brief Return true if this is AtomicModel. Default is false.
         * @return true if Model is atomic, false otherwise.
         */
        virtual bool isAtomic() const
        { return false; }

        /**
         * @brief Return true if this is CoupledModel. Default is false.
         * @return true if Model is coupled, false otherwise.
         */
        virtual bool isCoupled() const
        { return false; }

        /**
         * Find recursively a model, atomic or coupled, with a specified name.
         * @param name model name to search.
         * @return model founded, otherwise 0.
         */
        virtual Model* findModel(const std::string& name) const = 0;

        /**
         * @brief Write the model in the output stream.
         * @param out output stream.
         */
        virtual void writeXML(std::ostream& out) const = 0;

        void getAtomicModelsSource(const std::string& portname,
                                   ModelPortList& result);

        void getAtomicModelsTarget(const std::string& portname,
                                   ModelPortList& result);

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * model base
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        inline const std::string& getName() const
        { return m_name; }

        /**
         * @brief change the name of the specified model to the new name. if mdl
         * have a parent, the this coupled model change is model list.
         * @param mdl graph::model to change.
         * @param newmane the new name.
         */
        static void rename(Model* mdl, const std::string& newmane);

        /**
         * @brief build a std::string based on the construction of the
         * concatenation of all coupled model parent. each parent name are
         * separated by a comma.
         * @code
         * top model,coupled modela
         * @endcode
         *
         * @return
         */
        std::string getParentName() const;

        /**
         * @brief Build a list of coupled model parents. The first model in the
         * vector is the direct parent of model.
         * @param parents A output vector.
         */
        void getParents(CoupledModelVector& parents) const;

        /**
         * @brief Get the atomic models from a CoupledModelVector of an another
         * devs graph hierarchy.
         * @param lst the list of coupled model from the another devs graph.
         * @param name the model to find.
         * @return A reference to the AtomicModel found.
         */
        Model* getModel(const CoupledModelVector& lst,
                        const std::string& name);

        /**
         * @brief Get the parent node of this model. Can be null if parent does
         * not exist.
         * @return A reference to the parent node or null.
         */
        inline CoupledModel* getParent() const
        { return m_parent; }

        /**
         * @brief Set the parent node of this model. Can be null.
         * @param cp The reference to the parent node or null.
         */
        inline void setParent(CoupledModel* cp)
        { m_parent = cp; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Port managment.
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        bool existInputPort(const std::string& name) const
        { return m_inPortList.find(name) != m_inPortList.end(); }

        bool existOutputPort(const std::string& name) const
        { return m_outPortList.find(name) != m_outPortList.end(); }

        const ModelPortList& getInPort(const std::string& name) const;
        const ModelPortList& getOutPort(const std::string& name) const;
        ModelPortList& getInPort(const std::string& name);
        ModelPortList& getOutPort(const std::string& name);

        int getInputPortNumber() const
        { return m_inPortList.size(); }

        int getOutputPortNumber() const
        { return m_outPortList.size(); }

        ModelPortList& addInputPort(const std::string& name);
        ModelPortList& addOutputPort(const std::string& name);

        /**
         * @brief Delete an input port with specified name and destroy input or
         * internal connections if exist.
         * @param name port name to delete.
         */
        void delInputPort(const std::string & name);

        /**
         * @brief Delete an output port with specified name and destroy output
         * or internal connections if exist.
         * @param name port name to delete.
         */
        void delOutputPort(const std::string & name);

	void addInPort(ModelPortList& model, ModelPortList& intern,
		       const std::string& name);
	void addOutPort(ModelPortList& model, ModelPortList& intern,
			const std::string& name);
	ModelPortList& renameInputPort(const std::string& old_name,
				       const std::string& new_name);
	ModelPortList& renameOutputPort(const std::string& old_name,
					const std::string& new_name);

        void addInputPort(const std::list < std::string > & lst);
        void addOutputPort(const std::list < std::string > & lst);
        bool existInputPort(const std::string & name);
        bool existOutputPort(const std::string & name);

        /**
         * @brief Get the index position of the input port into the input port
         * list.
         * @param name The name of the input port to find.
         * @return An integer greather than 0.
         * @throw utils::DevsGraphError if output port not exit.
         */
        int getInputPortIndex(const std::string& name) const;

        /**
         * @brief Get the index position of the output port into the output port
         * list.
         * @param name The name of the output port to find.
         * @return A integer greather than 0.
         * @throw utils::DevsGraphError if output port not exit.
         */
        int getOutputPortIndex(const std::string& name) const;

        inline const ConnectionList& getInputPortList() const
        { return m_inPortList; }

        inline const ConnectionList& getOutputPortList() const
        { return m_outPortList; }

        inline ConnectionList& getInputPortList()
        { return m_inPortList; }

        inline ConnectionList& getOutputPortList()
        { return m_outPortList; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * XML manage
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Write the port list in the output stream.
         * @param out output stream.
         */
        void writePortListXML(std::ostream& out) const;

        /**
         * return cast of a Model to an AtomicModel. If Model is not an
         * AtomicModel, return NULL
         *
         * @param m Model to cast to AtomicModel
         *
         * @return a cast AtomicModel or NULL if error
         */
        static AtomicModel* toAtomic(Model* m);

        /**
         * return cast of a Model to a CoupledModel. If Model is not a
         * CoupledModel, return NULL
         *
         * @param m Model to cast to CoupledModel
         *
         * @return a cast CoupledModel or NULL if error
         */
        static CoupledModel* toCoupled(Model* m);

        /**
         * @brief Cast the current graph::Model to graph::CoupledModel.
         *
         * @return A graph::CoupledModel pointer or 0 if this model is not a
         * coupled model.
         */
        CoupledModel* toCoupled() { return toCoupled(this); }

        /**
         * @brief Cast the current graph::Model to graph::AtomicModel.
         *
         * @return A graph::AtomicModel pointer of 0 if this model is not an
         * atomic model.
         */
        AtomicModel* toAtomic() { return toAtomic(this); }

        static void getAtomicModelList(Model* model,
                                       std::vector < AtomicModel* >& list);

        /**
         * return true if model m is present in Model List
         *
         * @param lst list to search m
         * @param m model to find in lst
         *
         * @return true if found, else false
         */
        static bool isInList(const ModelList& lst, graph::Model* m);

        ////
        //// Graphics position to replace GModel declaration.
        ////

        /**
         * @brief Get X position of model.
         * @return a X position.
         */
        inline int x() const { return m_x; }

        /**
         * @brief Get Y position of model.
         * @return a Y position.
         */
        inline int y() const { return m_y; }

        /**
         * @brief Get the width of model.
         * @return a width.
         */
        inline int width() const { return m_width; }

        /**
         * @brief Get the height of model.
         * @return a height.
         */
        inline int height() const { return m_height; }

        /**
         * @brief Set a new X position to model.
         * @param x new X position.
         */
        inline void setX(int x) { m_x = x; }

        /**
         * @brief Set a new Y position to model.
         * @param y new Y position.
         */
        inline void setY(int y) { m_y = y; }

        /**
         * @brief Set a new width to model.
         * @param width new width.
         */
        inline void setWidth(int width) { m_width = width; }

        /**
         * @brief Set a new height to model.
         * @param height new height.
         */
        inline void setHeight(int height) { m_height = height; }

        /**
         * @brief Set a new position to model.
         * @param x new X position.
         * @param y new Y position.
         */
        inline void setPosition(int x, int y) { setX(x); setY(y); }

        /**
         * @brief Set a new site to model.
         * @param width a new width.
         * @param height a new height.
         */
        inline void setSize(int width, int height)
        { setWidth(width); setHeight(height); }

        ////
        //// Functor
        ////

        struct AddInputPort
        {
            AddInputPort(Model& mdl) : mdl(mdl) { }

            inline void operator()(const std::string& name) const
            { mdl.addInputPort(name); }

            Model& mdl;
        };

        struct AddOutputPort
        {
            AddOutputPort(Model& mdl) : mdl(mdl) { }

            inline void operator()(const std::string& name) const
            { mdl.addOutputPort(name); }

            Model& mdl;
        };

        struct CopyWithoutConnection
        {
            CopyWithoutConnection(ConnectionList& cnt) : cnt(cnt) { }

            inline void operator()(const ConnectionList::value_type& x) const
            { cnt[x.first] = ModelPortList(); }

            ConnectionList& cnt;
        };

    protected:
        CoupledModel*   m_parent;
        ConnectionList  m_inPortList;
        ConnectionList  m_outPortList;

        int             m_x;
        int             m_y;
        int             m_width;
        int             m_height;

    private:
        /**
         * @brief Default constructor, position (0,0) size (0,0) and parent to
         * null.
         */
        Model();

        Model& operator=(const Model& mdl);

        std::string     m_name;
    };

}} // namespace vle graph

#endif
