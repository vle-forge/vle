/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/vpz/Vpz.hpp>
#include <fstream>
#include <iomanip>
#include <limits>
#include <vle/version.hpp>
#include <libxml/parser.h>
#include <libxml/tree.h>

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
    out << std::showpoint
        << std::fixed
        << std::setprecision(std::numeric_limits < double >::digits10)
        << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        << "<!DOCTYPE vle_project PUBLIC \"-//VLE TEAM//DTD Strict//EN\" "
        << "\"http://www.vle-project.org/vle-"
        << VLE_MAJOR_VERSION << "." << VLE_MINOR_VERSION << ".0.dtd\">\n";

    m_project.write(out);
}

void Vpz::parseFile(const std::string& filename)
{
    m_filename.assign(filename);
    vpz::SaxParser saxparser(*this);

    try {
        saxparser.parseFile(filename);
    } catch(const std::exception& sax) {
        try {
            validateFile(filename);
        } catch(const std::exception& dom) {
            saxparser.clearParserState();
            throw utils::SaxParserError(fmt(_("%2%\n\n%1%")) % dom.what() %
                                        sax.what());
        }
        throw utils::SaxParserError(sax.what());
    }
}

void Vpz::parseMemory(const std::string& buffer)
{
    m_filename.clear();

    vpz::SaxParser saxparser(*this);
    try {
        saxparser.parseMemory(buffer);
    } catch(const std::exception& sax) {
        try {
            validateMemory(buffer);
        } catch(const std::exception& dom) {
            throw utils::SaxParserError(fmt(_("%2%\n\n%1%")) % dom.what() %
                                        sax.what());
        }
        throw utils::SaxParserError(sax.what());
    }
}

value::Value* Vpz::parseValue(const std::string& buffer)
{
    Vpz vpz;
    SaxParser sax(vpz);
    sax.parseMemory(buffer);

    if (not sax.isValue()) {
        throw utils::ArgError(fmt(_("The buffer [%1%] is not a value.")) %
                              buffer);
    }

    return sax.getValue(0);
}

std::vector < value::Value* > Vpz::parseValues(const std::string& buffer)
{
    Vpz vpz;
    SaxParser sax(vpz);
    sax.parseMemory(buffer);

    if (not sax.isValue()) {
        throw utils::ArgError(fmt(_("The buffer [%1%] is not a value.")) %
                              buffer);
    }

    return sax.getValues();
}

void Vpz::write()
{
    std::ofstream out(m_filename.c_str());

    if (out.fail() or out.bad()) {
        throw utils::FileError(fmt(_(
                "Vpz: cannot open file '%1%' for writing"))
            % m_filename);
    }

    out << std::showpoint
        << std::fixed
        << std::setprecision(std::numeric_limits < double >::digits10)
        << *this;
}

void Vpz::write(const std::string& filename)
{
    m_filename.assign(filename);
    write();
}

std::string Vpz::writeToString() const
{
    std::ostringstream out;

    out << std::showpoint
        << std::fixed
        << std::setprecision(std::numeric_limits < double >::digits10)
        << *this;

    return out.str();
}

void Vpz::clear()
{
    m_filename.clear();
    m_project.clear();
    m_isGzip = false;
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
    xmlParserCtxtPtr ctxt;
    xmlDocPtr doc;

    ctxt = xmlNewParserCtxt();
    if (ctxt == NULL) {
        throw utils::SaxParserError(_("Failed to allocate parser context\n"));
    }

    doc = xmlCtxtReadFile(ctxt, filename.c_str(), NULL, XML_PARSE_DTDVALID);
    if (not doc) {
        std::string msg((fmt(_("Failed to parse '%1%': %2%")) % filename %
                         (ctxt->lastError.message ? ctxt->lastError.message :
                          "")).str());

        xmlFreeParserCtxt(ctxt);

        throw utils::SaxParserError(msg);
    }

    if (ctxt->valid == 0) {
        std::string msg((fmt(_("Failed to validate '%1%': %2%")) % filename %
                         (ctxt->lastError.message ? ctxt->lastError.message :
                          "")).str());

        xmlFreeDoc(doc);
        xmlFreeParserCtxt(ctxt);

        throw utils::SaxParserError(msg);
    }
    xmlFreeParserCtxt(ctxt);
}

void Vpz::validateMemory(const std::string& buffer)
{
    xmlParserCtxtPtr ctxt;
    xmlDocPtr doc;

    ctxt = xmlNewParserCtxt();
    if (ctxt == NULL) {
        throw utils::SaxParserError(_("Failed to allocate parser context\n"));
    }

    doc = xmlCtxtReadMemory(ctxt, buffer.c_str(), buffer.size(), NULL, NULL,
                            XML_PARSE_DTDVALID);
    if (not doc) {
        xmlFreeParserCtxt(ctxt);
        throw utils::SaxParserError(_("Failed to parse memory '%1%'"));
    }

    if (ctxt->valid == 0) {
        xmlFreeDoc(doc);
        xmlFreeParserCtxt(ctxt);
        throw utils::SaxParserError(_("Failed to validate memory"));
    }
    xmlFreeParserCtxt(ctxt);
}

std::set < std::string > Vpz::depends() const
{
    std::set < std::string > dynamics = project().dynamics().depends();
    std::set < std::string > outputs = project().experiment().views().outputs().depends();

    std::set < std::string > result(dynamics);
    result.insert(outputs.begin(), outputs.end());

    return result;
}

}} // namespace vle vpz
