/** 
 * @file vpz/Vpz.cpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 28 fév 2006 12:00:10 +0100
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

#include <vle/vpz/Vpz.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>
#include <fstream>

namespace vle { namespace vpz {

using namespace vle::utils;

Vpz::Vpz(const std::string& filename) :
    m_filename(filename)
{
    parse_file(filename);
}

Vpz::Vpz(const Vpz& vpz) :
    Base(vpz),
    m_isGzip(vpz.m_isGzip),
    m_filename(vpz.m_filename),
    m_project(vpz.m_project)
{
}

void Vpz::write(std::ostream& out) const
{
    out << "<?xml version=\"1.0\" ?>\n";
    m_project.write(out);
    out << std::endl;
}

void Vpz::parse_file(const std::string& filename)
{
    m_filename.assign(filename);

    vpz::VLESaxParser sax(*this);
    sax.parse_file(filename);
}

void Vpz::parse_memory(const std::string& buffer)
{
    m_filename.clear();

    vpz::VLESaxParser sax(*this);
    sax.parse_memory(buffer);
}

void Vpz::expandTranslator()
{
    m_project.expandTranslator();
}

bool Vpz::hasNoVLE() const
{
    return m_project.hasNoVLE();
}

value::Value Vpz::parse_value(const std::string& buffer)
{
    Vpz vpz;
    VLESaxParser sax(vpz);
    sax.parse_memory(buffer);
    
    Assert(utils::InternalError, sax.is_value(),
           boost::format("The buffer [%1%] is not a value.") % buffer);

    return sax.get_value(0);
}

TrameList Vpz::parse_trame(const std::string& buffer)
{
    Vpz vpz;
    VLESaxParser sax(vpz);
    sax.parse_memory(buffer);
    
    Assert(utils::InternalError, sax.is_trame(),
           boost::format("The buffer [%1%] is not a trame.") % buffer);

    return sax.tramelist();
}

std::vector < value::Value > Vpz::parse_values(const std::string& buffer)
{
    Vpz vpz;
    VLESaxParser sax(vpz);
    sax.parse_memory(buffer);
    
    Assert(utils::InternalError, sax.is_value(),
           boost::format("The buffer [%1%] is not a value.") % buffer);

    return sax.get_values();
}

void Vpz::write()
{
    std::ofstream out(m_filename.c_str());
    if (out.fail() or out.bad()) {
        Throw(utils::SaxParserError,
              (boost::format("Cannot open file %1% for writing.") %
               m_filename));
    }
    out << *this;
}

void Vpz::write(const std::string& filename)
{
    m_filename.assign(filename);
    write();
}

void Vpz::clear()
{
    m_filename.clear();
    m_project.clear();
    m_isGzip = false;
}

std::string Vpz::writeToString()
{
    std::ostringstream out;
    out << *this;
    return out.str();
}

void Vpz::fixExtension(std::string& filename)
{
    const std::string::size_type dot = filename.find_last_of('.');
    if (dot == std::string::npos) {
	filename += ".vpz";
    } else {
        if (filename.size() >= 4) {
            const std::string extension(filename, dot, 4);
	    if (extension != ".vpz") {
                filename += ".vpz";
            }
        } else {
            filename += ".vpz";
        }
    }
}

}} // namespace vle vpz
