/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
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

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/cast.hpp>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <libxml/SAX2.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <sstream>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/i18n.hpp>
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
#include <vle/vpz/SaxParser.hpp>
#include <vle/vpz/Vpz.hpp>

namespace vle {
namespace vpz {

struct XmlCompare {
    inline bool operator()(const xmlChar *s1, const xmlChar *s2) const
    {
        return xmlStrcmp(s1, s2) < 0;
    }
};

using startfunc = void (SaxParser::*)(const xmlChar **);
using endfunc = void (SaxParser::*)();
using StartFuncList = std::map<const xmlChar *, startfunc, XmlCompare>;
using EndFuncList = std::map<const xmlChar *, endfunc, XmlCompare>;

StartFuncList &starts()
{
    static StartFuncList ret = {
        {(const xmlChar *)"boolean", &SaxParser::onBoolean},
        {(const xmlChar *)"integer", &SaxParser::onInteger},
        {(const xmlChar *)"double", &SaxParser::onDouble},
        {(const xmlChar *)"string", &SaxParser::onString},
        {(const xmlChar *)"set", &SaxParser::onSet},
        {(const xmlChar *)"matrix", &SaxParser::onMatrix},
        {(const xmlChar *)"map", &SaxParser::onMap},
        {(const xmlChar *)"key", &SaxParser::onKey},
        {(const xmlChar *)"tuple", &SaxParser::onTuple},
        {(const xmlChar *)"table", &SaxParser::onTable},
        {(const xmlChar *)"xml", &SaxParser::onXML},
        {(const xmlChar *)"null", &SaxParser::onNull},
        {(const xmlChar *)"vle_project", &SaxParser::onVLEProject},
        {(const xmlChar *)"structures", &SaxParser::onStructures},
        {(const xmlChar *)"model", &SaxParser::onModel},
        {(const xmlChar *)"in", &SaxParser::onIn},
        {(const xmlChar *)"out", &SaxParser::onOut},
        {(const xmlChar *)"port", &SaxParser::onPort},
        {(const xmlChar *)"submodels", &SaxParser::onSubModels},
        {(const xmlChar *)"connections", &SaxParser::onConnections},
        {(const xmlChar *)"connection", &SaxParser::onConnection},
        {(const xmlChar *)"origin", &SaxParser::onOrigin},
        {(const xmlChar *)"destination", &SaxParser::onDestination},
        {(const xmlChar *)"dynamics", &SaxParser::onDynamics},
        {(const xmlChar *)"dynamic", &SaxParser::onDynamic},
        {(const xmlChar *)"experiment", &SaxParser::onExperiment},
        {(const xmlChar *)"conditions", &SaxParser::onConditions},
        {(const xmlChar *)"condition", &SaxParser::onCondition},
        {(const xmlChar *)"views", &SaxParser::onViews},
        {(const xmlChar *)"outputs", &SaxParser::onOutputs},
        {(const xmlChar *)"output", &SaxParser::onOutput},
        {(const xmlChar *)"view", &SaxParser::onView},
        {(const xmlChar *)"observables", &SaxParser::onObservables},
        {(const xmlChar *)"observable", &SaxParser::onObservable},
        {(const xmlChar *)"attachedview", &SaxParser::onAttachedView},
        {(const xmlChar *)"classes", &SaxParser::onClasses},
        {(const xmlChar *)"class", &SaxParser::onClass}};

    return ret;
}

EndFuncList &ends()
{
    static EndFuncList ret = {
        {(const xmlChar *)"boolean", &SaxParser::onEndBoolean},
        {(const xmlChar *)"integer", &SaxParser::onEndInteger},
        {(const xmlChar *)"double", &SaxParser::onEndDouble},
        {(const xmlChar *)"string", &SaxParser::onEndString},
        {(const xmlChar *)"set", &SaxParser::onEndSet},
        {(const xmlChar *)"matrix", &SaxParser::onEndMatrix},
        {(const xmlChar *)"map", &SaxParser::onEndMap},
        {(const xmlChar *)"key", &SaxParser::onEndKey},
        {(const xmlChar *)"tuple", &SaxParser::onEndTuple},
        {(const xmlChar *)"table", &SaxParser::onEndTable},
        {(const xmlChar *)"xml", &SaxParser::onEndXML},
        {(const xmlChar *)"null", &SaxParser::onEndNull},
        {(const xmlChar *)"vle_project", &SaxParser::onEndVLEProject},
        {(const xmlChar *)"structures", &SaxParser::onEndStructures},
        {(const xmlChar *)"model", &SaxParser::onEndModel},
        {(const xmlChar *)"in", &SaxParser::onEndIn},
        {(const xmlChar *)"out", &SaxParser::onEndOut},
        {(const xmlChar *)"port", &SaxParser::onEndPort},
        {(const xmlChar *)"submodels", &SaxParser::onEndSubModels},
        {(const xmlChar *)"connections", &SaxParser::onEndConnections},
        {(const xmlChar *)"connection", &SaxParser::onEndConnection},
        {(const xmlChar *)"origin", &SaxParser::onEndOrigin},
        {(const xmlChar *)"destination", &SaxParser::onEndDestination},
        {(const xmlChar *)"dynamics", &SaxParser::onEndDynamics},
        {(const xmlChar *)"dynamic", &SaxParser::onEndDynamic},
        {(const xmlChar *)"experiment", &SaxParser::onEndExperiment},
        {(const xmlChar *)"conditions", &SaxParser::onEndConditions},
        {(const xmlChar *)"condition", &SaxParser::onEndCondition},
        {(const xmlChar *)"views", &SaxParser::onEndViews},
        {(const xmlChar *)"outputs", &SaxParser::onEndOutputs},
        {(const xmlChar *)"output", &SaxParser::onEndOutput},
        {(const xmlChar *)"view", &SaxParser::onEndView},
        {(const xmlChar *)"observables", &SaxParser::onEndObservables},
        {(const xmlChar *)"observable", &SaxParser::onEndObservable},
        {(const xmlChar *)"attachedview", &SaxParser::onEndAttachedView},
        {(const xmlChar *)"classes", &SaxParser::onEndClasses},
        {(const xmlChar *)"class", &SaxParser::onEndClass}};

    return ret;
}

SaxParser::SaxParser(Vpz &vpz)
    : m_ctxt(nullptr)
    , m_vpzstack(vpz)
    , m_vpz(vpz)
    , m_isValue(false)
    , m_isVPZ(false)
{
}

struct scope_exit {
    std::function<void(void)> fn;

