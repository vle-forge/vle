/**
 * @file vle/oov/plugins/csv.hpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

    DECLARE_OOV_PLUGIN(vle::oov::plugin::CSV)

}}} // namespace vle oov plugin

#endif
