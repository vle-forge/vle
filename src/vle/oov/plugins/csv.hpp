/** 
 * @file cvs.hpp
 * @brief 
 * @author The vle Development Team
 * @date 2007-11-17
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

#ifndef VLE_OOV_PLUGINS_CSV_HPP
#define VLE_OOV_PLUGINS_CSV_HPP

#include <vle/oov/SimpleFile.hpp>

namespace vle { namespace oov { namespace plugin {

    class CSV : public SimpleFile
    {
    public:
        CSV(const std::string& location);

        virtual ~CSV();

        virtual std::string extension() const;

        virtual void writeSeparator(std::ostream& out);

        virtual void writeHead(std::ostream& out,
                               const std::vector < std::string >& heads);

    };

    DECLARE_OOV_PLUGIN(vle::oov::plugin::CSV);

}}} // namespace vle oov plugin

#endif
