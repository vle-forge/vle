/**
 * @file graph/Model.hpp
 * @author The VLE Development Team.
 * @brief The DEVS model base class.
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

#ifndef VLE_GRAPH_MODEL_HPP
#define VLE_GRAPH_MODEL_HPP

#include <libxml++/libxml++.h>
#include <map>
#include <string>
#include <list>
#include <vector>



namespace vle { namespace graph {

    class Model;
    class AtomicModel;
    class CoupledModel;
    class NoVLEModel;
    class Port;

    typedef std::map < std::string, Port * > MapStringPort;
    typedef std::vector < AtomicModel * > AtomicModelVector;
    typedef std::vector < Model* > VectorModel;
    
    /**
     * @brief The DEVS model base class.
     *
     */
    class Model
    {
    public:
        /** 
         * @brief Default constructor, position (0,0) size (0,0) and parent to
         * null.
         */
        Model();

        /** 
         * @brief Constructor to initialize parent, position (0,0), size (0,0)
         * and parent to parameter parent.
         * 
         * @param parent 
         */
        Model(CoupledModel* parent);

        /** 
         * @brief Copy constructuor. Be carefull to modify the parent.
         * 
         * @param model copy constructor.
         */
        Model(const Model& model);

        /** 
         * @brief Affect operator. Be carefull to modify the parent.
         * 
         * @param mdl model to copy.
         * 
         * @return a reference to the current model.
         */
        Model& operator=(const Model& mdl);

	virtual ~Model();

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Base class define
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /** @return clone of class. */
        virtual Model* clone() const = 0;

        /** @return true if Model is atomic, false otherwise. */
        virtual bool isAtomic() const = 0;

        /** @return true if Model is coupled, false otherwise. */
        virtual bool isCoupled() const = 0;

        /** @return true if Model is novle, false otherwise. */
        virtual bool isNoVLE() const = 0;

        /**
	 * Find recursively a model, atomic or coupled, with a specified name.
         *
	 * @param name model name to search.
         *
	 * @return model founded, otherwise 0.
         */
	virtual Model* findModel(const std::string& name) const = 0;

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Model base
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        inline const std::string& getDescription() const
        { return m_description; }

        inline void setDescription(const std::string& dest)
        { m_description.assign(dest); }

        inline const std::string& getName() const
        { return m_name; }

        inline void setName(const std::string& name)
        { m_name.assign(name); }

        inline CoupledModel * getParent() const
        { return m_parent; }

        inline void setParent(CoupledModel* cp)
        { m_parent = cp; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Port managment.
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	int getInitPortNumber() const;
	int getInPortNumber() const;
	int getOutPortNumber() const;
	int getStatePortNumber() const;
	Port * getPort(const std::string & name) const;
	Port * getInitPort(const std::string & name) const;
	Port * getInPort(const std::string & name) const;
	Port * getOutPort(const std::string & name) const;
	Port * getStatePort(const std::string & name) const;
	int getInitPortIndex(const std::string & name) const;
	int getInputPortIndex(const std::string & name) const;
	int getOutputPortIndex(const std::string & name) const;
	int getStatePortIndex(const std::string & name) const;
	void addInitPort(const std::string & name);
	void addInputPort(const std::string & name);
	void addOutputPort(const std::string & name);
	void addStatePort(const std::string & name);

        /**
	 * Delete an init port with specified name.
         *
	 * @param name port name to delete.
         *
	 * @return true if success, otherwise false.
         */
	bool delInitPort(const std::string & name);

        /**
	 * Delete an input port with specified name.
         *
	 * @param name port name to delete.
         *
	 * @return true if success, otherwise false if connection exist.
         */
        bool delInputPort(const std::string & name);

        /**
	 * Delete an output port with specified name.
         *
	 * @param name port name to delete.
         *
	 * @return true if success, otherwise false if connection exist.
         */
        bool delOutputPort(const std::string & name);

        /**
	 * Delete a state port with specified name.
         *
	 * @param name port name to delete.
         *
	 * @return true if success, otherwise false.
         */
	bool delStatePort(const std::string & name);

        /**
	 * Delete all connection on port name (input or output).
         *
	 * @param name port name to find connection and delete it.
         */
        void delConnectionOnPort(const std::string& name);

        /**
	 * Delete an input port with specified name and destroy input or
         * internal connections if exist.
         *
	 * @param name port name to delete.
         */
        void delInputPortAndConnection(const std::string& name);

        /**
	 * Delete an output port with specified name and destroy output
         * or internal connections if exist.
         *
	 * @param name port name to delete.
         */
        void delOutputPortAndConnection(const std::string& name);

	void addInitPort(const std::list < std::string > & lst);
	void addInputPort(const std::list < std::string > & lst);
	void addOutputPort(const std::list < std::string > & lst);
	void addStatePort(const std::list < std::string > & lst);
	bool existInitPort(const std::string & name);
	bool existInputPort(const std::string & name);
	bool existOutputPort(const std::string & name);
	bool existStatePort(const std::string & name);
	void clearInitPort();
	void clearInputPort();
	void clearOutputPort();
	void clearStatePort();
	const MapStringPort& getInitPortList() const;
	const MapStringPort& getInputPortList() const;
	const MapStringPort& getOutputPortList() const;
	const MapStringPort& getStatePortList() const;

        /**
	 * Merge define port of this model with input, output, state and init
         * lists. If two port have same name be nothing, if port not in
         * parameter list port delete, if port not in this model add this port.
         * If connection exist with a deleted port, connection is delete.
	 *
         * @param inlist input list port name.
         * @param outlist ouput list port name.
         * @param statelist state list port name.
         * @param initlist init list port name.
         */
        void mergePort(std::list < std::string >& inlist,
                       std::list < std::string >& outlist,
                       std::list < std::string >& statelist,
                       std::list < std::string >& initlist);

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * XML manage
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
	 * Parse XML element structure who represent VLE model and attach it to
         * the coupledmodel parent.
	 *
         * @param modelNode XML element structure.
         * @param parent CoupledModel parent of XML element structure.
	 *
         * @return graph::Model read.
         */
        static Model* parseXMLmodel(xmlpp::Element* modelNode,
                                    CoupledModel* parent = 0);

        /**
	 * Parse XML element structure
	 *
         * @param modelNode XML element structure.
         * @param parent parent of XML element structure.
         * @return true if reading is ok, false otherwise.
         */
	virtual bool parseXML(xmlpp::Element* modelNode,
			      CoupledModel* parent) = 0;

        /**
	 * Write under XML node VLE structures ports.
	 *
         * @param elt XML node parent.
         */
        void writePortListXML(std::ostream& out) const;

        /**
	 * Write under XML node VLE structures.
	 *
         * @param elt XML node parent.
         */
	virtual void writeXML(std::ostream& out) const = 0;

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
	 * return cast of a Model to a NoVLEModel. If Model is not a
         * NoVLEModel, return NULL
         *
         * @param m Model to cast to NoVLEModel
	 *
         * @return a cast NoVLEModel or NULL if error
         */
        static NoVLEModel* toNoVLE(Model* m);

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
        static bool isInList(const VectorModel& lst, graph::Model* m);

        /*
         * 
         * Graphics position to replace GModel declaration.
         *
         */

        /** 
         * @brief Get X position of model.
         * 
         * @return a X position.
         */
        int x() const { return m_x; }

        /** 
         * @brief Get Y position of model.
         * 
         * @return a Y position.
         */
        int y() const { return m_y; }

        /** 
         * @brief Get the width of model.
         * 
         * @return a width.
         */
        int width() const { return m_width; }

        /** 
         * @brief Get the height of model.
         * 
         * @return a height.
         */
        int height() const { return m_height; }

        /** 
         * @brief Set a new X position to model.
         * 
         * @param x new X position.
         */
        void setX(int x) { m_x = x; }

        /** 
         * @brief Set a new Y position to model.
         * 
         * @param y new Y position.
         */
        void setY(int y) { m_y = y; }

        /** 
         * @brief Set a new width to model.
         * 
         * @param width new width.
         */
        void setWidth(int width) { m_width = width; }

        /** 
         * @brief Set a new height to model.
         * 
         * @param height new height.
         */
        void setHeight(int height) { m_height = height; }

        /** 
         * @brief Set a new position to model.
         * 
         * @param x new X position.
         * @param y new Y position.
         */
        void setPosition(int x, int y) { setX(x); setY(y); }

        /** 
         * @brief Set a new site to model.
         * 
         * @param width a new width.
         * @param height a new height.
         */
        void setSize(int width, int height)
        { setWidth(width); setHeight(height); }

    protected:
        bool parseXMLports(xmlpp::Element* modelNode);
        bool parseXMLport(xmlpp::Element* modelNode,
			  xmlpp::Element* portNode,
			  std::map < std::string, Port * > &portList);
        bool parseXMLspace(xmlpp::Element* modelNode,
			   xmlpp::Element* spaceNode);
        bool parseXMLtime(xmlpp::Element* modelNode,
			  xmlpp::Element* timeNode);

	std::string     m_name;
	std::string     m_description;
	CoupledModel*   m_parent;
	MapStringPort   m_initPortList;
	MapStringPort   m_inPortList;
	MapStringPort   m_outPortList;
	MapStringPort   m_statePortList;

        int             m_x;
        int             m_y;
        int             m_width;
        int             m_height;
    };

    typedef Model * PModel;

}} // namespace vle graph

#endif
