/**
 * @file vle/oov/plugins/Rdata.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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


#ifndef VLE_OOV_PLUGINS_FILETYPE_HPP
#define VLE_OOV_PLUGINS_FILETYPE_HPP

#include <vle/oov/plugins/File.hpp>

namespace vle { namespace oov { namespace plugin {

    class Rdata : public File::FileType
    {
    public:
        virtual std::string extension() const;

        virtual void writeSeparator(std::ostream& out);

        virtual void writeHead(std::ostream& out, const File::Strings& heads);
    };

    class CSV : public File::FileType
    {
    public:
        virtual std::string extension() const;

        virtual void writeSeparator(std::ostream& out);

        virtual void writeHead(std::ostream& out, const File::Strings& heads);
    };

    class Text : public File::FileType
    {
    public:
        virtual std::string extension() const;

        virtual void writeSeparator(std::ostream& out);

        virtual void writeHead(std::ostream& out, const File::Strings& heads);
    };

}}} // namespace vle oov plugin

#endif
