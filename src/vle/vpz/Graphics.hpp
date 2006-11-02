/** 
 * @file vpz/Graphics.hpp
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

#ifndef VLE_VPZ_GRAPHICS_HPP
#define VLE_VPZ_GRAPHICS_HPP

#include <map>
#include <vle/vpz/Graphic.hpp>

namespace vle { namespace vpz {

    class Graphics : public Base
    {
    public:
        Graphics()
        { }

        virtual ~Graphics()
        { }
        
        /** 
         * @brief Initialise the Graphics with XML tags.
         * @code
         * <GRAPHICS>
         *  <MODELS>
         *   <MODEL NAME="toto">
         *    <POSITION X="1" Y="2" />
         *   </MODEL>
         *  </MODELS>
         * </GRAPHICS>
         * @endcode
         * 
         * @param elt a reference to the GRAPHICS tag.
         *
         * @throw Exception::Internal if elt is null or elt is not a GRAPHICS
         * tag.
         */
        virtual void init(xmlpp::Element* elt);
        
        /** 
         * @brief Write a Graphics information if graphics exist.
         * @code
         * <GRAPHICS>
         *  <MODELS>
         *   <MODEL NAME="toto">
         *    <POSITION X="1" Y="2" />
         *   </MODEL>
         *  <MODELS>
         * </GRAPHICS>
         * @endcode
         * 
         * @param elt a reference to the parent GRAPHICS tag.
         *
         * @throw Exception::Internal if elt is null.
         */
        virtual void write(xmlpp::Element* elt) const;

        /** 
         * @brief Initialise the Graphics with XML tags.
         * @code
         * <MODELS>
         *  <MODEL NAME="toto">
         *   <POSITION X="1" Y="2" />
         *  </MODEL>
         * </MODELS>
         * @endcode
         * 
         * @param elt a reference to the tag.
         *
         * @throw Exception::Internal if elt is null or elt is not a MODELS
         * tag.
         */
        void initFromModels(xmlpp::Element* elt);

        /** 
         * @brief Add a list of Dynamics to the list.
         * 
         * @param g A Graphics object to add.
         *
         * @throw Exception::Internal if a Graphic already exist.
         */
        void addGraphics(const Graphics& g);

        /** 
         * @brief Add a Graphic to the set.
         * 
         * @param name the model name.
         * @param g the graphic to attach to the model.
         *
         * @throw Exception::Internal if a Graphic with the same model name
         * already exist.
         */
        void addGraphic(const std::string& name, const Graphic& g);

        /** 
         * @brief Just delete the complete list of vpz::Graphic.
         */
        void clear();

        /** 
         * @brief Delete the Graphic for a specific model name.
         * 
         * @param name the name of the model.
         */
        void delGraphic(const std::string& name);

        /** 
         * @brief Get the Graphic for a specific model name.
         * 
         * @param name the name of the model to get the Graphic.
         * 
         * @return A reference to a Graphic.
         *
         * @throw Exception::Internal if model name does not exist.
         */
        const Graphic& graphic(const std::string& name) const;

        /** 
         * @brief Get the list of Graphic.
         * 
         * @return A constant reference to the Graphic.
         */
        inline const std::map < std::string, Graphic >& graphics() const
        { return m_lst; }

    private:
        std::map < std::string, Graphic >    m_lst;
    };

}} // namespace vle vpz

#endif
