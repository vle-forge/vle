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


#include <vle/vpz/SaxParser.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/vpz/BaseModel.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/String.hpp>
#include <vle/value/XML.hpp>
#include <vle/value/Null.hpp>
#include <vle/utils/i18n.hpp>
#include <boost/cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <libxml/SAX2.h>
#include <libxml/parser.h>
#include <cerrno>
#include <cstring>

namespace vle { namespace vpz {

SaxParser::SaxParser(Vpz& vpz)
    : m_stop(false), m_vpzstack(vpz), m_vpz(vpz), m_isValue(false),
    m_isVPZ(false)
{
    fillTagList();
}

void SaxParser::parseFile(const std::string& filename)
{
    setlocale(LC_ALL, "C");
    memset(&m_sax, 0, sizeof(xmlSAXHandler));
    m_sax.initialized = XML_SAX2_MAGIC;
    m_sax.startDocument = &SaxParser::onStartDocument;
    m_sax.endDocument = &SaxParser::onEndDocument;
    m_sax.startElement = &SaxParser::onStartElement;
    m_sax.endElement = &SaxParser::onEndElement;
    m_sax.characters = &SaxParser::onCharacters;
    m_sax.cdataBlock = &SaxParser::onCDataBlock;
    m_sax.warning = &SaxParser::onWarning;
    m_sax.error = &SaxParser::onError;
    m_sax.fatalError = &SaxParser::onFatalError;

    if (xmlSAXUserParseFile(&m_sax, this, filename.c_str())) {
        if (m_error.empty()) {
            throw utils::SaxParserError(fmt(
                    _("Error parsing file '%1%'")) % filename);
        } else {
            throw utils::SaxParserError(fmt(
                    _("Error parsing file '%1%': %2%")) % filename % m_error);
        }
    }

    if (m_stop) {
        throw utils::SaxParserError(fmt(
                _("Error when parsing file '%1%': %2%")) % filename % m_error);
    }

    xmlMemoryDump();
    setlocale(LC_ALL, "");
}

void SaxParser::parseMemory(const std::string& buffer)
{
    setlocale(LC_ALL, "C");
    memset(&m_sax, 0, sizeof(xmlSAXHandler));
    m_sax.initialized = XML_SAX2_MAGIC;
    m_sax.startDocument = &SaxParser::onStartDocument;
    m_sax.endDocument = &SaxParser::onEndDocument;
    m_sax.startElement = &SaxParser::onStartElement;
    m_sax.endElement = &SaxParser::onEndElement;
    m_sax.characters = &SaxParser::onCharacters;
    m_sax.cdataBlock = &SaxParser::onCDataBlock;
    m_sax.warning = &SaxParser::onWarning;
    m_sax.error = &SaxParser::onError;
    m_sax.fatalError = &SaxParser::onFatalError;

    if (xmlSAXUserParseMemory(&m_sax, this, buffer.c_str(), buffer.size())) {
        if (m_error.empty()) {
            throw utils::SaxParserError(_("Error parsing memory"));
        } else {
            throw utils::SaxParserError(fmt(
                    _("Error parsing memory: %1%")) % m_error);
        }
    }

    if (m_stop) {
        throw utils::SaxParserError(fmt(
                _("Error when parsing memory: %1%")) % m_error);
    }
    xmlMemoryDump();
    setlocale(LC_ALL, "");
}

void SaxParser::stopParser(const std::string& error)
{
    m_error.assign(error);
    m_stop = true;
}

void SaxParser::clearParserState()
{
    m_vpzstack.clear();
    m_valuestack.clear();
    m_lastCharacters.clear();
    m_isValue = false;
    m_isVPZ = false;
    m_stop = false;
}

void SaxParser::onStartDocument(void* ctx)
{
    SaxParser* sax = static_cast < SaxParser* >(ctx);

    sax->clearParserState();
}

void SaxParser::onEndDocument(void* ctx)
{
    SaxParser* sax = static_cast < SaxParser* >(ctx);

    if (sax->m_isVPZ == false) {
        if (not sax->m_valuestack.getResults().empty()) {
            sax->m_isValue = true;
        }
    }
}

void SaxParser::onStartElement(void* ctx, const xmlChar* name,
                               const xmlChar** atts)
{
    SaxParser* sax = static_cast < SaxParser* >(ctx);

    if (not sax->isStopped()) {
        sax->clearLastCharactersStored();
        StartFuncList::iterator it = sax->m_starts.find(name);
        if (it != sax->m_starts.end()) {
            try {
                (sax->*(it->second))(atts);
            } catch (const std::exception& e) {
                sax->stopParser(e.what());
            }
        } else {
            sax->stopParser((fmt(_("Unknow tag '%1%'")) %
                             (const char*)name).str());
        }
    }
}

void SaxParser::onEndElement(void* ctx, const xmlChar* name)
{
    SaxParser* sax = static_cast < SaxParser* >(ctx);

    if (not sax->isStopped()) {
        EndFuncList::iterator it = sax->m_ends.find(name);
        if (it != sax->m_ends.end()) {
            try {
                (sax->*(it->second))();
            } catch (const std::exception& e) {
                sax->stopParser(e.what());
            }
        } else {
            sax->stopParser((fmt(_("Unknow end tag '%1%'")) %
                             (const char*)name).str());
        }
    }
}

void SaxParser::onCharacters(void* ctx, const xmlChar* ch, int len)
{
    SaxParser* sax = static_cast < SaxParser* >(ctx);

    if (not sax->isStopped()) {
        std::string buf((const char*)ch, len);

        sax->addToCharacters(buf);
    }
}


void SaxParser::onCDataBlock(void* ctx, const xmlChar* value, int len)
{
    SaxParser* sax = static_cast < SaxParser* >(ctx);

    if (not sax->isStopped()) {
        std::string buf((const char*)value, len);

        sax->m_cdata.assign(buf);
    }
}

void SaxParser::onWarning(void* /* ctx */, const char *msg, ...)
{
    char* buffer = new char[1024];
    memset(buffer, 0, 1024);

    va_list args;

    va_start(args, msg);
    vsnprintf(buffer, 1023, msg, args);
    va_end(args);

    TraceAlways(fmt(_("XML warning: %1%")) % buffer);

    delete [] buffer;
}

void SaxParser::onError(void* ctx, const char *msg, ...)
{
    setlocale(LC_ALL, "");

    SaxParser* sax = static_cast < SaxParser* >(ctx);
    char* buffer = new char[1024];
    memset(buffer, 0, 1024);

    va_list args;
    va_start(args, msg);
    vsnprintf(buffer, 1023, msg, args);
    va_end(args);

    sax->stopParser(buffer);
    delete [] buffer;
}

void SaxParser::onFatalError(void* ctx, const char *msg, ...)
{
    setlocale(LC_ALL, "");

    SaxParser* sax = static_cast < SaxParser* >(ctx);
    char* buffer = new char[1024];
    memset(buffer, 0, 1024);

    va_list args;

    va_start(args, msg);
    vsnprintf(buffer, 1023, msg, args);
    va_end(args);

    sax->stopParser(buffer);
    delete [] buffer;
}

//
//
//

void SaxParser::onBoolean(const xmlChar**)
{
    m_valuestack.pushBoolean();
}

void SaxParser::onInteger(const xmlChar**)
{
    m_valuestack.pushInteger();
}

void SaxParser::onDouble(const xmlChar**)
{
    m_valuestack.pushDouble();
}

void SaxParser::onString(const xmlChar**)
{
    m_valuestack.pushString();
}

void SaxParser::onSet(const xmlChar**)
{
    m_valuestack.pushSet();
}

void SaxParser::onMatrix(const xmlChar** att)
{
    const xmlChar* rows = 0;
    const xmlChar* columns = 0;
    const xmlChar* rowmax = 0;
    const xmlChar* columnmax = 0;
    const xmlChar* columnstep = 0;
    const xmlChar* rowstep = 0;

    for (int i = 0; att[i] != 0; i += 2) {
        if (xmlStrcmp(att[i], (const xmlChar*)"rows") == 0) {
            rows = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"columns") == 0) {
            columns = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"columnmax") == 0) {
            columnmax = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"rowmax") == 0) {
            rowmax = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"columnstep") == 0) {
            columnstep = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"rowstep") == 0) {
            rowstep = att[i + 1];
        }
    }

    if (not rows or not columns) {
        throw utils::SaxParserError(
            _("Matrix tag does not have 'row' or 'column' attribute"));
    }

    try {
        using boost::numeric_cast;
        typedef value::Matrix::index m_t;

        m_valuestack.pushMatrix(
            numeric_cast < m_t >(xmlCharToInt(columns)),
            numeric_cast < m_t >(xmlCharToInt(rows)),
            numeric_cast < m_t >(columnmax ? xmlCharToInt(columnmax) : 0),
            numeric_cast < m_t >(rowmax ? xmlCharToInt(rowmax) : 0),
            numeric_cast < m_t >(columnstep ? xmlCharToInt(columnstep) : 0),
            numeric_cast < m_t >(rowstep ? xmlCharToInt(rowstep) : 0));
    } catch(const std::exception& e) {
        throw utils::SaxParserError(fmt(
            _("Matrix tag does not convert a attribute '%1%'")) % e.what());
    }
}

