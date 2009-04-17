/**
 * @file vle/vpz/AtomicModels.hpp
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
#include <iterator>
#include <string>
#include <list>

namespace vle { namespace vpz {

    /**
     * @brief Define a list of string for the list of conditions.
     */
    typedef std::list < std::string > Strings;

    /**
     * @brief The AtomicModel class is used by the AtomicModelList to attach an
     * atomic model to his conditions, observables and dynamics names.
     */
    class AtomicModel
    {
    public:
        /**
         * @brief Build a new AtomicModel from specific conditions, dynamics and
         * observables.
         * @param condition The condition to attach.
         * @param dynamic The dynamics to attach.
         * @param observable The observable to attach.
         */
        AtomicModel(const std::string& condition,
                    const std::string& dynamic,
                    const std::string& observable);

        /**
         * @brief Get the list of conditions.
         * @return List of conditions.
         */
        inline const Strings& conditions() const
        { return m_conditions; }

        /**
         * @brief Get the dynamic.
         * @return The dynamic.
         */
        inline const std::string& dynamics() const
        { return m_dynamics; }

        /**
         * @brief Get the observable.
         * @return The observable.
         */
        inline const std::string& observables() const
        { return m_observables; }

        /**
         * @brief Assign a list of condition.
         * @param vect A list of condition.
         */
        inline void setConditions(const Strings& vect)
        { m_conditions = vect; }

        /**
         * @brief Add a new condition.
         * @param str The new condition to add.
         */
        inline void addCondition(const std::string& str)
        { m_conditions.push_back(str); }

	/**
	 * @brief Del a condition.
	 * @param str The condition to delete.
	 */
	void delCondition(const std::string& str);

        /**
         * @brief Assign the dynamic.
         * @param str The dynamic.
         */
        inline void setDynamics(const std::string& str)
        { m_dynamics = str; }

        /**
         * @brief Assign an observable.
         * @param str The observable.
         */
        inline void setObservables(const std::string& str)
        { m_observables = str; }

        /**
         * @brief Output the AtomicModel informations into a std::ostream.
         * @param out Output paramter.
         * @param a An AtomicModel.
         * @return the out parameter.
         */
        friend std::ostream& operator<<(std::ostream& out, const AtomicModel& a)
        {
            out << "conditions: ";
            std::copy(a.m_conditions.begin(), a.m_conditions.end(),
                      std::ostream_iterator < std::string >(out, " "));
            return out << "\ndynamics: " << a.m_dynamics << "\nobservables: "
                << a.m_observables << "\n";
        }

    private:
        AtomicModel() {}

	Strings     m_conditions;
        std::string m_dynamics;
        std::string m_observables;
    };

    /**
     * @brief Define a dictionary between a graph::Model* from the hierarchy of
     * models or from the classes.
     */
    typedef std::map < graph::Model*, AtomicModel > AtomicModels;

    /**
     * @brief The AtomicModelList class is a dictionary used to attach atomic
     * model to conditions and dynamics names.
     */
    class AtomicModelList
    {
    public:
        /**
         * @brief A iterator for the atomicmodels.
         */
        typedef AtomicModels::iterator iterator;

        /**
         * @brief A constant iterator for the AtomicModels.
         */
        typedef AtomicModels::const_iterator const_iterator;

        /**
         * @brief Add a list of atomicmodel information.
         * @param atoms list of atomicmodel.
         * @throw utils::ArgError if an model already exist.
         */
        void add(const AtomicModelList& atoms);

        /**
         * @brief Add a new atomicmodel information to a graph::Model.
         * @param mdl the graph::Model to attach atomic model information.
         * @param atom the vpz::AtomicModel information.
         * @return a reference to the builded atomicmodel.
         * @throw utils::ArgError if mdl already exist.
         */
        AtomicModel& add(graph::Model* mdl, const AtomicModel& atom);

        /**
         * @brief Delete the specified AtomicModel.
         * @param mdl The graph::Model to find and delete.
         * @throw utils::ArgError if mdl already exist.
         */
        void del(graph::Model* mdl);

        /**
         * @brief Get an vpz::Model by his structural reference.
         * @param atom the reference to the structure.
         * @throw utils::ArgError if atom have no dynamics.
         * @return A constant reference to the vpz::Model.
         */
        const AtomicModel& get(graph::Model* atom) const;

        /**
         * @brief Get an vpz::Model by his structural reference.
         * @param atom the reference to the structure.
         * @throw utils::ArgError if atom have no dynamics.
         * @return A constant reference to the vpz::Model.
         */
        AtomicModel& get(graph::Model* atom);

        /**
         * @brief Get an vpz::Model by his structural reference.
         * @param atom the reference to the structure.
         * @throw utils::ArgError if atom have no dynamics.
         * @return A constant reference to the vpz::Model.
         */
        const AtomicModel& get(const graph::Model* atom) const;

        /**
         * @brief Get an vpz::Model by his structural reference.
         * @param atom the reference to the structure.
         * @throw utils::ArgError if atom have no dynamics.
         * @return A constant reference to the vpz::Model.
         */
        AtomicModel& get(const graph::Model* atom);

	/**
	 * @brief Update the conditions list of each AtomicModel
	 * where an oldname became newname.
	 * @param oldname the old name of the condition.
	 * @param newname the new name of the condition.
	 */
	void updateCondition(const std::string oldname,
			     const std::string newname);

        /**
         * @brief Check if the graph::AtomicModel exist into the
         * AtomicModelList.
         * @param atom The graph::AtomicModel to check.
         * @return true if found, false otherwise.
         */
        bool exist(graph::Model* atom) const
        { return m_lst.find(atom) != m_lst.end(); }

        /**
         * @brief Remove all reference into the AtomicModels.
         */
        void clear()
        { m_lst.clear(); }

        /**
         * @brief Output the AtomicModelList into an output stream.
         * @param out The output stream.
         * @param a An AtomicModelList.
         * @return The output stream.
         */
        friend std::ostream& operator<<(std::ostream& out,
                                        const AtomicModelList& a)
        {
            out << "AtomicModelList:";
            for (const_iterator it = a.m_lst.begin(); it != a.m_lst. end();
                 ++it) {
                out << "Model [" << it->first << "] " << it->first->getName()
                    << "\n" << it->second;
            }
            return out;
        }

        /**
         * @brief Get the AtomicModels.
         * @return A constant reference to the AtomicModels.
         */
        const AtomicModels& atomicmodels() const
        { return m_lst; }

        /**
         * @brief Get the begin of the AtomicModels.
         * @return A reference to the begin of the AtomicModels.
         */
        iterator begin()
        { return m_lst.begin(); }

        /**
         * @brief Get the begin of the AtomicModels.
         * @return A constant reference to the begin of the AtomicModels.
         */
        const_iterator begin() const
        { return m_lst.begin(); }

        /**
         * @brief Get the end of the AtomicModels.
         * @return A constant reference to the end of the AtomicModels.
         */
        iterator end()
        { return m_lst.end(); }

        /**
         * @brief Get the end of the AtomicModels.
         * @return A reference to the end of the AtomicModels.
         */
        const_iterator end() const
        { return m_lst.end(); }

        /**
         * @brief Write the model or hierarchy of models into the output stream.
         * @param mdl The top node of the hierarchy or a single atomic model.
         * @param out The output stream.
         */
        void writeModel(const graph::Model* mdl, std::ostream& out) const;

    private:
        AtomicModels m_lst;

        void writeAtomic(std::ostream& out,
                         const graph::AtomicModel* mdl) const;
        void writeCoupled(std::ostream& out,
                          const graph::CoupledModel* mdl) const;
        void writePort(std::ostream& out, const graph::Model* mdl) const;
        void writeConnection(std::ostream& out,
                             const graph::CoupledModel* mdl) const;
        void writeGraphics(std::ostream& out, const graph::Model* mdl) const;
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
        /**
         * @brief The constructor.
         * @param lst The AtomicModelList to add new models.
         * @param top the hierarchy of models.
         */
        CopyAtomicModel(AtomicModelList& lst, graph::Model& top) :
            lst(lst), top(top)
        {}

        /**
         * @brief Copy the AtomicModels into the AtomicModelList.
         * @param x the AtomicModel to add.
         */
        void operator()(const AtomicModels::value_type& x)
        {
            graph::CoupledModelVector vec;
            x.first->getParents(vec);
            graph::Model* atom = top.getModel(vec, x.first->getName());

            lst.add(atom, x.second);
        }

        AtomicModelList& lst; //!< the output of this functor.
        graph::Model& top; //!< the hierarchy of graph::Model.
    };

}} // namespace vle vpz

#endif

