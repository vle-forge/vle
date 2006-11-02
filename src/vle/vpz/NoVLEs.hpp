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

    class NoVLEs : public Base
    {
    public:
        NoVLEs()
        { }

        virtual ~NoVLEs()
        { }

        /** 
         * @brief initialise the NoVLEs structure with NoVLE information.
         * @code
         * <NO_VLES>
         *  <NO_VLE MODEL_NAME="system" TRANSLATOR="lifegame-tr">
         *   <SYSTEM NAME="c" LANGUAGE="c++">
         *    <SIZE L="30" C="30" />
         *    <TIME_STEP VALUE="1" />
         *    <SEED VALUE="6352" />
         *   </SYSTEM>
         *  </NO_VLE>
         * </NO_VLES>
         * @endcode
         * 
         * @param elt A XML reference to the node NO_VLES.
         *
         * @throw Exception::Internal if elt is NULL or note on a NO_VLES tags.
         */
        virtual void init(xmlpp::Element* elt);

        /** 
         * @brief Write the NoVLEs information under specified node.
         * @code
         * <NO_VLES>
         *  <NO_VLE MODEL_NAME="system" TRANSLATOR="lifegame-tr">
         *  </NO_VLE>
         * </NO_VLES>
         * @endcode
         * 
         * @param elt a reference to the NO_VLES parent's node.
         *
         * @throw Exception::Internal if elt is NULL.
         */
        virtual void write(xmlpp::Element* elt) const;

        /** 
         * @brief Add a new NoVLE into the list.
         * 
         * @param modelname the name of the NoVLE model.
         * @param novle the NoVLE information.
         *
         * @throw Exception::Internal if modelname already exist.
         */
        void addNoVLE(const std::string& modelname, const NoVLE& novle);

        /** 
         * @brief Just delete all the vpz::NoVLE.
         */
        void clear();

        /** 
         * @brief Del the specified model.
         * 
         * @param modelname the name of the NoVLE model.
         */
        void delNoVLE(const std::string& modelname);

        void fusion(Model& model,
                    Dynamics& dynamics,
                    Graphics& graphics,
                    Experiment& experiment);

        /** 
         * @brief Return true if a NoVLE exist with the novle string.
         * 
         * @param novle name to find.
         * 
         * @return true if NoVLE is found, false otherwise.
         */
        bool exist(const std::string& novle) const
        { return m_novles.find(novle) != m_novles.end(); }

        /** 
         * @brief Return a NoVLE with the specified name.
         * 
         * @param novle name to find.
         * 
         * @return A reference to a NoVLE.
         *
         * @throw Exception::Internal model not found.
         */
        const NoVLE& find(const std::string& novle) const;

        /** 
         * @brief Return a NoVLE with the specified name.
         * 
         * @param novle name to find.
         * 
         * @return A reference to a NoVLE.
         *
         * @throw Exception::Internal model not found.
         */
        NoVLE& find(const std::string& novle);

        /** 
         * @brief Get a constant reference to the NoVLE list.
         * 
         * @return A constant reference.
         */
        const std::map < std::string, NoVLE >& noVLEs() const
        { return m_novles; }

    private:
        std::map < std::string, NoVLE > m_novles;
    };

}} // namespace vle vpz

#endif
