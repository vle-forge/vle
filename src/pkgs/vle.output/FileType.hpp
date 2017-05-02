/*
 * @file vle/oov/plugins/FileType.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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
#define VLE_OOV_PLUGINS_FILETYPE_HPP 1

#include "File.hpp"

namespace vle {
namespace oov {
namespace plugin {

class Rdata : public File::FileType
{
public:
    virtual ~Rdata();

    virtual std::string extension() const;

    virtual void writeSeparator(std::ostream& out);

    virtual void writeHead(std::ostream& out, const File::Strings& heads);
};

class CSV : public File::FileType
{
public:
    virtual ~CSV();

    virtual std::string extension() const;

    virtual void writeSeparator(std::ostream& out);

    virtual void writeHead(std::ostream& out, const File::Strings& heads);
};

class Text : public File::FileType
{
public:
    virtual ~Text();

    virtual std::string extension() const;

    virtual void writeSeparator(std::ostream& out);

    virtual void writeHead(std::ostream& out, const File::Strings& heads);
};
}
}
} // namespace vle oov plugin

#endif
