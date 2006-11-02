/** 
 * @file vpz/Dynamic.hpp
 * @brief 
 * @author The vle Development Team
 * @date lun, 06 fév 2006 10:10:13 +0100
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

#ifndef VLE_VPZ_DYNAMIC_HPP
#define VLE_VPZ_DYNAMIC_HPP

#include <vle/vpz/Base.hpp>

namespace vle { namespace vpz {


    /** 
     * @brief A Dynamic represents the DYNAMICS tag of XML project file. It
     * stores the name of model, the formalism use, the type, wrapping or
     * mapping and an XML attached to the formalism to initialise the simulation
     * plugin.
     */
    class Dynamic : public Base
    {
    public:
        enum Type { WRAPPING, MAPPING };
        
        Dynamic() 
        { }

        virtual ~Dynamic()
        { }

        /** 
         * @brief Initialise Dynamic with XML information.
         * @code
         * <DYNAMICS FORMALISM="name" TYPE="mapping">
         *  <INIT I="5" />
         * </DYNAMICS>
         * @endcode
         * 
         * @param elt A XML reference to the DYNAMICS tag.
         */
        virtual void init(xmlpp::Element* elt);

        /** 
         * @brief Write Dynamic information under specified node.
         * @code@
         * <MODEL NAME="counter">
         *  <DYNAMICS FORMALISM="name" TYPE="mapping">
         *   <INIT I="5" />
         *  </DYNAMICS>
         * </MODEL>
         * @endcode@
         * 
         * @param elt A XML reference to the MODEL tag.
         */
        virtual void write(xmlpp::Element* elt) const;

        /** 
         * @brief Set this Dynamic using Mapping type.
         * 
         * @param formalism formalism use by simulation plugin.
         * @param dynamic XML attached to the simulation plugin.
         *
         * @throw Exception::Internal if formalism is empty.
         */
        void setMappingDynamic(const std::string& dynamic,
                               const std::string& formalism);

        /** 
         * @brief Set this Dynamic using Wrapping type.
         * 
         * @param formalism formalism use by simulation plugin.
         * @param dynamic XML attached to the simulation plugin.
         *
         * @throw Exception::Internal if formalism is empty.
         */
        void setWrappingDynamic(const std::string& dynamic,
                                const std::string& formalism);

        /** 
         * @brief Get the XML dynamic. Can be empty.
         * 
         * @return an XML format.
         */
        inline const std::string& dynamic() const { return m_dynamic; }
        
        /** 
         * @brief The simulation plugin's name.
         * 
         * @return a string representation of plugin.
         */
        inline const std::string& formalism() const { return m_formalism; }
        
        /** 
         * @brief The type of dynamic, Wrapping or Mapping.
         * 
         * @return a type of representation.
         */
        inline Dynamic::Type type() const { return m_type; }

    private:
        std::string     m_dynamic;
        std::string     m_formalism;
        Dynamic::Type   m_type;
    };

}} // namespace vle vpz

#endif