void SaxParser::onMap(const xmlChar**)
{
    m_valuestack.pushMap();
}

void SaxParser::onKey(const xmlChar** att)
{
    const xmlChar* name = 0;

    for (int i = 0; att[i] != 0; i += 2) {
        if (xmlStrcmp(att[i], (const xmlChar*)"name") == 0) {
            name = att[i + 1];
        }
    }

    if (not name) {
        throw utils::SaxParserError(
            _("Key value tag does not have attribute 'name'"));
    }

    m_valuestack.pushMapKey(xmlCharToString(name));
}

void SaxParser::onTuple(const xmlChar**)
{
    m_valuestack.pushTuple();
}

void SaxParser::onTable(const xmlChar** att)
{
    const xmlChar* width = 0;
    const xmlChar* height = 0;

    for (int i = 0; att[i] != 0; i += 2) {
        if (xmlStrcmp(att[i], (const xmlChar*)"width") == 0) {
            width = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"height") == 0) {
            height = att[i + 1];
        }
    }

    if (not width or not height) {
        throw utils::SaxParserError(
            _("Table value tag does not have attributes 'width' or 'height'"));
    }

    try {
        m_valuestack.pushTable(xmlCharToInt(width), xmlCharToInt(height));
    } catch (const std::exception& e) {
        throw utils::SaxParserError(fmt(
            _("Table value tag can not convert attributes 'width' or "
              "'height': %1%")) % e.what());
    }
}

