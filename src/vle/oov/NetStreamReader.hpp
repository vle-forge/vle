/** 
 * @file NetStreamReader.hpp
 * @brief 
 * @author The vle Development Team
 * @date 2007-07-16
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

#ifndef VLE_OOV_NETSTREAMREADER_HPP
#define VLE_OOV_NETSTREAMREADER_HPP

#include <vle/oov/StreamReader.hpp>



namespace vle { namespace oov {

    class NetStreamReader
    {
    public:
        NetStreamReader();

        virtual ~NetStreamReader();


        void process()
        {
            while (buffer = resau.lecture()) {
                vpz::SaxParser parser(buffer);
                if (parser.isTrame()) {
                    if (parser.isParameterTrame()) {
                    } else if (parser.isNewObservableTrame()) {
                    } else if (parser.isDelObservableTrame()) {
                    } else if (parser.isValueTrame()) {
                    } else if (parser.isEndTrame()) {
                    }
                }
            }
        }



}} // namespace vle oov

#endif
