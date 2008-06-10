/**
 * @file src/vle/vpz/Model.hpp
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

#include <map>
#include <vle/vpz/Base.hpp>
#include <vle/graph/Model.hpp>

namespace vle { namespace vpz {

    typedef std::vector < std::string > StringVector;

    /** 
     * @brief The AtomicModel class is used by the AtomicModelList to attach an
     * atomic model to his conditions, observables and dynamics names.
     */
    class AtomicModel
    {
    public:
        AtomicModel(const std::string& conditions,
                    const std::string& dynamics,
                    const std::string& observables);

        inline const StringVector& conditions() const
        { return m_conditions; }

        inline const std::string& dynamics() const
        { return m_dynamics; }

        inline const std::string& observables() const
        { return m_observables; }

        inline void setConditions(const StringVector& vect)
        { m_conditions = vect; }

        inline void setDynamics(const std::string& str)
        { m_dynamics = str; }

        inline void setObservables(const std::string& str)
        { m_observables = str; }

        friend std::ostream& operator<<(std::ostream& out, const AtomicModel& a)
        {
            return out << "  conditions  : " << a.m_conditions.front() << "\n"
                       << "  dynamics    : " << a.m_dynamics << "\n"
                       << "  observables : " << a.m_observables << "\n";
        }

    private:
        AtomicModel() { }

	StringVector    m_conditions;
        std::string     m_dynamics;
        std::string     m_observables;
    };


    /** 
     * @brief The AtomicModelList class is a dictionary used to attach atomic
     * model to conditions and dynamics names.
     */
    class AtomicModelList : public std::map < graph::Model*, AtomicModel >
    {
    public:
        /** 
         * @brief Add a list of atomicmodels information.
         * @param atoms list of atomicmodels.
         * @throw utils::SaxParserError if an model already exist.
         */
        void add(const AtomicModelList& atoms);

        /** 
         * @brief Add a new atomicmodel information to a graph::Model.
         * @param mdl the graph::Model to attach atomic model information.
         * @param atom the vpz::AtomicModel information.
         * @return a reference to the builded atomicmodel.
         * @throw utils::SaxParserError if mdl already exist.
         */
        AtomicModel& add(graph::Model* mdl, const AtomicModel& atom);

        /** 
         * @brief Get an vpz::Model by his structural reference.
         * @param atom the reference to the structure.
         * @throw utils::SaxParserError if atom have no dynamics.
         * @return A constant reference to the vpz::Model.
         */
        const AtomicModel& get(graph::Model* atom) const;

        /** 
         * @brief Get an vpz::Model by his structural reference.
         * @param atom the reference to the structure.
         * @throw utils::SaxParserError if atom have no dynamics.
         * @return A constant reference to the vpz::Model.
         */
        AtomicModel& get(graph::Model* atom);

        /** 
         * @brief Get an vpz::Model by his structural reference.
         * @param atom the reference to the structure.
         * @throw utils::SaxParserError if atom have no dynamics.
         * @return A constant reference to the vpz::Model.
         */
        const AtomicModel& get(const graph::Model* atom) const;

        /** 
         * @brief Get an vpz::Model by his structural reference.
         * @param atom the reference to the structure.
         * @throw utils::SaxParserError if atom have no dynamics.
         * @return A constant reference to the vpz::Model.
         */
        AtomicModel& get(const graph::Model* atom);
        
        friend std::ostream& operator<<(std::ostream& out,
                                        const AtomicModelList& a)
        {
            out << "AtomicModelList:\n";
            for (const_iterator it = a.begin(); it != a.end(); ++it) {
                out << "Model [" << it->first << "] " << it->first->getName()
                    << "\n"
                    << it->second;
            }
            return out;
        }

    };


    class Model : public Base
    {
    public:
        Model() :
            m_graph(0)
        { }

        Model(const Model& mdl);

        virtual ~Model()
        { }

        /** 
         * @brief Write the devs hierachy of model.
         * 
         * @param elt a reference to the parent STRUCTURES tag.
         *
         * @throw Exception::Internal if elt is null.
         */
        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return MODEL; }

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
         * 
         * @param mdl the new graph::Model hierarchy to set.
         */
        void set_model(graph::Model* mdl);
        
        /** 
         * @brief Get a reference to the graph::Model hierarchy.
         * 
         * @return A reference to the graph::Model, be carreful, you can damage
         * graph::Vpz instance.
         */
        graph::Model* model();

        /** 
         * @brief Get a reference to the graph::Model hierarchy.
         * 
         * @return A reference to the graph::Model, be carreful, you can damage
         * graph::Vpz instance.
         */
        graph::Model* model() const;

        /** 
         * @brief Return a constant reference to the vpz::AtomicModelList.
         * 
         * @return 
         */
        const AtomicModelList& atomicModels() const
        { return m_atomicmodels; }

        /** 
         * @brief Return a reference to the vpz::AtomicModelList.
         * 
         * @return 
         */
        AtomicModelList& atomicModels()
        { return m_atomicmodels; }

    private:
        struct CopyAtomicModel
        {
            AtomicModelList& lst;
            graph::Model& top;

            CopyAtomicModel(AtomicModelList& lst, graph::Model& top) :
                lst(lst), top(top) { }

            void operator()(const AtomicModelList::value_type& x)
            {
                graph::CoupledModelVector vec;
                x.first->getParents(vec);
                graph::Model* atom = top.getModel(vec, x.first->getName());

                lst.insert(std::make_pair < graph::Model*, AtomicModel >(
                        atom, x.second));
            }
        };

        void writeModel(std::ostream& out) const;
        void writeAtomic(std::ostream& out,
                         const graph::AtomicModel* mdl) const;
        void writeCoupled(std::ostream& out,
                          const graph::CoupledModel* mdl) const;
        void writePort(std::ostream& out, const graph::Model* mdl) const;
        void writeConnection(std::ostream& out,
                             const graph::CoupledModel* mdl) const;
        void writeGraphics(std::ostream& out, const graph::Model* mdl) const;

        AtomicModelList     m_atomicmodels;
        graph::Model*       m_graph;
    };



    class Submodels : public Base
    {
    public:
        Submodels() { }

        virtual ~Submodels() { }

        virtual void write(std::ostream& /* out */) const
        { }

        virtual Base::type getType() const
        { return SUBMODELS; }
    };

    class Connections : public Base
    {
    public:
        Connections() { }

        virtual ~Connections() { }

        virtual void write(std::ostream& /* out */) const
        { }

        virtual Base::type getType() const
        { return CONNECTIONS; }
    };

    class InternalConnection : public Base
    {
    public:
        InternalConnection() { }

        virtual ~InternalConnection() { }

        virtual void write(std::ostream& /* out */) const
        { }

        virtual Base::type getType() const
        { return INTERNAL_CONNECTION; }
    };

    class InputConnection : public Base
    {
    public:
        InputConnection() { }

        virtual ~InputConnection() { }

        virtual void write(std::ostream& /* out */) const
        { }

        virtual Base::type getType() const
        { return INPUT_CONNECTION; }
    };

    class OutputConnection : public Base
    {
    public:
        OutputConnection() { }

        virtual ~OutputConnection() { }

        virtual void write(std::ostream& /* out */) const
        { }

        virtual Base::type getType() const
        { return OUTPUT_CONNECTION; }
    };

    class Origin: public Base
    {
    public:
        Origin(const Glib::ustring& model,
                    const Glib::ustring& port) :
            model(model),
            port(port)
        { }

        virtual ~Origin() { }

        virtual void write(std::ostream& /* out */) const
        { }

        virtual Base::type getType() const
        { return ORIGIN; }

        Glib::ustring model;
        Glib::ustring port;
    };

    class Destination: public Base
    {
    public:
        Destination(const Glib::ustring& model,
                    const Glib::ustring& port) :
            model(model),
            port(port)
        { }

        virtual ~Destination() { }

        virtual void write(std::ostream& /* out */) const
        { }

        virtual Base::type getType() const
        { return DESTINATION; }
        
        Glib::ustring model;
        Glib::ustring port;
    };

}} // namespace vle vpz

#endif