void SaxParser::onXML(const xmlChar**)
{
    m_valuestack.pushXml();
}

void SaxParser::onNull(const xmlChar**)
{
    m_valuestack.pushNull();
}

void SaxParser::onVLEProject(const xmlChar** att)
{
    if (m_isValue) {
        throw utils::SaxParserError();
    }

    m_isVPZ = true;
    m_vpzstack.pushVpz(att);
}

void SaxParser::onStructures(const xmlChar**)
{
    m_vpzstack.pushStructure();
}

void SaxParser::onModel(const xmlChar** att)
{
    m_vpzstack.pushModel(att);
}

void SaxParser::onIn(const xmlChar**)
{
    m_vpzstack.pushPortType("in");
}

void SaxParser::onOut(const xmlChar**)
{
    m_vpzstack.pushPortType("out");
}

void SaxParser::onPort(const xmlChar** att)
{
    m_vpzstack.pushPort(att);
}

void SaxParser::onSubModels(const xmlChar**)
{
    m_vpzstack.pushSubModels();
}

void SaxParser::onConnections(const xmlChar**)
{
    m_vpzstack.pushConnections();
}

void SaxParser::onConnection(const xmlChar** att)
{
    m_vpzstack.pushConnection(att);
}

void SaxParser::onOrigin(const xmlChar** att)
{
    m_vpzstack.pushOrigin(att);
}

void SaxParser::onDestination(const xmlChar** att)
{
    m_vpzstack.pushDestination(att);
}

void SaxParser::onDynamics(const xmlChar**)
{
    m_vpzstack.pushDynamics();
}

void SaxParser::onDynamic(const xmlChar** att)
{
    m_vpzstack.pushDynamic(att);
}

void SaxParser::onExperiment(const xmlChar** att)
{
    m_vpzstack.pushExperiment(att);
}

void SaxParser::onConditions(const xmlChar**)
{
    m_vpzstack.pushConditions();
}

void SaxParser::onCondition(const xmlChar** att)
{
    m_vpzstack.pushCondition(att);
}

void SaxParser::onViews(const xmlChar**)
{
    m_vpzstack.pushViews();
}

void SaxParser::onOutputs(const xmlChar**)
{
    m_vpzstack.pushOutputs();
}

void SaxParser::onOutput(const xmlChar** att)
{
    m_vpzstack.pushOutput(att);
}

void SaxParser::onView(const xmlChar** att)
{
    m_vpzstack.pushView(att);
}

