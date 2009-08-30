/**
 * @file vle/vpz/Model.hpp
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


#ifndef VLE_VPZ_MODEL_HPP
#define VLE_VPZ_MODEL_HPP

#include <vle/vpz/DllDefines.hpp>
#include <vle/vpz/Base.hpp>
#include <vle/vpz/AtomicModels.hpp>
#include <vle/graph/Model.hpp>
#include <string>

namespace vle { namespace vpz {

    /**
     * @brief The vpz::Model is use to store the graph::Model hierarchy.
     */
    class VLE_VPZ_EXPORT Model : public Base
    {
    public:
        /**
         * @brief Build an empty Model.
         */
        Model() :
            m_graph(0)
        {}

        /**
         * @brief Copy constructor. The hierarchy of graph::Model is cloned.
         * @param mdl The model to copy.
         */
        Model(const Model& mdl);

        /**
         * @brief Nothing to delete. Be carefull, you must delete the graph.
         */
        virtual ~Model()
        {}

        /**
         * @brief Write the XML representation of this class.
         * @code
         * <structures>
         *  [...]
         * </structures>
         * @endcode
         * @param out an output stream.
         */
        virtual void write(std::ostream& out) const;

        /**
         * @brief Get the type of this class.
         * @return MODEL.
         */
        virtual Base::type getType() const
        { return MODEL; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Manage the graph.
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Just delete the complete list of graph::AtomicModelList and
         * set to NULL the graph::Model hierarchy. Don't forget to delete it
         * yourself.
         */
        void clear();

        /**
         * @brief Set a hierachy of graph::Model. If a previous hierarchy
         * already exist, it is not delete same if the new is empty. This
         * function is just an affectation, no clone is build.
         * @param mdl the new graph::Model hierarchy to set.
         */
        void setModel(graph::Model* mdl);

        /**
         * @brief Get a reference to the graph::Model hierarchy.
         * @return A reference to the graph::Model, be carreful, you can damage
         * graph::Vpz instance.
         */
        graph::Model* model();

        /**
         * @brief Get a reference to the graph::Model hierarchy.
         * @return A reference to the graph::Model, be carreful, you can damage
         * graph::Vpz instance.
         */
        graph::Model* model() const;

        /**
         * @brief Return a constant reference to the vpz::AtomicModelList.
         * @return a constant reference to the vpz::AtomicModelList.
         */
        const AtomicModelList& atomicModels() const
        { return m_atomicmodels; }

        /**
         * @brief Return a reference to the vpz::AtomicModelList.
         * @return a reference to the vpz::AtomicModelList.
         */
        AtomicModelList& atomicModels()
        { return m_atomicmodels; }

    private:
        AtomicModelList     m_atomicmodels;
        graph::Model*       m_graph;
    };

    /**
     * @brief The Submodels, only used by the SaxParser.
     */
    class VLE_VPZ_EXPORT Submodels : public Base
    {
    public:
        /**
         * @brief Build a Submodels.
         */
        Submodels()
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~Submodels()
        {}

        /**
         * @brief Nothing to output.
         */
        virtual void write(std::ostream& /* out */) const
        {}

        /**
         * @brief Get the type of this class.
         * @return SUBMODELS.
         */
        virtual Base::type getType() const
        { return SUBMODELS; }
    };

    /**
     * @brief The Connections, only used by the SaxParser.
     */
    class VLE_VPZ_EXPORT Connections : public Base
    {
    public:
        /**
         * @brief Build a Connections.
         */
        Connections()
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~Connections()
        {}

        /**
         * @brief Nothing to output.
         */
        virtual void write(std::ostream& /* out */) const
        {}

        /**
         * @brief Get the type of this class.
         * @return CONNECTIONS.
         */
        virtual Base::type getType() const
        { return CONNECTIONS; }
    };

    /**
     * @brief The InternalConnection, only used by the SaxParser.
     */
    class VLE_VPZ_EXPORT InternalConnection : public Base
    {
    public:
        /**
         * @brief Build an InternalConnection.
         */
        InternalConnection()
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~InternalConnection()
        {}

        /**
         * @brief Nothing to output.
         */
        virtual void write(std::ostream& /* out */) const
        {}

        /**
         * @brief Get the type of this class.
         * @return INTERNAL_CONNECTION.
         */
        virtual Base::type getType() const
        { return INTERNAL_CONNECTION; }
    };

    /**
     * @brief The InputConnection, only used by the SaxParser.
     */
    class VLE_VPZ_EXPORT InputConnection : public Base
    {
    public:
        /**
         * @brief Build an InputConnection.
         */
        InputConnection()
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~InputConnection()
        {}

        /**
         * @brief Nothing to output.
         */
        virtual void write(std::ostream& /* out */) const
        {}

        /**
         * @brief Get the type of this class.
         * @return INPUT_CONNECTION.
         */
        virtual Base::type getType() const
        { return INPUT_CONNECTION; }
    };

    /**
     * @brief The OutputConnection, only used by the SaxParser.
     */
    class VLE_VPZ_EXPORT OutputConnection : public Base
    {
    public:
        /**
         * @brief Build an OutputConnection.
         */
        OutputConnection()
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~OutputConnection()
        {}

        /**
         * @brief Nothing to output.
         */
        virtual void write(std::ostream& /* out */) const
        {}

        /**
         * @brief Get the type of this class.
         * @return OUTPUT_CONNECTION.
         */
        virtual Base::type getType() const
        { return OUTPUT_CONNECTION; }
    };

    /**
     * @brief The Origin, only used by the SaxParser.
     */
    class VLE_VPZ_EXPORT Origin: public Base
    {
    public:
        /**
         * @brief Build an Origin with specific model source.
         * @param model the name of the model source.
         * @param port the name of the port source.
         */
        Origin(const std::string& model,
               const std::string& port) :
            model(model),
            port(port)
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~Origin()
        {}

        /**
         * @brief Nothing to output.
         */
        virtual void write(std::ostream& /* out */) const
        {}

        /**
         * @brief Get the type of this class.
         * @return ORIGIN.
         */
        virtual Base::type getType() const
        { return ORIGIN; }

        std::string model; //!< the name of the model.
        std::string port; //!< the name of the In port.
    };

    /**
     * @brief The Destination, only used by the SaxParser.
     */
    class VLE_VPZ_EXPORT Destination: public Base
    {
    public:
        /**
         * @brief Build a Destination with specific model destination.
         * @param model the name of the destination model.
         * @param port the name of the destination port.
         */
        Destination(const std::string& model,
                    const std::string& port) :
            model(model),
            port(port)
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~Destination()
        {}

        /**
         * @brief Nothing to output.
         */
        virtual void write(std::ostream& /* out */) const
        {}

        /**
         * @brief Get the type of this class.
         * @return DESTINATION.
         */
        virtual Base::type getType() const
        { return DESTINATION; }

        std::string model; //!< the name of the model.
        std::string port; //!< the name of the Out port.
    };

}} // namespace vle vpz

#endif
