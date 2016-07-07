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


#ifndef VLE_VPZ_DYNAMICS_HPP
#define VLE_VPZ_DYNAMICS_HPP

#include <vle/DllDefines.hpp>
#include <vle/vpz/Dynamic.hpp>
#include <set>
#include <map>

namespace vle {
namespace vpz {

    /**
     * @brief A operator to retrieve the key
     *
     * @param pair the key Dynamic pair.
     * @return DYNAMICS.
     */
    inline std::string dynamicKey(std::pair < std::string, Dynamic > pair)
    { return pair.first; }

    /**
     * @brief Define a list of Dynamcis.
     */
    typedef std::map < std::string, Dynamic > DynamicList;

    /**
     * @brief The vpz::Dynamics class wrap the DynamicList and provides
     * functions to access it.
     */
    class VLE_API Dynamics : public Base
    {
    public:
        typedef DynamicList::iterator iterator;
        typedef DynamicList::const_iterator const_iterator;
        typedef DynamicList::size_type size_type;
        typedef DynamicList::value_type value_type;

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
         * @brief A operator to retrieve the list of keys
         *
         * @return vector of Dynamic names
         */
        std::set < std::string > getKeys();

        /**
         * @brief Write the Dynamics information into stream
         * @code
         * <dynamics>
         *  [...]
         * </dynamics>
         * @endcode
         * @param out the output stream.
         */
        virtual void write(std::ostream& out) const override;

        /**
         * @brief Get the type of this class.
         * @return DYNAMICS.
         */
        virtual Base::type getType() const override
        { return VLE_VPZ_DYNAMICS; }

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

        /**
         * @brief Copy a specified vpz::Dynamic.
         * @param sourcename the name of the current vpz::Dynamic
         * @param targetname the name of the copy
         */
        void copy(const std::string& sourcename,
                  const std::string& targetname);

        /**
         * @brief Rename a specified vpz::Dynamic oldname with the newname
         * parameter.
         * @param oldname The old name of the vpz::Dynamics
         * @param newname The new name of the vpz::Dynamics
         */
        void rename(const std::string& oldname,
                    const std::string& newname);

        /**
         * @brief Fill the std::set with all external Dynamic (ie. in
         * another package).
         * @return A list of package name.
         */
        std::set < std::string > depends() const;

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
