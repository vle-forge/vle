/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#ifndef VLE_VPZ_MODEL_HPP
#define VLE_VPZ_MODEL_HPP

#include <memory>
#include <set>
#include <string>
#include <vector>
#include <vle/DllDefines.hpp>
#include <vle/vpz/Base.hpp>

namespace vle {
namespace vpz {

class AtomicModel;
class BaseModel;

/**
 * @brief The vpz::Model is use to store the Model hierarchy.
 */
class VLE_API Model : public Base
{
public:
    /**
     * @brief Build an empty Model.
     */
    Model();

    /**
     * @brief Copy constructor. The hierarchy of Model is cloned.
     * @param mdl The model to copy.
     */
    Model(const Model& mdl);

    Model& operator=(const Model& mdl) = delete;

    /**
     * @brief Nothing to delete. Be carefull, you must delete the graph.
     */
    virtual ~Model();

    /**
     * @brief Write the XML representation of this class.
     * @code
     * <structures>
     *  [...]
     * </structures>
     * @endcode
     * @param out an output stream.
     */
    virtual void write(std::ostream& out) const override;

    /**
     * @brief Get the type of this class.
     * @return MODEL.
     */
    virtual Base::type getType() const override
    {
        return VLE_VPZ_MODEL;
    }

    /**
     * @brief Just delete the complete list of graph::AtomicModelList and
     * set to NULL the Model hierarchy. Don't forget to delete it
     * yourself.
     */
    void clear();

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage the graph if it uses in the Vpz.Project.Model
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    void setGraph(std::unique_ptr<BaseModel> graph);

    std::unique_ptr<BaseModel> graph();

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage the node if it is not used in the Vpz.Project.Model
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Set a hierachy of Model. If a previous hierarchy
     * already exist, it is not delete same if the new is empty. This
     * function is just an affectation, no clone is build.
     * @param mdl the new Model hierarchy to set.
     */
    void setNode(BaseModel* mdl);

    /**
     * @brief Get a reference to the Model hierarchy.
     * @return A reference to the Model, be carreful, you can damage
     * graph::Vpz instance.
     */
    BaseModel* node();

    /**
     * @brief Get a reference to the Model hierarchy.
     * @return A reference to the Model, be carreful, you can damage
     * graph::Vpz instance.
     */
    BaseModel* node() const;

    /**
     * @brief Update the dynamics of the AtomicModels where an
     * oldname became newname, for the main graph model.
     * @param oldname the old name of the dynamics.
     * @param newname the new name of the dynamics.
     */
    void updateDynamics(const std::string& oldname,
                        const std::string& newname);

    /**
     * @brief purge the dymamics references of the model where the
     * dynamic is not present in the list
     * @param dynamicslist a list of dynamics name
     */
    void purgeDynamics(const std::set<std::string>& dynamicslist);

    /**
     * @brief Update the Observable of the AtomicModels where an
     * oldname became newname, for the main graph model.
     * @param oldname the old name of the observable.
     * @param newname the new name of the observable.
     */
    void updateObservable(const std::string& oldname,
                          const std::string& newname);

    /**
     * @brief purge the observables references of the model where the
     * observable is not present in the list
     * @param observablelist a list of observable name
     */
    void purgeObservable(const std::set<std::string>& observablelist);

    /**
     * @brief Update the Conditions of the AtomicModels where an
     * oldname became newname, for the main graph model.
     * @param oldname the old name of the observable.
     * @param newname the new name of the observable.
     */
    void updateConditions(const std::string& oldname,
                          const std::string& newname);

    /**
     * @brief purge the Conditions references of the model where the
     * Condition is not present in the list
     * @param conditionlist a list of condition name
     */
    void purgeConditions(const std::set<std::string>& conditionlist);

    void getAtomicModelList(std::vector<AtomicModel*>& list) const;

private:
    std::unique_ptr<BaseModel> m_graph;
    BaseModel* m_node;
};

/**
 * @brief The Submodels, only used by the SaxParser.
 */
class VLE_API Submodels : public Base
{
public:
    /**
     * @brief Build a Submodels.
     */
    Submodels()
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~Submodels()
    {
    }

