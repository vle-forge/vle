/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
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

#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/XML.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/BaseModel.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/vpz/Vpz.hpp>

#include "utils/i18n.hpp"
#include "vpz/SaxParser.hpp"

#include <fstream>
#include <sstream>
#include <utility>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/cast.hpp>
#include <boost/spirit/include/qi.hpp>

#include <cerrno>
#include <cstdlib>
#include <cstring>

#include <expat.h>

namespace vle {
namespace vpz {

struct xml_compare
{
    inline bool operator()(const char* s1, const char* s2) const
    {
        return strcmp(s1, s2) < 0;
    }
};

using startfunc = void (SaxParser::*)(const char**);
using endfunc = void (SaxParser::*)();
using StartFuncList = std::map<const char*, startfunc, xml_compare>;
using EndFuncList = std::map<const char*, endfunc, xml_compare>;

StartFuncList&
starts()
{
    static StartFuncList ret = { { "boolean", &SaxParser::onBoolean },
                                 { "integer", &SaxParser::onInteger },
                                 { "double", &SaxParser::onDouble },
                                 { "string", &SaxParser::onString },
                                 { "set", &SaxParser::onSet },
                                 { "matrix", &SaxParser::onMatrix },
                                 { "map", &SaxParser::onMap },
                                 { "key", &SaxParser::onKey },
                                 { "tuple", &SaxParser::onTuple },
                                 { "table", &SaxParser::onTable },
                                 { "xml", &SaxParser::onXML },
                                 { "null", &SaxParser::onNull },
                                 { "vle_project", &SaxParser::onVLEProject },
                                 { "structures", &SaxParser::onStructures },
                                 { "model", &SaxParser::onModel },
                                 { "in", &SaxParser::onIn },
                                 { "out", &SaxParser::onOut },
                                 { "port", &SaxParser::onPort },
                                 { "submodels", &SaxParser::onSubModels },
                                 { "connections", &SaxParser::onConnections },
                                 { "connection", &SaxParser::onConnection },
                                 { "origin", &SaxParser::onOrigin },
                                 { "destination", &SaxParser::onDestination },
                                 { "dynamics", &SaxParser::onDynamics },
                                 { "dynamic", &SaxParser::onDynamic },
                                 { "experiment", &SaxParser::onExperiment },
                                 { "conditions", &SaxParser::onConditions },
                                 { "condition", &SaxParser::onCondition },
                                 { "views", &SaxParser::onViews },
                                 { "outputs", &SaxParser::onOutputs },
                                 { "output", &SaxParser::onOutput },
                                 { "view", &SaxParser::onView },
                                 { "observables", &SaxParser::onObservables },
                                 { "observable", &SaxParser::onObservable },
                                 { "attachedview",
                                   &SaxParser::onAttachedView },
                                 { "classes", &SaxParser::onClasses },
                                 { "class", &SaxParser::onClass } };

    return ret;
}

EndFuncList&
ends()
{
    static EndFuncList ret = { { "boolean", &SaxParser::onEndBoolean },
                               { "integer", &SaxParser::onEndInteger },
                               { "double", &SaxParser::onEndDouble },
                               { "string", &SaxParser::onEndString },
                               { "set", &SaxParser::onEndSet },
                               { "matrix", &SaxParser::onEndMatrix },
                               { "map", &SaxParser::onEndMap },
                               { "key", &SaxParser::onEndKey },
                               { "tuple", &SaxParser::onEndTuple },
                               { "table", &SaxParser::onEndTable },
                               { "xml", &SaxParser::onEndXML },
                               { "null", &SaxParser::onEndNull },
                               { "vle_project", &SaxParser::onEndVLEProject },
                               { "structures", &SaxParser::onEndStructures },
                               { "model", &SaxParser::onEndModel },
                               { "in", &SaxParser::onEndIn },
                               { "out", &SaxParser::onEndOut },
                               { "port", &SaxParser::onEndPort },
                               { "submodels", &SaxParser::onEndSubModels },
                               { "connections", &SaxParser::onEndConnections },
                               { "connection", &SaxParser::onEndConnection },
                               { "origin", &SaxParser::onEndOrigin },
                               { "destination", &SaxParser::onEndDestination },
                               { "dynamics", &SaxParser::onEndDynamics },
                               { "dynamic", &SaxParser::onEndDynamic },
                               { "experiment", &SaxParser::onEndExperiment },
                               { "conditions", &SaxParser::onEndConditions },
                               { "condition", &SaxParser::onEndCondition },
                               { "views", &SaxParser::onEndViews },
                               { "outputs", &SaxParser::onEndOutputs },
                               { "output", &SaxParser::onEndOutput },
                               { "view", &SaxParser::onEndView },
                               { "observables", &SaxParser::onEndObservables },
                               { "observable", &SaxParser::onEndObservable },
                               { "attachedview",
                                 &SaxParser::onEndAttachedView },
                               { "classes", &SaxParser::onEndClasses },
                               { "class", &SaxParser::onEndClass } };

    return ret;
}

SaxParser::SaxParser(Vpz& vpz)
  : m_ctxt(nullptr)
  , m_vpzstack(vpz)
  , m_vpz(vpz)
  , m_isValue(false)
  , m_isVPZ(false)
{}

struct parser_data
{
    std::weak_ptr<XML_ParserStruct> parser;
    SaxParser& sax;

