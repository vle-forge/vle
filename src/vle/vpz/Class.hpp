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


#ifndef VLE_VPZ_CLASS_HPP
#define VLE_VPZ_CLASS_HPP

#include <vle/vpz/Base.hpp>
#include <vle/DllDefines.hpp>
#include <vector>
#include <set>

namespace vle { namespace vpz {

    class BaseModel;
    class AtomicModel;

    /**
     * @brief The class Class is build on the vpz::Model structure. It stores a
     * new vpz::Model hierarchy and a vpz::AtomicModels class to build easily
     * new structure of models.
     */
    class VLE_API Class : public Base
    {
    public:
        /**
         * @brief Build a new Class with a name but without vpz::Model
         * hierarchy.
         * @param name The name of this Class.
         */
        Class(const std::string& name) :
            m_name(name),
            m_model(0)
        {}

        /**
         * @brief Build a new Class by copying the parameter. The vpz::Model
         * hierarchy is clone in this function.
         * @param cls The class to copy.
         */
        Class(const Class& cls);

        /**
         * @brief Delete the vpz::Model hierarchy.
         */
        virtual ~Class();

        /**
         * @brief Write the class XML representation into the output stream.
         * @code
         * <class name="xxxx">
         *  <model type="coupled">
         *    ...
         *  </model>
         * </class>
         * @endcode
         * @param out The output stream.
         */
        virtual void write(std::ostream& out) const;

        /**
         * @brief Get the type of this class.
         * @return CLASS.
         */
        virtual Base::type getType() const
        { return VLE_VPZ_CLASS; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Manage the vpz::Model hierarchy
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Get a constant reference to the stored Model.
         * @return Get a constant reference to the stored Model.
         */
        inline const BaseModel* model() const
        { return m_model; }

        /**
         * @brief Get a reference to the stored Model.
         * @return Get a reference to the stored Model.
         */
        inline BaseModel* model()
        { return m_model; }

        /**
         * @brief Set the current graph::model hierarchy by a new one. Be
         * carreful, the old one are not destroyed.
         * @param mdl The reference to the new model to set.
         */
        inline void setModel(BaseModel* mdl)
        { m_model = mdl; }


        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Get/Set functons.
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Return a constant reference to the name of this Class.
         * @return A constant reference to the name of this Class.
         */
        const std::string& name() const
        { return m_name; }

        /**
         * @brief Set a new name to this Class.
         * @param newname The new name.
         */
        void setName(const std::string& newname)
        { m_name.assign(newname); }

        /**
	 * @brief Update the dynamics of the AtomicModels where an
	 * oldname became newname, for the class graph model.
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
	void purgeDynamics(const std::set < std::string >& dynamicslist);

        /**
	 * @brief Update the Observable of the AtomicModels where an
	 * oldname became newname, for the graph model.
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
	void purgeObservable(const std::set < std::string >& observablelist);

        /**
	 * @brief Update the Conditions of the AtomicModels where an
	 * oldname became newname, for the class graph model.
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
	void purgeConditions(const std::set < std::string >& conditionlist);

        void getAtomicModelList(std::vector < AtomicModel* >& list) const;

    private:
        std::string     m_name;
        BaseModel*      m_model;
    };

}} // namespace vle vpz

#endif
