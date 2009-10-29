/**
 * @file vle/vpz/Dynamics.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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


#ifndef VLE_VPZ_DYNAMICS_HPP
#define VLE_VPZ_DYNAMICS_HPP

#include <vle/vpz/DllDefines.hpp>
#include <vle/vpz/Dynamic.hpp>
#include <map>

namespace vle { namespace vpz {

    /**
     * @brief Define a list of Dynamcis.
     */
    typedef std::map < std::string, Dynamic > DynamicList;

    /**
     * @brief The vpz::Dynamics class wrap the DynamicList and provides
     * functions to access it.
     */
    class VLE_VPZ_EXPORT Dynamics : public Base
    {
    public:
        /**
         * @brief A iterator to DynamicList.
         */
        typedef DynamicList::iterator iterator;

        /**
         * @brief A constant iterator to DynamicList.
         */
        typedef DynamicList::const_iterator const_iterator;

        /**
         * @brief Build a new Dynamics.
         */
        Dynamics()
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~Dynamics()
        {}

        /**
         * @brief Write the Dynamics information into stream
         * @code
         * <dynamics>
         *  [...]
         * </dynamics>
         * @endcode
         * @param out the output stream.
         */
        virtual void write(std::ostream& out) const;

        /**
         * @brief Get the type of this class.
         * @return DYNAMICS.
         */
        virtual Base::type getType() const
        { return DYNAMICS; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Manage the DynamicList
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Add a list of Dynamics to the list.
         * @param dyns A Dynamics objet to add.
         * @throw utils::ArgError if a Dynamic already exist.
         */
        void add(const Dynamics& dyns);

        /**
         * @brief Add a Dynamic to the list.
         * @param dyn the Dynamic to add.
         * @return a reference to the newly created dynamics.
         * @throw utils::ArgError if a dynamic with the same model name already
         * exist.
         */
        Dynamic& add(const Dynamic& dyn);

        /**
         * @brief Delete the dynamic for a specific model name.
         * @param name the name of the model.
         */
        void del(const std::string& name);

        /**
         * @brief Remove all Dynamic from the DynamicList.
         */
        inline void clear()
        { m_list.clear(); }

        /**
         * @brief Search a Dynamic with the specified name.
         * @param name Dynamic name to find.
         * @return A constant reference to the Dynamic find.
         * @throw utils::ArgError if no Dynamic find.
         */
        const Dynamic& get(const std::string& name) const;

        /**
         * @brief Search a Dynamic with the specified name.
         * @param name Dynamic name to find.
         * @return A constant reference to the Dynamic find.
         * @throw utils::ArgError if no Dynamic find.
         */
        Dynamic& get(const std::string& name);

        /**
         * @brief Search a Dynamic with the specified name.
         * @param name Dynamic name to find.
         * @throw utils::ArgError if no Dynamic find.
         */
        bool exist(const std::string& name) const
        { return m_list.find(name) != end(); }

        /**
         * @brief Remove all no permanent value of the list. This function is
         * use to clean not usefull data for the devs::ModelFactory. Linear
         * function.
         */
        void cleanNoPermanent();

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Get/Set
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Get a constant reference to the vpz::DynamicList.
         * @return Get a constant reference to the vpz::DynamicList.
         */
        inline const DynamicList& dynamiclist() const
        { return m_list; }

        /**
         * @brief Get a reference to the vpz::DynamicList.
         * @return Get a reference to the vpz::DynamicList.
         */
        inline DynamicList& dynamiclist()
        { return m_list; }

        /**
         * @brief Get a iterator the begin of the vpz::DynamicList.
         * @return Get a iterator the begin of the vpz::DynamicList.
         */
        iterator begin()
        { return m_list.begin(); }

        /**
         * @brief Get a iterator the end of the vpz::DynamicList.
         * @return Get a iterator the end of the vpz::DynamicList.
         */
        iterator end()
        { return m_list.end(); }

        /**
         * @brief Get a constant iterator the begin of the vpz::DynamicList.
         * @return Get a constant iterator the begin of the
         * vpz::DynamicList.
         */
        const_iterator begin() const
        { return m_list.begin(); }

        /**
         * @brief Get a constant iterator the end of the vpz::DynamicList.
         * @return Get a constant iterator the end of the vpz::DynamicList.
         */
        const_iterator end() const
        { return m_list.end(); }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Functors
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Functor to add a Dynamic to a DynamicList.
         */
        struct AddDynamic
        {
            /**
             * @brief Constructor to add a Dynamic to a Dynamics.
             * @param dynamics The Dynamics that stores new Dynamic.
             */
            AddDynamic(Dynamics& dynamics)
                : m_dynamics(dynamics)
            {}

            /**
             * @brief Add the DynamicList to the Dynamics.
             * @param pair the DynamicList to add.
             */
            void operator()(const DynamicList::value_type& pair)
            { m_dynamics.add(pair.second); }

            Dynamics& m_dynamics; //!< the output parameter.
        };
    private:
        DynamicList     m_list;

    };

}} // namespace vle vpz

#endif