    bool is_in_cdata_section = false;

    parser_data(std::shared_ptr<XML_ParserStruct> parser_, SaxParser& sax_)
      : parser(parser_)
      , sax(sax_)
      , is_in_cdata_section(false)
    {
        sax.clear();
    }

    void clear()
    {
        sax.clear();

        is_in_cdata_section = false;
    }

    void stop_parser(const std::string& msg)
    {
        sax.error(msg);

        if (auto sp_parser = parser.lock())
            XML_StopParser(sp_parser.get(), XML_FALSE);
    }
};

static void
XML_StartElementHandler(void* userData,
                        const XML_Char* name,
                        const XML_Char** atts)
{
    auto* sax = static_cast<parser_data*>(userData);

    sax->sax.clearLastCharactersStored();

    auto it = starts().find(name);
    if (it != starts().end()) {
        try {
            (sax->sax.*(it->second))(atts);
        } catch (const std::exception& e) {
            sax->stop_parser(e.what());
        }
    } else {
        sax->stop_parser(utils::format(_("Unknown tag '%s'"), name));
    }
}

static void
XML_EndElementHandler(void* userData, const XML_Char* name)
{
    auto* sax = static_cast<parser_data*>(userData);

    auto it = ends().find(name);
    if (it != ends().end()) {
        try {
            (sax->sax.*(it->second))();
        } catch (const std::exception& e) {
            sax->stop_parser(e.what());
        }
    } else {
        sax->stop_parser(utils::format(_("Unknown end tag '%s'"), name));
    }
}

static void
XML_CharacterDataHandler(void* userData, const XML_Char* s, int len)
{
    auto* sax = static_cast<parser_data*>(userData);

    if (sax->is_in_cdata_section) {
        sax->sax.addToCdata(std::string(s, len));
    } else {
        sax->sax.addToCharacters(std::string(s, len));
    }
}

static void
XML_StartCdataSectionHandler(void* userData)
{
    auto* sax = static_cast<parser_data*>(userData);

    sax->is_in_cdata_section = true;
}

static void
XML_EndCdataSectionHandler(void* userData)
{
    auto* sax = static_cast<parser_data*>(userData);

    sax->is_in_cdata_section = false;
}

static std::shared_ptr<XML_ParserStruct>
create_parser()
{
    auto parser = XML_ParserCreate(nullptr);

    return std::shared_ptr<XML_ParserStruct>(parser, &XML_ParserFree);
}

void
SaxParser::parse(std::istream& is, std::size_t buffer_size)
{
    auto parser = create_parser();

    parser_data data(parser, *this);
    XML_SetElementHandler(
      parser.get(), XML_StartElementHandler, XML_EndElementHandler);
    XML_SetCharacterDataHandler(parser.get(), XML_CharacterDataHandler);
    XML_SetUserData(parser.get(), reinterpret_cast<void*>(&data));
    XML_SetCdataSectionHandler(
      parser.get(), XML_StartCdataSectionHandler, XML_EndCdataSectionHandler);

    enum class sax_parser_status
    {
        success = 0,
        not_enough_memory,
        file_format_error
    };

    auto status = sax_parser_status::success;
    int done;
    XML_Size line = 0;
    XML_Size column = 0;

    do {
        char* buffer = static_cast<char*>(
          XML_GetBuffer(parser.get(), static_cast<int>(buffer_size)));
        if (buffer == nullptr) {
            status = sax_parser_status::not_enough_memory;
            break;
        }

        is.read(buffer, buffer_size);
        auto len = static_cast<int>(is.gcount());
        done = len < static_cast<std::streamsize>(buffer_size);

        if (XML_ParseBuffer(parser.get(), len, done) == XML_STATUS_ERROR) {
            if (status == sax_parser_status::success)
                status = sax_parser_status::file_format_error;

            line = XML_GetCurrentLineNumber(parser.get());
            column = XML_GetCurrentColumnNumber(parser.get());
            break;
        }
    } while (!done);

    switch (status) {
    case sax_parser_status::success:
        if (!m_isVPZ)
            if (!m_valuestack.getResults().empty())
                m_isValue = true;
        return;
    case sax_parser_status::not_enough_memory:
        throw utils::SaxParserError(_("Not enough memory"));
    case sax_parser_status::file_format_error:
        throw utils::SaxParserError(
          _("Error parsing at %ld:%ld (internal error: %s"),
          line,
          column,
          m_error.c_str());
    }
}

void
SaxParser::parseFile(const std::string& filename)
{
    std::ifstream ifs(filename);
    if (not ifs.is_open())
        throw utils::SaxParserError(_("Error opening file `%s'"),
                                    filename.c_str());

    parse(ifs, BUFSIZ);
}

void
SaxParser::parseMemory(const std::string& buffer)
{
    std::istringstream iss(buffer);

    parse(iss, BUFSIZ);
}

void
SaxParser::onBoolean(const char**)
{
    m_valuestack.pushBoolean();
}

void
SaxParser::onInteger(const char**)
{
    m_valuestack.pushInteger();
}

void
SaxParser::onDouble(const char**)
{
    m_valuestack.pushDouble();
}

void
SaxParser::onString(const char**)
{
    m_valuestack.pushString();
}

void
SaxParser::onSet(const char**)
{
    m_valuestack.pushSet();
}

void
SaxParser::onMatrix(const char** att)
{
    const char* rows = nullptr;
    const char* columns = nullptr;
    const char* rowmax = nullptr;
    const char* columnmax = nullptr;
    const char* columnstep = nullptr;
    const char* rowstep = nullptr;

    for (int i = 0; att[i] != nullptr; i += 2) {
        if (strcmp(att[i], "rows") == 0) {
            rows = att[i + 1];
        } else if (strcmp(att[i], "columns") == 0) {
            columns = att[i + 1];
        } else if (strcmp(att[i], "columnmax") == 0) {
            columnmax = att[i + 1];
        } else if (strcmp(att[i], "rowmax") == 0) {
            rowmax = att[i + 1];
        } else if (strcmp(att[i], "columnstep") == 0) {
            columnstep = att[i + 1];
        } else if (strcmp(att[i], "rowstep") == 0) {
            rowstep = att[i + 1];
        }
    }

    if (not rows or not columns) {
        throw utils::SaxParserError(
          _("Matrix tag does not have 'row' or 'column' "
            "attribute"));
    }

    try {
        using boost::numeric_cast;
        using m_t = value::Matrix::index;

        m_valuestack.pushMatrix(
          numeric_cast<m_t>(charToInt(columns)),
          numeric_cast<m_t>(charToInt(rows)),
          numeric_cast<m_t>(columnmax ? charToInt(columnmax) : 0),
          numeric_cast<m_t>(rowmax ? charToInt(rowmax) : 0),
          numeric_cast<m_t>(columnstep ? charToInt(columnstep) : 0),
          numeric_cast<m_t>(rowstep ? charToInt(rowstep) : 0));
    } catch (const std::exception& e) {
        throw utils::SaxParserError(
          _("Matrix tag does not convert a attribute '%s'"), e.what());
    }
}

void
SaxParser::onMap(const char**)
{
    m_valuestack.pushMap();
}

void
SaxParser::onKey(const char** att)
{
    const char* name = nullptr;

    for (int i = 0; att[i] != nullptr; i += 2) {
        if (strcmp(att[i], "name") == 0) {
            name = att[i + 1];
        }
    }

    if (not name) {
        throw utils::SaxParserError(
          _("Key value tag does not have attribute 'name'"));
    }

    m_valuestack.pushMapKey(name);
}

void
SaxParser::onTuple(const char**)
{
    m_valuestack.pushTuple();
}

void
SaxParser::onTable(const char** att)
{
    const char* width = nullptr;
    const char* height = nullptr;

    for (int i = 0; att[i] != nullptr; i += 2) {
        if (strcmp(att[i], "width") == 0) {
            width = att[i + 1];
        } else if (strcmp(att[i], "height") == 0) {
            height = att[i + 1];
        }
    }

    if (not width or not height) {
        throw utils::SaxParserError(_("Table value tag does not have "
                                      "attributes 'width' or 'height'"));
    }

    try {
        m_valuestack.pushTable(charToInt(width), charToInt(height));
    } catch (const std::exception& e) {
        throw utils::SaxParserError(_("Table value tag can not convert "
                                      "attributes 'width' or 'height': %s"),
                                    e.what());
    }
}

void
SaxParser::onXML(const char**)
{
    m_valuestack.pushXml();
}

void
SaxParser::onNull(const char**)
{
    m_valuestack.pushNull();
}

void
SaxParser::onVLEProject(const char** att)
{
    assert(not m_isValue);

    m_isVPZ = true;
    m_vpzstack.pushVpz(att);
}

void
SaxParser::onStructures(const char**)
{
    m_vpzstack.pushStructure();
}

void
SaxParser::onModel(const char** att)
{
    m_vpzstack.pushModel(att);
}

void
SaxParser::onIn(const char**)
{
    m_vpzstack.pushPortType("in");
}

void
SaxParser::onOut(const char**)
{
    m_vpzstack.pushPortType("out");
}

void
SaxParser::onPort(const char** att)
{
    m_vpzstack.pushPort(att);
}

void
SaxParser::onSubModels(const char**)
{
    m_vpzstack.pushSubModels();
}

void
SaxParser::onConnections(const char**)
{
    m_vpzstack.pushConnections();
}

void
SaxParser::onConnection(const char** att)
{
    m_vpzstack.pushConnection(att);
}

void
SaxParser::onOrigin(const char** att)
{
    m_vpzstack.pushOrigin(att);
}

void
SaxParser::onDestination(const char** att)
{
    m_vpzstack.pushDestination(att);
}

void
SaxParser::onDynamics(const char**)
{
    m_vpzstack.pushDynamics();
}

void
SaxParser::onDynamic(const char** att)
{
    m_vpzstack.pushDynamic(att);
}

void
SaxParser::onExperiment(const char** att)
{
    m_vpzstack.pushExperiment(att);
}

void
SaxParser::onConditions(const char**)
{
    m_vpzstack.pushConditions();
}

void
SaxParser::onCondition(const char** att)
{
    m_vpzstack.pushCondition(att);
}

void
SaxParser::onViews(const char**)
{
    m_vpzstack.pushViews();
}

void
SaxParser::onOutputs(const char**)
{
    m_vpzstack.pushOutputs();
}

void
SaxParser::onOutput(const char** att)
{
    m_vpzstack.pushOutput(att);
}

void
SaxParser::onView(const char** att)
{
    m_vpzstack.pushView(att);
}

void
SaxParser::onObservables(const char**)
{
    m_vpzstack.pushObservables();
}

void
SaxParser::onObservable(const char** att)
{
    m_vpzstack.pushObservable(att);
}

void
SaxParser::onAttachedView(const char** att)
{
    m_vpzstack.pushAttachedView(att);
}

void
SaxParser::onClasses(const char**)
{
    m_vpzstack.pushClasses();
}

void
SaxParser::onClass(const char** att)
{
    m_vpzstack.pushClass(att);
}

void
SaxParser::onEndBoolean()
{
    m_valuestack.pushOnVectorValue<value::Boolean>(
      charToBoolean(((char*)lastCharactersStored().c_str())));
}

void
SaxParser::onEndInteger()
{
    m_valuestack.pushOnVectorValue<value::Integer>(
      static_cast<int>(charToInt(((char*)lastCharactersStored().c_str()))));
}

void
SaxParser::onEndDouble()
{
    m_valuestack.pushOnVectorValue<value::Double>(
      charToDouble(((char*)lastCharactersStored().c_str())));
}

void
SaxParser::onEndString()
{
    m_valuestack.pushOnVectorValue<value::String>(lastCharactersStored());
}

void
SaxParser::onEndKey()
{}

void
SaxParser::onEndSet()
{
    m_valuestack.popValue();
}

void
SaxParser::onEndMatrix()
{
    m_valuestack.popValue();
}

void
SaxParser::onEndMap()
{
    m_valuestack.popValue();
}

void
SaxParser::onEndTuple()
{
    value::Tuple& tuple(m_valuestack.topValue()->toTuple());

    std::string cpy(lastCharactersStored());
    boost::algorithm::trim(cpy);

    std::vector<std::string> result;
    boost::algorithm::split(
      result, cpy, boost::algorithm::is_any_of(" \n\t\r"));

    for (auto& elem : result) {
        boost::algorithm::trim(elem);
        if (not(elem).empty()) {
            tuple.add(charToDouble(((elem).c_str())));
        }
    }

    m_valuestack.popValue();
}

void
SaxParser::onEndTable()
{
    value::Table& table(m_valuestack.topValue()->toTable());

    std::string cpy(lastCharactersStored());
    boost::algorithm::trim(cpy);

    std::vector<std::string> result;
    boost::algorithm::split(
      result, cpy, boost::algorithm::is_any_of(" \n\t\r"));

    size_t size;
    try {
        size = boost::numeric_cast<size_t>(table.width() * table.height());
    } catch (const std::exception& /*e*/) {
        throw utils::SaxParserError(
          _("VPZ parser: bad height (%lu) or width (%lu) table "),
          static_cast<unsigned long>(table.width()),
          static_cast<unsigned long>(table.height()));
    }

    if (result.size() != size) {
        throw utils::SaxParserError(_("VPZ parser: bad height or width "
                                      "for number of real in table"));
    }

    value::Table::index i = 0;
    value::Table::index j = 0;
    for (auto& elem : result) {
        boost::algorithm::trim(elem);
        if (not(elem).empty()) {
            table.get(i, j) = charToDouble(((elem).c_str()));
            if (i + 1 >= table.width()) {
                i = 0;
                if (j + 1 >= table.height()) {
                    break;
                } else {
                    j++;
                }
            } else {
                i++;
            }
        }
    }

    m_valuestack.popValue();
}

void
SaxParser::onEndXML()
{
    m_valuestack.pushOnVectorValue<value::Xml>(m_cdata);
    m_cdata.clear();
}

void
SaxParser::onEndNull()
{
    m_valuestack.pushOnVectorValue<value::Null>();
}

void
SaxParser::onEndPort()
{
    if (m_vpzstack.top()->isCondition()) {
        auto& vals = m_vpzstack.popConditionPort();
        auto& lst = getValues();

        for (auto& elem : lst)
            vals.emplace_back(elem);

        m_valuestack.clear();
    } else if (m_vpzstack.top()->isObservablePort()) {
        m_vpzstack.pop();
    }
}

void
SaxParser::onEndIn()
{
    delete m_vpzstack.pop();
}

void
SaxParser::onEndOut()
{
    delete m_vpzstack.pop();
}

void
SaxParser::onEndStructures()
{
    delete m_vpzstack.pop();
}

void
SaxParser::onEndSubModels()
{
    delete m_vpzstack.pop();
}

void
SaxParser::onEndModel()
{
    delete m_vpzstack.pop();
}

void
SaxParser::onEndConnections()
{
    delete m_vpzstack.pop();
}

void
SaxParser::onEndConnection()
{}

void
SaxParser::onEndVLEProject()
{
    m_vpzstack.pop();
}

void
SaxParser::onEndConditions()
{
    m_vpzstack.pop();
}

void
SaxParser::onEndCondition()
{
    m_vpzstack.pop();
}

void
SaxParser::onEndOutputs()
{
    m_vpzstack.pop();
}

void
SaxParser::onEndDynamics()
{
    m_vpzstack.pop();
}

void
SaxParser::onEndViews()
{
    m_vpzstack.pop();
}

void
SaxParser::onEndObservables()
{
    m_vpzstack.pop();
}

void
SaxParser::onEndObservable()
{
    m_vpzstack.pop();
}

void
SaxParser::onEndExperiment()
{
    m_vpzstack.pop();
}

void
SaxParser::onEndDestination()
{
    m_vpzstack.buildConnection();
}

void
SaxParser::onEndView()
{}

void
SaxParser::onEndOrigin()
{}

void
SaxParser::onEndReplicas()
{}

void
SaxParser::onEndAttachedView()
{}

void
SaxParser::onEndDynamic()
{}

void
SaxParser::onEndOutput()
{
    if (m_vpzstack.top()->isOutput()) {
        auto* out = dynamic_cast<Output*>(m_vpzstack.top());
        auto& lst = getValues();
        if (not lst.empty()) {
            if (lst.size() > 1) {
                throw utils::SaxParserError(
                  _("VPZ parser: multiples values for output"));
            }

            if (lst[0].get()) {
                out->setData(lst[0]);
                m_valuestack.clear();
            }
        }
        m_vpzstack.popOutput();
    }
}

void
SaxParser::onEndClasses()
{
    m_vpzstack.popClasses();
}

void
SaxParser::onEndClass()
{
    m_vpzstack.popClass();
}

void
SaxParser::clear()
{
    m_error.clear();
    m_vpzstack.clear();
    m_valuestack.clear();
    m_lastCharacters.clear();
    m_cdata.clear();
    m_vpz.clear();

    m_isValue = false;
    m_isVPZ = false;
}

void
SaxParser::error(std::string error)
{
    m_error = error;
}

std::string
SaxParser::error() const
{
    return m_error;
}

const std::vector<std::shared_ptr<value::Value>>&
SaxParser::getValues() const
{
    return m_valuestack.getResults();
}

std::vector<std::shared_ptr<value::Value>>&
SaxParser::getValues()
{
    return m_valuestack.getResults();
}

const std::shared_ptr<value::Value>&
SaxParser::getValue(const size_t pos) const
{
    return m_valuestack.getResult(pos);
}

bool
charToBoolean(const char* str)
{
    if ((strncmp(str, "true", 4) == 0) || (strncmp(str, "1", 1) == 0))
        return true;

    return false;
}

long int
charToInt(const char* str)
{
    auto c_str = reinterpret_cast<const char*>(str);
    long int dest;

    if (!boost::spirit::qi::parse(
          c_str, c_str + strlen(c_str), boost::spirit::qi::long_, dest))
        throw utils::SaxParserError(_("fail to convert to integer `%s'"),
                                    c_str);

    return dest;
}

unsigned long int
charToUnsignedInt(const char* str)
{
    auto c_str = reinterpret_cast<const char*>(str);
    unsigned long int dest;

    if (!boost::spirit::qi::parse(
          c_str, c_str + strlen(c_str), boost::spirit::qi::ulong_, dest))
        throw utils::SaxParserError(
          _("fail to convert to unsigned integer `%s'"), c_str);

    return dest;
}

double
charToDouble(const char* str)
{
    auto c_str = reinterpret_cast<const char*>(str);
    double dest;

    if (!boost::spirit::qi::parse(
          c_str, c_str + strlen(c_str), boost::spirit::qi::double_, dest))
        throw utils::SaxParserError(_("fail to convert to real `%s'"), c_str);

    return dest;
}
}
} // namespace vle vpz
