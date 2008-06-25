/**
 * @file src/vle/vpz/AtomicModels.hpp
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




#ifndef VLE_VPZ_ATOMICMODELS_HPP
#define VLE_VPZ_ATOMICMODELS_HPP

#include <vle/graph/Model.hpp>
#include <string>
#include <vector>

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

    /**
     * @brief A functor to build an vpz::AtomicModelList from a graph::Model
     * hierarchy of models.
     * @code
     * AtomicModelList lst;
     * CoupledModel* mygraph;
     * ...
     * CoupledModel* newmygraph = mygraph->clone();
     *
     * std::for_each(lst.begin(), lst.end(), CopyAtomicModel(newlst,
     *               *newmygraph));
     * @endcode
     */
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


}} // namespace vle vpz

#endif

