/**
 * @file vle/vpz/Vpz.cpp
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


#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Debug.hpp>
#include <fstream>

namespace vle { namespace vpz {

Vpz::Vpz(const std::string& filename) :
    m_filename(filename)
{
    parseFile(filename);
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
    out << "<?xml version=\"1.0\" ?>\n"
        << "<!DOCTYPE vle_project SYSTEM "
        << "\"http://vle.univ-littoral.fr/vle-0.5.0.dtd\">";

    m_project.write(out);
    out << std::endl;
}

void Vpz::parseFile(const std::string& filename)
{
    m_filename.assign(filename);

    vpz::SaxParser sax(*this);
    try {
        sax.parse_file(filename);
    } catch(const std::exception& sax) {
        try {
            Vpz::validateFile(m_filename);
        } catch(const std::exception& dom) {
            Throw(utils::SaxParserError, boost::format(
                    "Parse file '%1%':\n[--libxml--]\n%2%[--libvpz--]\n%3%") %
                filename % dom.what() % sax.what());
        }
        Throw(utils::SaxParserError, boost::format(
                "Parse file '%1%':\n[--libvpz--]\n%2%") % filename %
            sax.what());
    }
}

void Vpz::parseMemory(const std::string& buffer)
{
    m_filename.clear();

    vpz::SaxParser sax(*this);
    try {
        sax.parse_memory(buffer);
    } catch(const std::exception& sax) {
        try {
            Vpz::validateFile(buffer);
        } catch(const std::exception& dom) {
            Throw(utils::SaxParserError, boost::format(
                    "Parse memory:\n[--libxml--]\n%1%[--libvpz--]\n%2%") %
                dom.what() % sax.what());
        }
        Throw(utils::SaxParserError, boost::format(
                "Parse memory:\n[--libvpz--]\n%1%") % sax.what());
    }
}

value::Value Vpz::parseValue(const std::string& buffer)
{
    Vpz vpz;
    SaxParser sax(vpz);
    sax.parse_memory(buffer);
    
    Assert(utils::InternalError, sax.isValue(),
           boost::format("The buffer [%1%] is not a value.") % buffer);

    return sax.getValue(0);
}

TrameList Vpz::parseTrame(const std::string& buffer)
{
    Vpz vpz;
    SaxParser sax(vpz);
    sax.parse_memory(buffer);
    
    Assert(utils::InternalError, sax.isTrame(),
           boost::format("The buffer [%1%] is not a trame.") % buffer);

    return sax.tramelist();
}

std::vector < value::Value > Vpz::parseValues(const std::string& buffer)
{
    Vpz vpz;
    SaxParser sax(vpz);
    sax.parse_memory(buffer);
    
    Assert(utils::InternalError, sax.isValue(),
           boost::format("The buffer [%1%] is not a value.") % buffer);

    return sax.getValues();
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

void Vpz::validateFile(const std::string& filename)
{
    xmlpp::DomParser dom;
    dom.set_validate(true);
    dom.parse_file(filename);
}

void Vpz::validateMemory(const Glib::ustring& buffer)
{
    xmlpp::DomParser dom;
    dom.set_validate(true);
    dom.parse_memory(buffer);
}

}} // namespace vle vpz