    scope_exit(std::function<void(void)> fn_)
        : fn(fn_)
    {
    }

    ~scope_exit() { fn(); }
};

void SaxParser::parse(std::istream &is, std::size_t size)
{
    std::vector<char> buffer(size == 0 ? 1024 : size);

    xmlSAXHandler sax;
    memset(&sax, 0, sizeof(xmlSAXHandler));
    sax.initialized = XML_SAX2_MAGIC;
    sax.startDocument = &SaxParser::onStartDocument;
    sax.endDocument = &SaxParser::onEndDocument;
    sax.startElement = &SaxParser::onStartElement;
    sax.endElement = &SaxParser::onEndElement;
    sax.characters = &SaxParser::onCharacters;
    sax.cdataBlock = &SaxParser::onCDataBlock;
    sax.warning = &SaxParser::onWarning;
    sax.error = &SaxParser::onError;
    sax.fatalError = &SaxParser::onFatalError;

    is.read(buffer.data(), 4);

    if (not is.good() or is.gcount() <= 0)
        throw utils::SaxParserError(_("Error parsing the 4 first bytes)"));

    xmlParserCtxtPtr ctx = xmlCreatePushParserCtxt(
        &sax, this, buffer.data(), is.gcount(), nullptr);
    m_ctxt = static_cast<void *>(ctx);

    scope_exit se([this, ctx]() {
        xmlFreeParserCtxt(ctx);
        m_ctxt = nullptr;
    });

    while (is.good() and not is.eof()) {
        is.read(buffer.data(), buffer.size());
        int err = xmlParseChunk(ctx, buffer.data(), is.gcount(), is.eof());

        if (err == 0)
            continue;

        if (err == XML_ERR_USER_STOP) {
            throw utils::SaxParserError(_("Error parsing: %s"),
                                        m_error.c_str());
        }
        else {
            xmlErrorPtr error = xmlCtxtGetLastError(ctx);
            if (error != nullptr) {
                int line = error->line;
                int column = error->int2;
                std::string msg = error->message;

                throw utils::SaxParserError(
                    _("Error parsing at %d:%d (internal error %d: %s"),
                    line,
                    column,
                    err,
                    msg.c_str());
            }
            else {
                throw utils::SaxParserError(
                    _("Error parsing (internal error %d"), err);
            }
        }
    }
}

void SaxParser::stopParser(const std::string &error)
{
    xmlParserCtxtPtr ctxt = static_cast<xmlParserCtxtPtr>(m_ctxt);

    xmlStopParser(ctxt);
    m_error.assign(error);
}

void SaxParser::parseFile(const std::string &filename)
{
    std::ifstream ifs(filename);
    if (not ifs.is_open())
        throw utils::SaxParserError(_("Error opening file `%s'"),
                                    filename.c_str());

    parse(ifs, BUFSIZ);
}

void SaxParser::parseMemory(const std::string &buffer)
{
    std::istringstream iss(buffer);

    parse(iss, BUFSIZ);
}

void SaxParser::onStartDocument(void *ctx)
{
    SaxParser *sax = static_cast<SaxParser *>(ctx);

    sax->m_error.clear();
    sax->m_vpzstack.clear();
    sax->m_valuestack.clear();
    sax->m_lastCharacters.clear();
    sax->m_cdata.clear();
    sax->m_vpz.clear();
    sax->m_isValue = false;
    sax->m_isVPZ = false;
}

void SaxParser::onEndDocument(void *ctx)
{
    SaxParser *sax = static_cast<SaxParser *>(ctx);

    if (sax->m_isVPZ == false) {
        if (not sax->m_valuestack.getResults().empty()) {
            sax->m_isValue = true;
        }
    }
}

void SaxParser::onStartElement(void *ctx,
                               const xmlChar *name,
                               const xmlChar **atts)
{
    SaxParser *sax = static_cast<SaxParser *>(ctx);

    sax->clearLastCharactersStored();

    auto it = starts().find(name);
    if (it != starts().end()) {
        try {
            (sax->*(it->second))(atts);
        }
        catch (const std::exception &e) {
            sax->stopParser(e.what());
        }
    }
    else {
        sax->stopParser(
            (fmt(_("Unknown tag '%1%'")) % (const char *)name).str());
    }
}

void SaxParser::onEndElement(void *ctx, const xmlChar *name)
{
    SaxParser *sax = static_cast<SaxParser *>(ctx);

    auto it = ends().find(name);
    if (it != ends().end()) {
        try {
            (sax->*(it->second))();
        }
        catch (const std::exception &e) {
            sax->stopParser(e.what());
        }
    }
    else {
        sax->stopParser(
            (fmt(_("Unknown end tag '%1%'")) % (const char *)name).str());
    }
}

void SaxParser::onCharacters(void *ctx, const xmlChar *ch, int len)
{
    SaxParser *sax = static_cast<SaxParser *>(ctx);

    std::string buf((const char *)ch, len);
    sax->addToCharacters(buf);
}

void SaxParser::onCDataBlock(void *ctx, const xmlChar *value, int len)
{
    SaxParser *sax = static_cast<SaxParser *>(ctx);

    std::string buf((const char *)value, len);
    sax->m_cdata.assign(buf);
}

void SaxParser::onWarning(void *ctx, const char *msg, ...)
{
    SaxParser *sax = static_cast<SaxParser *>(ctx);

    va_list args;
    va_start(args, msg);
    std::string buffer = utils::vformat(msg, args);
    va_end(args);

    sax->m_error = buffer;
}

void SaxParser::onError(void *ctx, const char *msg, ...)
{
    SaxParser *sax = static_cast<SaxParser *>(ctx);

    va_list args;
    va_start(args, msg);
    std::string buffer = utils::vformat(msg, args);
    va_end(args);

    sax->stopParser(buffer);
}

void SaxParser::onFatalError(void *ctx, const char *msg, ...)
{
    SaxParser *sax = static_cast<SaxParser *>(ctx);

    va_list args;
    va_start(args, msg);
    std::string buffer = vle::utils::vformat(msg, args);
    va_end(args);

    sax->stopParser(buffer);
}

//
//
//

void SaxParser::onBoolean(const xmlChar **)
{
    m_valuestack.pushBoolean();
}

void SaxParser::onInteger(const xmlChar **)
{
    m_valuestack.pushInteger();
}

void SaxParser::onDouble(const xmlChar **)
{
    m_valuestack.pushDouble();
}

void SaxParser::onString(const xmlChar **)
{
    m_valuestack.pushString();
}

void SaxParser::onSet(const xmlChar **)
{
    m_valuestack.pushSet();
}

void SaxParser::onMatrix(const xmlChar **att)
{
    const xmlChar *rows = nullptr;
    const xmlChar *columns = nullptr;
    const xmlChar *rowmax = nullptr;
    const xmlChar *columnmax = nullptr;
    const xmlChar *columnstep = nullptr;
    const xmlChar *rowstep = nullptr;

    for (int i = 0; att[i] != nullptr; i += 2) {
        if (xmlStrcmp(att[i], (const xmlChar *)"rows") == 0) {
            rows = att[i + 1];
        }
        else if (xmlStrcmp(att[i], (const xmlChar *)"columns") == 0) {
            columns = att[i + 1];
        }
        else if (xmlStrcmp(att[i], (const xmlChar *)"columnmax") == 0) {
            columnmax = att[i + 1];
        }
        else if (xmlStrcmp(att[i], (const xmlChar *)"rowmax") == 0) {
            rowmax = att[i + 1];
        }
        else if (xmlStrcmp(att[i], (const xmlChar *)"columnstep") == 0) {
            columnstep = att[i + 1];
        }
        else if (xmlStrcmp(att[i], (const xmlChar *)"rowstep") == 0) {
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
        typedef value::Matrix::index m_t;

        m_valuestack.pushMatrix(
            numeric_cast<m_t>(xmlCharToInt(columns)),
            numeric_cast<m_t>(xmlCharToInt(rows)),
            numeric_cast<m_t>(columnmax ? xmlCharToInt(columnmax) : 0),
            numeric_cast<m_t>(rowmax ? xmlCharToInt(rowmax) : 0),
            numeric_cast<m_t>(columnstep ? xmlCharToInt(columnstep) : 0),
            numeric_cast<m_t>(rowstep ? xmlCharToInt(rowstep) : 0));
    }
    catch (const std::exception &e) {
        throw utils::SaxParserError(
            (fmt(_("Matrix tag does not convert a attribute "
                   "'%1%'")) %
             e.what())
                .str());
    }
}

void SaxParser::onMap(const xmlChar **)
{
    m_valuestack.pushMap();
}

void SaxParser::onKey(const xmlChar **att)
{
    const xmlChar *name = nullptr;

    for (int i = 0; att[i] != nullptr; i += 2) {
        if (xmlStrcmp(att[i], (const xmlChar *)"name") == 0) {
            name = att[i + 1];
        }
    }

    if (not name) {
        throw utils::SaxParserError(
            _("Key value tag does not have attribute 'name'"));
    }

    m_valuestack.pushMapKey(xmlCharToString(name));
}

void SaxParser::onTuple(const xmlChar **)
{
    m_valuestack.pushTuple();
}

void SaxParser::onTable(const xmlChar **att)
{
    const xmlChar *width = nullptr;
    const xmlChar *height = nullptr;

    for (int i = 0; att[i] != nullptr; i += 2) {
        if (xmlStrcmp(att[i], (const xmlChar *)"width") == 0) {
            width = att[i + 1];
        }
        else if (xmlStrcmp(att[i], (const xmlChar *)"height") == 0) {
            height = att[i + 1];
        }
    }

    if (not width or not height) {
        throw utils::SaxParserError(_("Table value tag does not have "
                                      "attributes 'width' or 'height'"));
    }

    try {
        m_valuestack.pushTable(xmlCharToInt(width), xmlCharToInt(height));
    }
    catch (const std::exception &e) {
        throw utils::SaxParserError(
            (fmt(_("Table value tag can not convert attributes "
                   "'width' or "

                   "'height': %1%")) %
             e.what())
                .str());
    }
}

void SaxParser::onXML(const xmlChar **)
{
    m_valuestack.pushXml();
}

void SaxParser::onNull(const xmlChar **)
{
    m_valuestack.pushNull();
}

void SaxParser::onVLEProject(const xmlChar **att)
{
    assert(not m_isValue);

    m_isVPZ = true;
    m_vpzstack.pushVpz(att);
}

void SaxParser::onStructures(const xmlChar **)
{
    m_vpzstack.pushStructure();
}

void SaxParser::onModel(const xmlChar **att)
{
    m_vpzstack.pushModel(att);
}

void SaxParser::onIn(const xmlChar **)
{
    m_vpzstack.pushPortType("in");
}

void SaxParser::onOut(const xmlChar **)
{
    m_vpzstack.pushPortType("out");
}

void SaxParser::onPort(const xmlChar **att)
{
    m_vpzstack.pushPort(att);
}

void SaxParser::onSubModels(const xmlChar **)
{
    m_vpzstack.pushSubModels();
}

void SaxParser::onConnections(const xmlChar **)
{
    m_vpzstack.pushConnections();
}

void SaxParser::onConnection(const xmlChar **att)
{
    m_vpzstack.pushConnection(att);
}

void SaxParser::onOrigin(const xmlChar **att)
{
    m_vpzstack.pushOrigin(att);
}

void SaxParser::onDestination(const xmlChar **att)
{
    m_vpzstack.pushDestination(att);
}

void SaxParser::onDynamics(const xmlChar **)
{
    m_vpzstack.pushDynamics();
}

void SaxParser::onDynamic(const xmlChar **att)
{
    m_vpzstack.pushDynamic(att);
}

void SaxParser::onExperiment(const xmlChar **att)
{
    m_vpzstack.pushExperiment(att);
}

void SaxParser::onConditions(const xmlChar **)
{
    m_vpzstack.pushConditions();
}

void SaxParser::onCondition(const xmlChar **att)
{
    m_vpzstack.pushCondition(att);
}

void SaxParser::onViews(const xmlChar **)
{
    m_vpzstack.pushViews();
}

void SaxParser::onOutputs(const xmlChar **)
{
    m_vpzstack.pushOutputs();
}

void SaxParser::onOutput(const xmlChar **att)
{
    m_vpzstack.pushOutput(att);
}

void SaxParser::onView(const xmlChar **att)
{
    m_vpzstack.pushView(att);
}

void SaxParser::onObservables(const xmlChar **)
{
    m_vpzstack.pushObservables();
}

void SaxParser::onObservable(const xmlChar **att)
{
    m_vpzstack.pushObservable(att);
}

void SaxParser::onAttachedView(const xmlChar **att)
{
    m_vpzstack.pushAttachedView(att);
}

void SaxParser::onClasses(const xmlChar **)
{
    m_vpzstack.pushClasses();
}

void SaxParser::onClass(const xmlChar **att)
{
    m_vpzstack.pushClass(att);
}

void SaxParser::onEndBoolean()
{
    m_valuestack.pushOnVectorValue<value::Boolean>(
        xmlCharToBoolean(((xmlChar *)lastCharactersStored().c_str())));
}

void SaxParser::onEndInteger()
{
    m_valuestack.pushOnVectorValue<value::Integer>(
        xmlCharToInt(((xmlChar *)lastCharactersStored().c_str())));
}

void SaxParser::onEndDouble()
{
    m_valuestack.pushOnVectorValue<value::Double>(xmlXPathCastStringToNumber(
        ((xmlChar *)lastCharactersStored().c_str())));
}

void SaxParser::onEndString()
{
    m_valuestack.pushOnVectorValue<value::String>(lastCharactersStored());
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
    value::Tuple &tuple(m_valuestack.topValue()->toTuple());

    std::string cpy(lastCharactersStored());
    boost::algorithm::trim(cpy);

    std::vector<std::string> result;
    boost::algorithm::split(
        result, cpy, boost::algorithm::is_any_of(" \n\t\r"));

    for (auto &elem : result) {
        boost::algorithm::trim(elem);
        if (not(elem).empty()) {
            tuple.add(
                xmlXPathCastStringToNumber((const xmlChar *)((elem).c_str())));
        }
    }

    m_valuestack.popValue();
}

void SaxParser::onEndTable()
{
    value::Table &table(m_valuestack.topValue()->toTable());

    std::string cpy(lastCharactersStored());
    boost::algorithm::trim(cpy);

    std::vector<std::string> result;
    boost::algorithm::split(
        result, cpy, boost::algorithm::is_any_of(" \n\t\r"));

    size_t size;
    try {
        size = boost::numeric_cast<size_t>(table.width() * table.height());
    }
    catch (const std::exception /*e*/) {
        throw utils::SaxParserError(
            (fmt(_("VPZ parser: bad height (%1%) or width (%2%) "
                   "table ")) %
             table.width() % table.height())
                .str());
    }

    if (result.size() != size) {
        throw utils::SaxParserError(_("VPZ parser: bad height or width "
                                      "for number of real in table"));
    }

    value::Table::index i = 0;
    value::Table::index j = 0;
    for (auto &elem : result) {
        boost::algorithm::trim(elem);
        if (not(elem).empty()) {
            table.get(i, j) =
                xmlXPathCastStringToNumber((const xmlChar *)((elem).c_str()));
            if (i + 1 >= table.width()) {
                i = 0;
                if (j + 1 >= table.height()) {
                    break;
                }
                else {
                    j++;
                }
            }
            else {
                i++;
            }
        }
    }

    m_valuestack.popValue();
}

void SaxParser::onEndXML()
{
    m_valuestack.pushOnVectorValue<value::Xml>(m_cdata);
    m_cdata.clear();
}

void SaxParser::onEndNull()
{
    m_valuestack.pushOnVectorValue<value::Null>();
}

void SaxParser::onEndPort()
{
    if (m_vpzstack.top()->isCondition()) {
        auto &vals = m_vpzstack.popConditionPort();
        auto &lst = getValues();

        for (auto &elem : lst)
            vals.emplace_back(elem);

        m_valuestack.clear();
    }
    else if (m_vpzstack.top()->isObservablePort()) {
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
        Output *out = dynamic_cast<Output *>(m_vpzstack.top());
        auto &lst = getValues();
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

void SaxParser::onEndClasses()
{
    m_vpzstack.popClasses();
}

void SaxParser::onEndClass()
{
    m_vpzstack.popClass();
}

const std::vector<std::shared_ptr<value::Value>> &SaxParser::getValues() const
{
    return m_valuestack.getResults();
}

std::vector<std::shared_ptr<value::Value>> &SaxParser::getValues()
{
    return m_valuestack.getResults();
}

const std::shared_ptr<value::Value> &
SaxParser::getValue(const size_t pos) const
{
    return m_valuestack.getResult(pos);
}

bool xmlCharToBoolean(const xmlChar *str)
{
    if ((xmlStrncmp(str, (const xmlChar *)"true", 4) == 0) or
        (xmlStrncmp(str, (const xmlChar *)"1", 1) == 0)) {
        return true;
    }

    return false;
}

long int xmlCharToInt(const xmlChar *str)
{
    return (long int)xmlXPathCastStringToNumber(str);
}

unsigned long int xmlCharToUnsignedInt(const xmlChar *str)
{
    return (unsigned long int)xmlXPathCastStringToNumber(str);
}
}
} // namespace vle vpz
