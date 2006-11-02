/** 
 * @file vpz/Graphic.hpp
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

#ifndef VLE_VPZ_GRAPHIC_HPP
#define VLE_VPZ_GRAPHIC_HPP

#include <vle/vpz/Base.hpp>

namespace vle { namespace vpz {

    /**
     * @brief A Graphic represents the GRAPHICS tag of XML project file. It
     * stores a model position.
     */
    class Graphic : public Base
    {
    public:
        Graphic() :
            m_x(0),
            m_y(0),
            m_width(0),
            m_height(0)
        { }

        virtual ~Graphic()
        { }

        /** 
         * @brief Initialise the Graphic with XML.
         * @code
         * <MODEL NAME="top model">
         *  <POSITION X="30" Y="20">
         * </MODEL>
         * @endcode
         * 
         * @param elt A reference to the tag MODEL.
         *
         * @throw Exception::Internal if elt is null or not on MODEL node.
         */
        virtual void init(xmlpp::Element* elt);

        /** 
         * @brief Write the Graphic information under XML node.
         * @code
         * <MODEL NAME="top model">
         *  <POSITION X="30" Y="20">
         * </MODEL>
         * @endcode
         * 
         * @param elt A reference to the tag MODEL.
         */
        virtual void write(xmlpp::Element* elt) const;

        /** 
         * @brief Set this Graphic using Wrapping type.
         * 
         * @param x coordinate.
         * @param y coordinate.
         */
        void setGraphic(int x, int y);

        /** 
         * @brief Set size of this Graphics.
         * 
         * @param width of model.
         * @param height of model.
         */
        void setSize(int width, int height);

        /** 
         * @brief Get the X position of model.
         * 
         * @return a x position.
         */
        int x() const { return m_x; }

        /** 
         * @brief Get the Y position of model.
         * 
         * @return a y position.
         */
        int y() const { return m_y; }

        int width() const { return m_width; }

        int height() const { return m_height; }
        
    private:
        int             m_x;
        int             m_y;
        int             m_width;
        int             m_height;
    };

}} // namespace vle vpz

#endif
