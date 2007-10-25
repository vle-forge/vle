/** 
 * @file OovLocalStreamReader.hpp
 * @brief 
 * @author The vle Development Team
 * @date 2007-10-25
 */

/*
 * Copyright (C) 2007 - The vle Development Team
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

#ifndef VLE_OOV_OOVLOCALSTREAMREADER_HPP
#define VLE_OOV_OOVLOCALSTREAMREADER_HPP

#include <vle/oov/LocalStreamReader.hpp>

namespace vle { namespace oov {

    class OovLocalStreamReader : public LocalStreamReader
    {
    public:
        OovLocalStreamReader()
        { }

        virtual ~OovLocalStreamReader()
        { }
        
        /** 
         * @brief Oov LocalStreamReader ask to the CairoPlugin to write a image
         * when receive a value.
         * @param trame 
         */
        virtual void onValue(const vpz::ValueTrame& trame);
    };

}} // namespace vle oov

#endif