    /**
     * @brief Nothing to output.
     */
    virtual void write(std::ostream& /* out */) const override
    {
    }

    /**
     * @brief Get the type of this class.
     * @return SUBMODELS.
     */
    virtual Base::type getType() const override
    {
        return VLE_VPZ_SUBMODELS;
    }
};

/**
 * @brief The Connections, only used by the SaxParser.
 */
class VLE_API Connections : public Base
{
public:
    /**
     * @brief Build a Connections.
     */
    Connections()
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~Connections()
    {
    }

    /**
     * @brief Nothing to output.
     */
    virtual void write(std::ostream& /* out */) const override
    {
    }

    /**
     * @brief Get the type of this class.
     * @return CONNECTIONS.
     */
    virtual Base::type getType() const override
    {
        return VLE_VPZ_CONNECTIONS;
    }
};

/**
 * @brief The InternalConnection, only used by the SaxParser.
 */
class VLE_API InternalConnection : public Base
{
public:
    /**
     * @brief Build an InternalConnection.
     */
    InternalConnection()
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~InternalConnection()
    {
    }

    /**
     * @brief Nothing to output.
     */
    virtual void write(std::ostream& /* out */) const override
    {
    }

    /**
     * @brief Get the type of this class.
     * @return INTERNAL_CONNECTION.
     */
    virtual Base::type getType() const override
    {
        return VLE_VPZ_INTERNAL_CONNECTION;
    }
};

/**
 * @brief The InputConnection, only used by the SaxParser.
 */
class VLE_API InputConnection : public Base
{
public:
    /**
     * @brief Build an InputConnection.
     */
    InputConnection()
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~InputConnection()
    {
    }

    /**
     * @brief Nothing to output.
     */
    virtual void write(std::ostream& /* out */) const override
    {
    }

    /**
     * @brief Get the type of this class.
     * @return INPUT_CONNECTION.
     */
    virtual Base::type getType() const override
    {
        return VLE_VPZ_INPUT_CONNECTION;
    }
};

/**
 * @brief The OutputConnection, only used by the SaxParser.
 */
class VLE_API OutputConnection : public Base
{
public:
    /**
     * @brief Build an OutputConnection.
     */
    OutputConnection()
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~OutputConnection()
    {
    }

    /**
     * @brief Nothing to output.
     */
    virtual void write(std::ostream& /* out */) const override
    {
    }

    /**
     * @brief Get the type of this class.
     * @return OUTPUT_CONNECTION.
     */
    virtual Base::type getType() const override
    {
        return VLE_VPZ_OUTPUT_CONNECTION;
    }
};

/**
 * @brief The Origin, only used by the SaxParser.
 */
class VLE_API Origin : public Base
{
public:
    /**
     * @brief Build an Origin with specific model source.
     * @param model the name of the model source.
     * @param port the name of the port source.
     */
    Origin(const std::string& model, const std::string& port)
      : model(model)
      , port(port)
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~Origin()
    {
    }

    /**
     * @brief Nothing to output.
     */
    virtual void write(std::ostream& /* out */) const override
    {
    }

    /**
     * @brief Get the type of this class.
     * @return ORIGIN.
     */
    virtual Base::type getType() const override
    {
        return VLE_VPZ_ORIGIN;
    }

    std::string model; //!< the name of the model.
    std::string port;  //!< the name of the In port.
};

/**
 * @brief The Destination, only used by the SaxParser.
 */
class VLE_API Destination : public Base
{
public:
    /**
     * @brief Build a Destination with specific model destination.
     * @param model the name of the destination model.
     * @param port the name of the destination port.
     */
    Destination(const std::string& model, const std::string& port)
      : model(model)
      , port(port)
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~Destination()
    {
    }

    /**
     * @brief Nothing to output.
     */
    virtual void write(std::ostream& /* out */) const override
    {
    }

    /**
     * @brief Get the type of this class.
     * @return DESTINATION.
     */
    virtual Base::type getType() const override
    {
        return VLE_VPZ_DESTINATION;
    }

    std::string model; //!< the name of the model.
    std::string port;  //!< the name of the Out port.
};
}
} // namespace vle vpz

#endif
