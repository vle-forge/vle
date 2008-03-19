/**
 * @file src/vle/vpz/SaxParser.cpp
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




#include <vle/vpz/SaxParser.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/vpz/Model.hpp>
#include <vle/vpz/ParameterTrame.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Debug.hpp>
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

namespace vle { namespace vpz {

SaxParser::SaxParser(Vpz& vpz) :
    m_vpzstack(vpz),
    m_vpz(vpz),
    m_isValue(false),
    m_isVPZ(false),
    m_isTrame(false),
    m_isEndTrame(false)
{
    fillTagList();
}

SaxParser::~SaxParser()
{
}

void SaxParser::clearParserState()
{
    m_valuestack.clear();
    m_lastCharacters.clear();
    m_isValue = false;
    m_isVPZ = false;
    m_isTrame = false;
    m_isEndTrame = false;
}

void SaxParser::on_start_document()
{
    clearParserState();
}

void SaxParser::on_end_document()
{
    if (m_isVPZ == false) {
        if (not m_vpzstack.trame().empty()) {
            m_isTrame = true;
        } else {
            if (not m_valuestack.getResults().empty()) {
                m_isValue = true;
            }
        }
    }
}

void SaxParser::on_start_element(
    const Glib::ustring& name,
    const AttributeList& att)
{
    clearLastCharactersStored();

    StartFuncList::iterator it = m_starts.find(name);
    Assert(utils::SaxParserError, it != m_starts.end(), boost::format(
            "Unknow element %1%") % name);

    (this->*(it->second))(att);
}

void SaxParser::onBoolean(const AttributeList&)
{
    m_valuestack.pushBoolean();
}

void SaxParser::onInteger(const AttributeList&)
{
    m_valuestack.pushInteger();
}

void SaxParser::onDouble(const AttributeList&)
{
    m_valuestack.pushDouble();
}

void SaxParser::onString(const AttributeList&)
{
    m_valuestack.pushString();
}

void SaxParser::onSet(const AttributeList&)
{
    m_valuestack.pushSet();
}

void SaxParser::onMatrix(const AttributeList& lst)
{
    value::MatrixFactory::index row = 0, rowmax = 0, rowstep = 0;
    value::MatrixFactory::index col = 0, colmax = 0, colstep = 0;
    bool haverow = false;
    bool havecol = false;

    for (AttributeList::const_iterator it = lst.begin();
         it != lst.end(); ++it) {
        using boost::lexical_cast;;
        if (it->name == "rows") {
            haverow = true;
            row = lexical_cast < value::MatrixFactory::index >(it->value);
        } else if (it->name == "columns") {
            havecol = true;
            col = lexical_cast < value::MatrixFactory::index >(it->value);
        } else if (it->name == "rowmax") {
            rowmax = lexical_cast < value::MatrixFactory::index >(it->value);
        } else if (it->name == "columnmax") {
            colmax = lexical_cast < value::MatrixFactory::index >(it->value);
        } else if (it->name == "columnstep") {
            colstep = lexical_cast < value::MatrixFactory::index >(it->value);
        } else if (it->name == "rowstep") {
            rowstep = lexical_cast < value::MatrixFactory::index >(it->value);
        }
    }

    Assert(utils::SaxParserError, haverow and havecol,
            "Matrix value have no row or column attribute");

    m_valuestack.pushMatrix(col, row, colmax, rowmax, colstep, rowstep);
}

void SaxParser::onMap(const AttributeList&)
{
    m_valuestack.pushMap();
}

void SaxParser::onKey(const AttributeList& att)
{
    m_valuestack.pushMapKey(getAttribute < Glib::ustring >(att, "name"));
}

void SaxParser::onTuple(const AttributeList&)
{
    m_valuestack.pushTuple();
}

void SaxParser::onTable(const AttributeList& att)
{
    m_valuestack.pushTable(
        getAttribute < value::TableFactory::index >(att, "width"),
        getAttribute < value::TableFactory::index >(att, "height"));
}

void SaxParser::onXML(const AttributeList&)
{
    m_valuestack.pushXml();
}

void SaxParser::onNull(const AttributeList&)
{
    m_valuestack.pushNull();
}

void SaxParser::onVLETrame(const AttributeList&)
{
    m_isEndTrame = false;
    m_vpzstack.pushVleTrame();
}

void SaxParser::onTrame(const AttributeList& att)
{
    m_vpzstack.pushTrame(att);
}

void SaxParser::onModelTrame(const AttributeList& att)
{
    m_vpzstack.pushModelTrame(att);
}

void SaxParser::onVLEProject(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_isValue and not m_isTrame);
    m_isVPZ = true;
    m_vpzstack.pushVpz(att);
}

void SaxParser::onStructures(const AttributeList&)
{
    m_vpzstack.pushStructure();
}

void SaxParser::onModel(const AttributeList& att)
{
    m_vpzstack.pushModel(att);
}

void SaxParser::onIn(const AttributeList&)
{
    m_vpzstack.pushPortType("in");
}

void SaxParser::onOut(const AttributeList&)
{
    m_vpzstack.pushPortType("out");
}

void SaxParser::onPort(const AttributeList& att)
{
    m_vpzstack.pushPort(att);
}

void SaxParser::onSubModels(const AttributeList&)
{
    m_vpzstack.pushSubModels();
}

void SaxParser::onConnections(const AttributeList&)
{
    m_vpzstack.pushConnections();
}

void SaxParser::onConnection(const AttributeList& att)
{
    m_vpzstack.pushConnection(att);
}

void SaxParser::onOrigin(const AttributeList& att)
{
    m_vpzstack.pushOrigin(att);
}

void SaxParser::onDestination(const AttributeList& att)
{
    m_vpzstack.pushDestination(att);
}

void SaxParser::onDynamics(const AttributeList&)
{
    m_vpzstack.pushDynamics();
}

void SaxParser::onDynamic(const AttributeList& att)
{
    m_vpzstack.pushDynamic(att);
}

void SaxParser::onExperiment(const AttributeList& att)
{
    m_vpzstack.pushExperiment(att);
}

void SaxParser::onReplicas(const AttributeList& att)
{
    m_vpzstack.pushReplicas(att);
}

void SaxParser::onConditions(const AttributeList&)
{
    m_vpzstack.pushConditions();
}

void SaxParser::onCondition(const AttributeList& att)
{
    m_vpzstack.pushCondition(att);
}

void SaxParser::onViews(const AttributeList&)
{
    m_vpzstack.pushViews();
}

void SaxParser::onOutputs(const AttributeList&)
{
    m_vpzstack.pushOutputs();
}

void SaxParser::onOutput(const AttributeList& att)
{
    m_vpzstack.pushOutput(att);
}

void SaxParser::onView(const AttributeList& att)
{
    m_vpzstack.pushView(att);
}

void SaxParser::onObservables(const AttributeList&)
{
    m_vpzstack.pushObservables();
}

void SaxParser::onObservable(const AttributeList& att)
{
    m_vpzstack.pushObservable(att);
}

void SaxParser::onAttachedView(const AttributeList& att)
{
    m_vpzstack.pushAttachedView(att);
}

void SaxParser::onClasses(const AttributeList&)
{
    m_vpzstack.pushClasses();
}

void SaxParser::onClass(const AttributeList& att)
{
    m_vpzstack.pushClass(att);
}

void SaxParser::on_end_element(const Glib::ustring& name)
{
    EndFuncList::iterator it = m_ends.find(name);
    Assert(utils::SaxParserError, it != m_ends.end(), boost::format(
            "Unknow end element %1%") % name);

    (this->*(it->second))();
}

void SaxParser::onEndBoolean()
{
    m_valuestack.pushOnVectorValue(
        value::BooleanFactory::create(
            utils::to_boolean(lastCharactersStored())));
}

void SaxParser::onEndInteger()
{
    m_valuestack.pushOnVectorValue(
        value::IntegerFactory::create(
            utils::to_int(lastCharactersStored())));
}

void SaxParser::onEndDouble()
{
    m_valuestack.pushOnVectorValue(
        value::DoubleFactory::create(
            utils::to_double(lastCharactersStored())));
}

void SaxParser::onEndString()
{
    m_valuestack.pushOnVectorValue(
        value::StringFactory::create(
            utils::to_string(lastCharactersStored())));
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
    value::Tuple tuple = value::toTupleValue(m_valuestack.topValue());
    tuple->fill(lastCharactersStored());
    m_valuestack.popValue();
}

void SaxParser::onEndTable()
{
    value::Table table = value::toTableValue(m_valuestack.topValue());
    table->fill(lastCharactersStored());
    m_valuestack.popValue();
}

void SaxParser::onEndXML()
{
    m_valuestack.pushOnVectorValue(
        value::XMLFactory::create(m_cdata));
    m_cdata.clear();
}

void SaxParser::onEndNull()
{
    m_valuestack.pushOnVectorValue(value::NullFactory::create());
}

void SaxParser::onEndTrame()
{
    m_vpzstack.popTrame();
    if (not m_cdata.empty()) {
        ParameterTrame* pt;
        pt  = dynamic_cast < ParameterTrame* >(m_vpzstack.top());
        if (pt) {
            pt->setData(m_cdata);
            m_cdata.clear();
        }
    }
    m_vpzstack.pop();
}

void SaxParser::onEndModelTrame()
{
    m_vpzstack.popModelTrame(getValue(0));
    m_valuestack.clear();
}

void SaxParser::onEndVLETrame()
{
    m_vpzstack.popVleTrame();
    m_isEndTrame = true;
}

void SaxParser::onEndPort()
{
    if (m_vpzstack.top()->isCondition()) {
        value::Set& vals(m_vpzstack.popConditionPort());
        std::vector < value::Value >& lst(getValues());
        for (std::vector < value::Value >::iterator it =
             lst.begin(); it != lst.end(); ++it) {
            vals->addValue(*it);
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
    if (not m_cdata.empty()) {
        Output* out;
        out = dynamic_cast < Output* >(m_vpzstack.top());
        if (out) {
            out->setData(m_cdata);
            m_cdata.clear();
        }
    }
    m_vpzstack.pop(); 
}

void SaxParser::onEndClasses()
{
    m_vpzstack.popClasses();
}

void SaxParser::onEndClass()
{
    m_vpzstack.popClass();
}

void SaxParser::on_characters(const Glib::ustring& characters)
{
    addToCharacters(characters);
}

void SaxParser::on_comment(const Glib::ustring& /* text */)
{
}

