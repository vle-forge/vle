/**
 * @file src/vle/vpz/Class.hpp
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




#ifndef VLE_VPZ_CLASS_HPP
#define VLE_VPZ_CLASS_HPP

#include <vle/vpz/Base.hpp>
#include <vle/vpz/AtomicModels.hpp>


namespace vle { namespace vpz {

    /**
     * @brief The class Class is build on the vpz::Model structure. It store a
     * new graph::Model hierarchy and a vpz::AtomicModels class to build easily
     * new structure of models.
     */
    class Class : public Base
    {
    public:
        Class(const std::string& name) :
            m_name(name),
            m_model(0)
        { }

        Class(const Class& cls);

        virtual ~Class() { }

        /**
         * @brief Write the class xml representation into the output stream.
         * @code
         * <class name="xxxx">
         *  <structure>
         *   <model type="coupled">
         *    ...
         *   </model>
         *  </structure>
         * </class>
         * @endcode
         *
         * @param out
         */
        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return CLASS; }

        /**
         * @brief Get a constant reference to the stored Model.
         * @return Get a constant reference to the stored Model.
         */
        inline const graph::Model* model() const
        { return m_model; }

        /**
         * @brief Get a reference to the stored Model.
         * @return Get a reference to the stored Model.
         */
        inline graph::Model* model()
        { return m_model; }

        /**
         * @brief Set the current graph::model hierarchy by a new one. Be
         * carreful, the old one are not destroyed.
         * @param mdl The reference to the new model to set.
         */
        inline void setModel(graph::Model* mdl)
        { m_model = mdl; }

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

        /**
         * @brief Return a constant reference to the name of this Class.
         * @return A constant reference to the name of this Class.
         */
        const std::string& name() const
        { return m_name; }

    private:
        Class()
        { }

        AtomicModelList m_atomicmodels;
        std::string     m_name;
        graph::Model*   m_model;
    };

}} // namespace vle vpz

#endif
