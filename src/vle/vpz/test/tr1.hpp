/** 
 * @file vle/vpz/tr1.hpp
 * @brief Translator test.
 * @author The vle Development Team
 * @date jeu, 14 jun 2007 18:48:27 +0200
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

#ifndef VLE_VPZ_TRANSLATOR_TEST_UNIT_1_HPP
#define VLE_VPZ_TRANSLATOR_TEST_UNIT_1_HPP

#include <vle/vpz/Translator.hpp>

namespace vle { namespace vpz { namespace testunit {

    /** 
     * @brief A translator unit test class.
     */
    class tr1 : public Translator
    {
    public:
        tr1(const Project& prj) :
            Translator(prj)
        { }

        virtual ~tr1() 
        { }

        virtual void translate(const std::string& buffer);
    };

}}} // namespace vle vpz testunit

DECLARE_TRANSLATOR(vle::vpz::testunit::tr1);

#endif