void SaxParser::on_warning(const Glib::ustring& text)
{
    TraceAlways(boost::format("XML warning: %1%") % text);
}

void SaxParser::on_error(const Glib::ustring& text)
{
    Throw(utils::SaxParserError,
          (boost::format("XML Error: %1%, stack: %2%") % text % m_vpzstack));
}

void SaxParser::on_fatal_error(const Glib::ustring& text)
{
    Throw(utils::SaxParserError,
          (boost::format("XML Fatal error: %1%") % text));
}

void SaxParser::on_cdata_block(const Glib::ustring& text)
{
    m_cdata.assign(text);
}

void SaxParser::on_validity_error(const Glib::ustring& text)
{
    Throw(utils::SaxParserError,
          (boost::format("XML validity error: %1%") % text));
}

void SaxParser::on_validity_warning(const Glib::ustring& text)
{
    TraceAlways(boost::format("XML validity warning: %1%") % text);
}

const std::vector < value::Value >& SaxParser::getValues() const
{
    return m_valuestack.getResults();
}

std::vector < value::Value >& SaxParser::getValues()
{
    return m_valuestack.getResults();
}

const value::Value& SaxParser::getValue(const size_t pos) const
{
    return m_valuestack.getResult(pos);
}

const Glib::ustring& SaxParser::lastCharactersStored() const
{
    return m_lastCharacters;
}

void SaxParser::clearLastCharactersStored()
{
    m_lastCharacters.clear();
}

void SaxParser::addToCharacters(const Glib::ustring& characters)
{
    m_lastCharacters.append(characters);
}

bool existAttribute(const AttributeList& lst, const Glib::ustring& name)
{
    AttributeList::const_iterator it;
    it = std::find_if(lst.begin(), lst.end(),
                      xmlpp::SaxParser::AttributeHasName(name));
    return it != lst.end();
}

}} // namespace vle vpz