void SaxParser::onObservables(const xmlChar**)
{
    m_vpzstack.pushObservables();
}

void SaxParser::onObservable(const xmlChar** att)
{
    m_vpzstack.pushObservable(att);
}

void SaxParser::onAttachedView(const xmlChar** att)
{
    m_vpzstack.pushAttachedView(att);
}

void SaxParser::onClasses(const xmlChar**)
{
    m_vpzstack.pushClasses();
}

void SaxParser::onClass(const xmlChar** att)
{
    m_vpzstack.pushClass(att);
}

void SaxParser::onEndBoolean()
{
    m_valuestack.pushOnVectorValue(
        value::Boolean::create(
            xmlCharToBoolean(
                ((xmlChar*)lastCharactersStored().c_str()))));
}

void SaxParser::onEndInteger()
{
    m_valuestack.pushOnVectorValue(
        value::Integer::create(
            xmlCharToInt(
                ((xmlChar*)lastCharactersStored().c_str()))));
}

void SaxParser::onEndDouble()
{
    m_valuestack.pushOnVectorValue(
        value::Double::create(
            xmlCharToDouble(
                ((xmlChar*)lastCharactersStored().c_str()))));
}

void SaxParser::onEndString()
{
    m_valuestack.pushOnVectorValue(
        value::String::create(lastCharactersStored()));
}

void SaxParser::onEndKey()
{
}

void SaxParser::onEndSet()
{
    m_valuestack.popValue();
}

void SaxParser::onEndMatrix()
{
    m_valuestack.popValue();
}

void SaxParser::onEndMap()
{
    m_valuestack.popValue();
}

void SaxParser::onEndTuple()
{
    value::Tuple& tuple(m_valuestack.topValue()->toTuple());

    std::string cpy(lastCharactersStored());
    boost::algorithm::trim(cpy);

    std::vector < std::string > result;
    boost::algorithm::split(result, cpy,
                            boost::algorithm::is_any_of(" \n\t\r"));

    for (std::vector < std::string >::iterator it = result.begin();
         it != result.end(); ++it) {
        boost::algorithm::trim(*it);
        if (not (*it).empty()) {
            tuple.add(xmlCharToDouble((const xmlChar*)((*it).c_str())));
        }
    }

    m_valuestack.popValue();
}

