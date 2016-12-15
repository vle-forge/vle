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

#ifndef VLE_VPZ_SAXSTACKVPZ_HPP
#define VLE_VPZ_SAXSTACKVPZ_HPP

#include <libxml/xmlstring.h>
#include <list>
#include <vle/DllDefines.hpp>
#include <vle/value/Set.hpp>

namespace vle {

namespace vpz {

class BaseModel;
class Base;
class Vpz;
class Class;

/**
 * @brief The SaxStackVpz is used to construct the vpz::Base hierarchy.
 */
class VLE_LOCAL SaxStackVpz {
public:
    /**
     * @brief Build a new SaxStackVpz which fill the vpz::Vpz parameter.
     * @param vpz The vpz::Vpz to fill.
     */
    SaxStackVpz(Vpz &vpz)
        : m_vpz(vpz)
    {
    }

    /**
     * @brief Nothing to delete.
     */
    ~SaxStackVpz();

    /**
     * @brief Clear all element of the stack.
     */
    void clear();

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage the stack.
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Push a Project tag to the stack.
     * @param name The attribute list.
     * @return A reference to the vpz::Vpz.
     * @throw utils::SaxParserError if stack is not empty.
     */
    vpz::Vpz *pushVpz(const xmlChar **name);

    /**
     * @brief Push the structure into the stack.
     * @throw utils::SaxParserError if stack is empty and parent is not vpz.
     */
    void pushStructure();

    /**
     * @brief Push a mode into the stack.
     * @param att The attribute list.
     * @throw utils::SaxParserError if stack is empty and parent it not
     * Structures, Submodels or Class.
     */
    void pushModel(const xmlChar **att);

    /**
     * @brief Push Port (condition or observation) into the stack.
     * @param att The attribute list.
     * @throw utils::SaxParserError if stack is empty and parent it not
     * Conditions, Observables or Connection (In or Out).
     */
    void pushPort(const xmlChar **att);

    /**
     * @brief Push the type of port.
     * @param name The attribute list.
     * @throw utils::SaxParserError if stack is empty or parent is not a
     * model.
     */
    void pushPortType(const char *name);

    /**
     * @brief Push a Submodels into the stack.
     * @throw utils::SaxParserError if stack is empty or parent is not a
     * model.
     */
    void pushSubModels();

    /**
     * @brief Push a Connections into the stack.
     * @throw utils::SaxParserError if stack is empty or parent is not a
     * Model.
     */
    void pushConnections();

    /**
     * @brief Push a Connection (InternalConnection, InputConnection or
     * OutputConnection) into the stack.
     * @param att The attribute list.
     * @throw utils::SaxParserError if stack is empty or parent is not
     * Connections.
     */
    void pushConnection(const xmlChar **att);

    /**
     * @brief Push an Origin of Connection into the stack.
     * @param att The attribute list.
     * @throw utils::SaxParserError if stack is empty or parent is not
     * InternalConnection, InputConnection or OutputConnection.
     */
    void pushOrigin(const xmlChar **att);

    /**
     * @brief Push an Destination of Connection into the stack.
     * @param att The attribute list.
     * @throw utils::SaxParserError if stack is empty or parent is not
     * InternalConnection, InputConnection or OutputConnection.
     */
    void pushDestination(const xmlChar **att);

    /**
     * @brief Push a Dynamics into the stack.
     * @throw utils::SaxParserError if stack is empty or parent is not Vpz.
     */
    void pushDynamics();

    /**
     * @brief Push a Dynamic into the stack.
     * @param att The attribute list.
     * @throw utils::SaxParserError if stack is empty or parent is not a
     * Dynamics.
     */
    void pushDynamic(const xmlChar **att);

    /**
     * @brief Push an Experiment into the stack.
     * @param att The attribute list.
     * @throw utils::SaxParserError if stack is empty or parent is not Vpz.
     */
    void pushExperiment(const xmlChar **att);

    /**
     * @brief Push a Conditions into th stack.
     * @throw utils::SaxParserError if stack is empty or parent is not
     * Experiment.
     */
    void pushConditions();

    /**
     * @brief Push a Condition into the stack.
     * @param att The attribute list.
     * @throw utils::SaxParserError if stack is empty or parent is not
     * Conditions.
     */
    void pushCondition(const xmlChar **att);

    /**
     * @brief Push a ConditionPort into a Condition.
     * @param att The attribute list.
     * @throw utils::SaxParserError if stack is empty or parent is not
     * Condition.
     */
    void pushConditionPort(const xmlChar **att);

    /**
     * @brief Push a Views into th stack.
     * @throw utils::SaxParserError if stack is empty or parent is not
     * Experiment.
     */
    void pushViews();

    /**
     * @brief Push an Outputs into th stack.
     * @throw utils::SaxParserError if stack is empty or parent is not
     * Views.
     */
    void pushOutputs();

    /**
     * @brief Push an Output into th stack.
     * @param att The attribute list.
     * @throw utils::SaxParserError if stack is empty or parent is not
     * Outputs.
     */
    void pushOutput(const xmlChar **att);

