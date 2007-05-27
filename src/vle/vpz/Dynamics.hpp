/** 
 * @file vpz/Dynamics.hpp
 * @brief 
 * @author The vle Development Team
 * @date lun, 06 fév 2006 11:48:59 +0100
 */

/*
 * Copyright (C) 2006 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef VLE_VPZ_DYNAMICS_HPP
#define VLE_VPZ_DYNAMICS_HPP

#include <set>
#include <vle/vpz/Dynamic.hpp>

namespace vle { namespace vpz {

    class Dynamics : public Base
    {
    public:
        typedef std::map < std::string, Dynamic > DynamicList;
        typedef DynamicList::const_iterator const_iterator;
        typedef DynamicList::iterator iterator;

        Dynamics()
        { }
       
        virtual ~Dynamics()
        { }

        /** 
         * @brief Write the Dynamics information into stream
         * @code.
         * <dynamics>
         *  [...]
         * </dynamics>
         * @endcode
         * 
         * @param out, the output stream.
         */
        virtual void write(std::ostream&  out) const;

        virtual Base::type getType() const
        { return DYNAMICS; }



        /** 
         * @brief Initialise the Dynamics tag with XML information.
         * @code
         * <MODELS>
         *  <MODEL NAME="name">
         *   <DYNAMICS FORMALISM="f1" TYPE="mapping">
         *    <INIT I="4" />
         *   </DYNAMICS>
         *  <MODEL>
         * </MODELS>
         * @endcode
         * 
         * @param elt An XML reference to the MODELS tag.
         *
         * @throw Exception::Internal if elt is null or not on MODELS tags.
         */
        void initFromModels(xmlpp::Element* elt);

        /** 
         * @brief Add a list of Dynamics to the list.
         * 
         * @param dyns A Dynamics objet to add.
         *
         * @throw Exception::Internal if a Dynamic already exist.
         */
        void addDynamics(const Dynamics& dyns);

        /** 
         * @brief Add a Dynamic to the list.
         * 
         * @param dyn the Dynamic to add.
         *
         * @throw Exception::Internal if a dynamic with the same model name
         * already exist.
         */
        void addDynamic(const Dynamic& dyn);

        /** 
         * @brief Just delete the complete list of vpz::Dynamic.
         */
        void clear();

        /** 
         * @brief Delete the dynamic for a specific model name.
         * 
         * @param name the name of the model.
         */
        void delDynamic(const std::string& name);

        /** 
         * @brief Search a Dynamic with the specified name.
         * 
         * @param name Dynamic name to find.
         * 
         * @return A constant reference to the Dynamic find.
         *
         * @throw Exception::Internal if no Dynamic find.
         */
        const Dynamic& find(const std::string& name) const;

        /** 
         * @brief Search a Dynamic with the specified name.
         * 
         * @param name Dynamic name to find.
         * 
         * @return true if founded, false otherwise.
         */
        bool exist(const std::string& name) const;

        /** 
         * @brief Get the list of model's dynamic.
         * 
         * @return A reference to a constant set.
         */
        inline const DynamicList& dynamics() const
        { return m_lst; }


    private:
        DynamicList     m_lst;
    };

}} // namespace vle vpz

#endif