void SaxParser::onEndTable()
{
    value::Table& table(m_valuestack.topValue()->toTable());

    std::string cpy(lastCharactersStored());
    boost::algorithm::trim(cpy);

    std::vector < std::string > result;
    boost::algorithm::split(result, cpy,
                            boost::algorithm::is_any_of(" \n\t\r"));

    size_t size;
    try {
        size = boost::numeric_cast < size_t >(table.width() * table.height());
    } catch (const std::exception /*e*/) {
        throw utils::SaxParserError(
            fmt(_("VPZ parser: bad height (%1%) or width (%2%) table ")) %
            table.width() % table.height());
    }

    if (result.size() != size) {
        throw utils::SaxParserError(
            _("VPZ parser: bad height or width for number of real in table"));
    }

    value::Table::index i = 0;
    value::Table::index j = 0;
    for (std::vector < std::string >::iterator it = result.begin(); it !=
         result.end(); ++it) {
        boost::algorithm::trim(*it);
        if (not (*it).empty()) {
            table.get(i, j) = xmlCharToDouble((const xmlChar*)((*it).c_str()));
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

void SaxParser::onEndXML()
{
    m_valuestack.pushOnVectorValue(value::Xml::create(m_cdata));
    m_cdata.clear();
}

void SaxParser::onEndNull()
{
    m_valuestack.pushOnVectorValue(value::Null::create());
}

void SaxParser::onEndPort()
{
    if (m_vpzstack.top()->isCondition()) {
        value::Set& vals(m_vpzstack.popConditionPort());
        std::vector < value::Value* >& lst(getValues());
        for (std::vector < value::Value* >::iterator it =
             lst.begin(); it != lst.end(); ++it) {
            vals.add(*it);
        }
        m_valuestack.clear();
    } else if (m_vpzstack.top()->isObservablePort()) {
        m_vpzstack.pop();
    }
}

void SaxParser::onEndIn()
{
    delete m_vpzstack.pop();
}

void SaxParser::onEndOut()
{
    delete m_vpzstack.pop();
}

void SaxParser::onEndStructures()
{
    delete m_vpzstack.pop();
}

void SaxParser::onEndSubModels()
{
    delete m_vpzstack.pop();
}

void SaxParser::onEndModel()
{
    delete m_vpzstack.pop();
}

void SaxParser::onEndConnections()
{
    delete m_vpzstack.pop();
}

void SaxParser::onEndConnection()
{
}

void SaxParser::onEndVLEProject()
{
    m_vpzstack.pop();
}

void SaxParser::onEndConditions()
{
    m_vpzstack.pop();
}

void SaxParser::onEndCondition()
{
    m_vpzstack.pop();
}

void SaxParser::onEndOutputs()
{
    m_vpzstack.pop();
}

void SaxParser::onEndDynamics()
{
    m_vpzstack.pop();
}

void SaxParser::onEndViews()
{
    m_vpzstack.pop();
}

void SaxParser::onEndObservables()
{
    m_vpzstack.pop();
}

void SaxParser::onEndObservable()
{
    m_vpzstack.pop();
}

void SaxParser::onEndExperiment()
{
    m_vpzstack.pop();
}

void SaxParser::onEndDestination()
{
    m_vpzstack.buildConnection();
}

void SaxParser::onEndView()
{
}

void SaxParser::onEndOrigin()
{
}

void SaxParser::onEndReplicas()
{
}

void SaxParser::onEndAttachedView()
{
}

void SaxParser::onEndDynamic()
{
}

void SaxParser::onEndOutput()
{
    if (m_vpzstack.top()->isOutput()) {
        Output* out = dynamic_cast < Output* >(m_vpzstack.top());

        std::vector < value::Value* >& lst(getValues());
        if (not lst.empty()) {
            if (lst.size() > 1) {
                throw utils::SaxParserError(
                    _("VPZ parser: multiples values for output"));
            }
            value::Value* val = lst[0];
            if (val) {
                out->setData(val);
                m_valuestack.clear();
            }
        }
        m_vpzstack.popOutput();
    }
}

void SaxParser::onEndClasses()
{
    m_vpzstack.popClasses();
}

void SaxParser::onEndClass()
{
    m_vpzstack.popClass();
}

const std::vector < value::Value* >& SaxParser::getValues() const
{
    return m_valuestack.getResults();
}

std::vector < value::Value* >& SaxParser::getValues()
{
    return m_valuestack.getResults();
}

value::Value* SaxParser::getValue(const size_t pos) const
{
    return m_valuestack.getResult(pos);
}

bool xmlCharToBoolean(const xmlChar* str)
{
    if ((xmlStrncmp(str, (const xmlChar*)"true", 4) == 0) or
        (xmlStrncmp(str, (const xmlChar*)"1", 1) == 0)) {
        return true;
    }

    return false;
}

long int xmlCharToInt(const xmlChar* str)
{
    char* res = 0;

    errno = 0;
    long int r = strtol((const char*)str, &res, 10);
    int err = errno;

    if ((err == ERANGE and (r == LONG_MAX || r == LONG_MIN)) or
        (err != 0 and r == 0)) {
        throw utils::SaxParserError(fmt(
                _("error to convert '%1%' to integer: %2%")) % str %
            strerror(err));
    }

    if (res == (const char*)str) {
        throw utils::SaxParserError(fmt(
                _("error to convert '%1%' to integer")) % str);
    }

    return r;
}

unsigned long int xmlCharToUnsignedInt(const xmlChar* str)
{
    char* res = 0;

    errno = 0;
    unsigned long int r = strtoul((const char*)str, &res, 10);
    int err = errno;

    if ((err == ERANGE and (r == ULONG_MAX)) or
        (err != 0 and r == 0)) {
        throw utils::SaxParserError(fmt(
                _("error to convert '%1%' to unsigned integer: %2%")) % str %
            strerror(err));
    }

    if (res == (const char*)str) {
        throw utils::SaxParserError(fmt(
                _("error to convert '%1%' to unsigned integer")) % str);
    }

    return r;
}

double xmlCharToDouble(const xmlChar* str)
{
    char* res = 0;

    errno = 0;
    double r = strtod((const char*)str, &res);
    int err = errno;

    if ((err == ERANGE and r == HUGE_VAL) or (err != 0 and r == 0)) {
        throw utils::SaxParserError(fmt(
                _("error to convert '%1%' to double: %2%")) % str %
            strerror(err));
    }

    if (res == (const char*)str) {
        throw utils::SaxParserError(fmt(
                _("error to convert '%1%' to double")) % str);
    }

    return r;
}

}} // namespace vle vpz
