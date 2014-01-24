/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
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

#ifndef VLE_GRAPH_ATOMIC_MODEL_HPP
#define VLE_GRAPH_ATOMIC_MODEL_HPP

#include <vle/vpz/BaseModel.hpp>
#include <vle/DllDefines.hpp>
#include <iterator>
#include <string>
#include <set>

namespace vle { namespace vpz {

    /**
     * @brief Represent the Atomic Model in DEVS formalism. This class just
     * represent the graph not the DEVS Simulator.
     */
    class VLE_API AtomicModel : public BaseModel
    {
    public:
        /**
         * @brief Constructor to intialize parent, position (0,0), size (0,0)
         * and name.
         * @param name the new name of this atomic model.
         * @param parent the parent of this atomic model, can be null if parent
         * does not exist.
         */
        AtomicModel(const std::string& name,
                         CoupledModel* parent);

        /**
         * @brief Build a new AtomicModel from specific conditions, dynamics and
         * observables, initialize parent position (0,0), size (0,0)
         * and name.
         * @param name the new name of this atomic model.
         * @param parent the parent of this atomic model, can be null if parent
         * @param condition The condition to attach.
         * @param dynamic The dynamics to attach.
         * @param observable The observable to attach.
         */
        AtomicModel(const std::string& name,
                        CoupledModel* parent,
                        const std::string& condition,
                        const std::string& dynamic,
                        const std::string& observable);

        AtomicModel(const AtomicModel& mdl);

        AtomicModel& operator=(const AtomicModel& mdl);

        virtual vpz::BaseModel* clone() const
        { return new AtomicModel(*this); }

        /**
         * @brief Nothing to delete.
         */
	virtual ~AtomicModel() { }

        /**
         * @brief Return true, AtomicModel is an atomic model.
         * @return true.
         */
        virtual bool isAtomic() const { return true; }

        /**
         * @brief Get the list of conditions.
         * @return List of conditions.
         */
        inline const std::vector < std::string >& conditions() const
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
        inline void setConditions(const std::vector < std::string >& vect)
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
         * @brief Return this if name is equal to the model's name.
         * @param name The name of the model to find.
         * @return this if name is equal to the model's name, null otherwise.
         * @deprecated
         */
        virtual BaseModel* findModel(const std::string & name) const;

        /**
         * @brief Write the atomic model in the output stream.
         * @param out output stream.
         */
	virtual void writeXML(std::ostream& out) const;

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

        /**
         * @brief Write the model into the output stream.
         * @param out The output stream.
         */
        void write(std::ostream& out) const;

        /**
	 * @brief Update the dynamics the AtomicModel
	 * where an oldname became newname.
	 * @param oldname the old name of the dynamics.
	 * @param newname the new name of the dynamics.
	 */
        virtual void updateDynamics(const std::string& oldname,
                            const std::string& newname);

        /**
	 * @brief purge the dymamics not present in the list.
	 * @param dynamicslist a list of dynamics name
	 */
	virtual void purgeDynamics(const std::set < std::string >& dynamicslist);

        /**
	 * @brief Update the Observable of the AtomicModel where an
	 * oldname became newname, for each model.
	 * @param oldname the old name of the observable.
	 * @param newname the new name of the observable.
	 */
	virtual void updateObservable(const std::string& oldname,
                                      const std::string& newname);

        /**
	 * @brief purge the observable reference of the AtomicModel
	 * where the observable is not present in the list.
	 * @param observablelist a list of observable name
	 */
	virtual void purgeObservable(const std::set < std::string >& observablelist);

        /**
	 * @brief Update the Conditions of the AtomicModel where an
	 * oldname became newname, for the model.
	 * @param oldname the old name of the observable.
	 * @param newname the new name of the observable.
	 */
	virtual void updateConditions(const std::string& oldname,
                                      const std::string& newname);

        /**
	 * @brief purge the Conditions references of the model where the
	 * Condition is not present in the list
	 * @param conditionlist a list of condition name
	 */
	virtual void purgeConditions(const std::set < std::string >& conditionlist);

    private:
        //AtomicModel() {}

	std::vector < std::string > m_conditions;
        std::string m_dynamics;
        std::string m_observables;
    };

}} // namespace vle vpz

#endif