    /**
     * @brief Push an View into th stack.
     * @param att The attribute list.
     * @throw utils::SaxParserError if stack is empty or parent is not
     * Views.
     */
    void pushView(const xmlChar **att);

    /**
     * @brief Push an attachedview into th stack.
     * @param att The attribute list.
     * @throw utils::SaxParserError if stack is empty or parent is not
     * ObservablePort.
     */
    void pushAttachedView(const xmlChar **att);

    /**
     * @brief Push an Observables into th stack.
     * @throw utils::SaxParserError if stack is empty or parent is not
     * Views.
     */
    void pushObservables();

    /**
     * @brief Push an Observable into th stack.
     * @param att The attribute list.
     * @throw utils::SaxParserError if stack is empty or parent is not
     * Observables.
     */
    void pushObservable(const xmlChar **att);

    /**
     * @brief Push an ObservablePort into th stack.
     * @param att The attribute list.
     * @throw utils::SaxParserError if stack is empty or parent is not
     * Observable.
     */
    void pushObservablePort(const xmlChar **att);

    /**
     * @brief Push a View to an ObservablePort.
     * @param att The attribute list.
     * @throw utils::SaxParserError if stack is empty or parent is not
     * ObservablePort.
     */
    void pushObservablePortOnView(const xmlChar **att);

    /**
     * @brief Push a Classes into the stack.
     * @throw utils::SaxParserError if stack is empty or parent is not
     * Vpz.
     */
    void pushClasses();

    /**
     * @brief Push a Class into the stack.
     * @param att The attribute list.
     * @throw utils::SaxParserError if stack is empty or parent is not
     * Classes.
     */
    void pushClass(const xmlChar **att);

    /**
     * @brief Pop the Classes
     * @throw utils::SaxParserError if stack is empty or parent is not
     * Classes.
     */
    void popClasses();

    /**
     * @brief Pop the Class.
     * @throw utils::SaxParserError if stack is empty or parent is not
     * Class.
     */
    void popClass();

    /**
     * @brief Pop the ConditionPort.
     * @return The value::Set read.
     */
    std::vector<std::shared_ptr<value::Value>> &popConditionPort();

    /**
     * @brief Pop the Output.
     * @return The value::Value read.
     */
    void popOutput();

    /**
     * @brief Pop the top of the stack and return it.
     * @throw utils::SaxParserError if stack is empty.
     * @return A pointer to the vpz::Base class.
     */
    vpz::Base *pop();

    /**
     * @brief Build the connection into the graph::Model from the three
     * elements at top of the stack: a Destination, a Origin, Connection
     * (InternalConnection, InputConnection, OutputConnection).
     * @throw utils::SaxParserError if stack is empty or the format
     * (Destination, Origin, Connection) is bad.
     */
    void buildConnection();

    /**
     * @brief Get the top of the stack.
     * @return A constant pointer to the stack.
     * @throw utils::SaxParserError if stack is empty.
     */
    const vpz::Base *top() const;

    /**
     * @brief Get the top of the stack.
     * @return A pointer to the stack.
     * @throw utils::SaxParserError if stack is empty.
     */
    vpz::Base *top();

    /**
     * @brief Get a reference to the Vpz.
     * @return A reference to the Vpz.
     */
    inline vpz::Vpz &vpz() { return m_vpz; }

    /**
     * @brief Output stream operator.
     * @param out The output stream.
     * @param stack The stack to show.
     * @return The output stream.
     */
    friend std::ostream &operator<<(std::ostream &out,
                                    const SaxStackVpz &stack);

private:
    inline void push(vpz::Base *base) { m_stack.push_front(base); }

    inline bool haveParent() { return not m_stack.empty(); }

    inline bool haveGrandParent() { return m_stack.size() > 1; }

    inline vpz::Base *parent() { return m_stack.front(); }

    inline vpz::Base *grandparent() { return *m_stack.begin()++; }

    inline const vpz::Base *parent() const { return m_stack.front(); }

    inline const vpz::Base *grandparent() const { return *m_stack.begin()++; }

    inline bool empty() const { return m_stack.empty(); }

    std::list<vpz::Base *> m_stack;
    vpz::Vpz &m_vpz;

    /**
     * @brief Assign the to graph::Model mdl the graphics information from
     * the string x, y, width and height taken from the Vpz stream.
     * @param mdl The graph::Model where assign (x, y) an or (width,
     * height).
     * @param x
     * @param y
     * @param width
     * @param height
     */
    void buildModelGraphics(vpz::BaseModel *mdl,
                            const std::string &x,
                            const std::string &y,
                            const std::string &width,
                            const std::string &height);

    /**
     * @brief Get the latest vpz::Class pushed in the list from the top of
     * the stack.
     * @return A reference to the class founded or 0 if no class in the
     * stack.
     */
    vpz::Class *getLastClass() const;

private:
    void checkParentIsVpz() const;
    void checkParentIsClass() const;
    void checkParentIsSubmodels() const;
    void checkParentIsStructures() const;
    void checkParentOfModel() const;
};
}
} // namespace vle vpz

#endif
