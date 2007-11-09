/** 
 * @file vpz/NoVLEs.hpp
 * @brief 
 * @author The vle Development Team
 * @date mer, 22 fév 2006 01:46:28 +0100
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

#ifndef VLE_VPZ_NOVLES_HPP
#define VLE_VPZ_NOVLES_HPP

#include <vle/vpz/Base.hpp>
#include <vle/vpz/NoVLE.hpp>
#include <map>

namespace vle { namespace vpz {

    /** 
     * @brief Define a list of NoVLE.
     */
    typedef std::map < std::string, NoVLE > NoVLEList;

    /** 
     * @brief NoVLEs is a container based on NoVLEList to build a list of NoVLE
     * using the NoVLE's name as key.
     */
    class NoVLEs : public Base
    {
    public:
        NoVLEs()
        { }

        virtual ~NoVLEs() { }

        /** 
         * @brief Write the NoVLEs information under specified node.
         * @code
         * <novles>
         *  <nvvle name="name" library="lifegame-tr">
         *  </novle>
         * </novles>
         * @endcode
         * @param elt a reference to the NO_VLES parent's node.
         * @throw Exception::Internal if elt is NULL.
         */
        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return NOVLES; }

        ////
        //// NoVLEList mangment
        ////

        /** 
         * @brief Return a constant reference to the NoVLEList.
         * @return A constant reference.
         */
        const NoVLEList& novlelist() const
        { return m_list; }

        /** 
         * @brief Add a new NoVLE into the list.
         * @param novle the NoVLE information.
         * @return a reference to the newly NoVLE builded.
         * @throw Exception::Internal if modelname already exist.
         */
        NoVLE& add(const NoVLE& novle);

        /** 
         * @brief Remove all NoVLE from the NoVLEList.
         */
        inline void clear()
        { m_list.clear(); }

        /** 
         * @brief Del the specified model.
         * @param modelname the name of the NoVLE model.
         */
        void del(const std::string& modelname);

        /** 
         * @brief Merge this NoVLEs object to the specified objects.
         * @param prj A reference to the project.
         * @param model where to merge model.
         * @param dynamics where to merge dynamics.
         * @param conditions where to merge conditions.
         * @param views where to merge views.
         */
        void fusion(const Project& prj,
                    Model& model,
                    Dynamics& dynamics,
                    Conditions& conditions,
                    Views& views);

        /** 
         * @brief Return true if a NoVLE exist with the novle string.
         * @param novle name to find.
         * @return true if NoVLE is found, false otherwise.
         */
        inline bool exist(const std::string& novle) const
        { return m_list.find(novle) != m_list.end(); }

        /** 
         * @brief Return a NoVLE with the specified name.
         * @param novle name to find.
         * @return A reference to a NoVLE.
         * @throw Exception::Internal model not found.
         */
        const NoVLE& get(const std::string& novle) const;

        /** 
         * @brief Return a NoVLE with the specified name.
         * @param novle name to find.
         * @return A reference to a NoVLE.
         * @throw Exception::Internal model not found.
         */
        NoVLE& get(const std::string& novle);

    private:
        NoVLEList       m_list;
    };

}} // namespace vle vpz

#endif
