/** 
 * @file SaxParser.cpp
 * @brief A class to parse VPZ XML with Sax parser.
 * @author The vle Development Team
 * @date jeu, 14 déc 2006 10:34:50 +0100
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



namespace vle { namespace vpz {

SaxParser::SaxParser(Vpz& vpz) :
    m_vpzstack(vpz),
    m_vpz(vpz),
    m_isValue(false),
    m_isVPZ(false),
    m_isTrame(false),
    m_isEndTrame(false)
{
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
    if (name == "boolean") {
        m_valuestack.pushBoolean();
    } else if (name == "integer") {
        m_valuestack.pushInteger();
    } else if (name == "double") {
        m_valuestack.pushDouble();
    } else if (name == "string") {
        m_valuestack.pushString();
    } else if (name == "set") {
        m_valuestack.pushSet();
    } else if (name == "map") {
        m_valuestack.pushMap();
    } else if (name == "key") {
        m_valuestack.pushMapKey(getAttribute < Glib::ustring >(att, "name"));
    } else if (name == "tuple") {
        m_valuestack.pushTuple();
    } else if (name == "table") {
        m_valuestack.pushTable(
            getAttribute < value::TableFactory::index >(att, "width"),
            getAttribute < value::TableFactory::index >(att, "height"));
    } else if (name == "xml") {
        m_valuestack.pushXml();
    } else if (name == "vle_project") {
        AssertS(utils::SaxParserError, not m_isValue and not m_isTrame);
        m_isVPZ = true;
        m_vpzstack.pushVpz(att);
    } else if (name == "structures") {
        m_vpzstack.pushStructure();
    } else if (name == "model") {
        m_vpzstack.pushModel(att);
    } else if (name == "in" or name == "out" or name == "state" or
               name == "init") {
        m_vpzstack.pushPortType(name);
    } else if (name == "port") {
        m_vpzstack.pushPort(att);
    } else if (name == "submodels") {
        m_vpzstack.pushSubModels();
    } else if (name == "connections") {
        m_vpzstack.pushConnections();
    } else if (name == "connection") {
        m_vpzstack.pushConnection(att);
    } else if (name == "origin") {
        m_vpzstack.pushOrigin(att);
    } else if (name == "destination") {
        m_vpzstack.pushDestination(att);
    } else if (name == "dynamics") {
        m_vpzstack.pushDynamics();
    } else if (name == "dynamic") {
        m_vpzstack.pushDynamic(att);
    } else if (name == "experiment") {
        m_vpzstack.pushExperiment(att);
    } else if (name == "replicas") {
        m_vpzstack.pushReplicas(att);
    } else if (name == "conditions") {
        m_vpzstack.pushConditions();
    } else if (name == "condition") {
        m_vpzstack.pushCondition(att);
    } else if (name == "views") {
        m_vpzstack.pushViews();
    } else if (name == "outputs") {
        m_vpzstack.pushOutputs();
    } else if (name == "output") {
        m_vpzstack.pushOutput(att);
    } else if (name == "view") {
        m_vpzstack.pushView(att);
    } else if (name == "observables") {
        m_vpzstack.pushObservables();
    } else if (name == "observable") {
        m_vpzstack.pushObservable(att);
    } else if (name == "attachedview") {
        m_vpzstack.pushAttachedView(att);
    } else if (name == "translators") {
        m_vpzstack.pushTranslators();
    } else if (name == "translator") {
        m_vpzstack.pushTranslator(att);
    } else if (name == "vle_trame") {
        m_isEndTrame = false;
        m_vpzstack.pushVleTrame();
    } else if (name == "trame") {
        m_vpzstack.pushTrame(att);
    } else if (name == "modeltrame") {
        m_vpzstack.pushModelTrame(att);
    } else {
        Throw(utils::SaxParserError,
              (boost::format("Unknow element %1%") % name));
    }
}

void SaxParser::on_end_element(const Glib::ustring& name)
{
    if (name == "boolean") {
        m_valuestack.pushOnVectorValue(
            value::BooleanFactory::create(
                utils::to_boolean(lastCharactersStored())));
    } else if (name == "integer") {
        m_valuestack.pushOnVectorValue(
            value::IntegerFactory::create(
                utils::to_int(lastCharactersStored())));
    } else if (name == "double") {
        m_valuestack.pushOnVectorValue(
            value::DoubleFactory::create(
                utils::to_double(lastCharactersStored())));
    } else if (name == "string") {
        m_valuestack.pushOnVectorValue(
            value::StringFactory::create(
                utils::to_string(lastCharactersStored())));
    } else if (name == "key") {
        // FIXME delete test
    } else if (name == "set" or name == "map") {
        m_valuestack.popValue();
    } else if (name == "tuple") {
        value::Tuple tuple = value::toTupleValue(m_valuestack.topValue());
        tuple->fill(lastCharactersStored());
        m_valuestack.popValue();
    } else if (name == "table") {
        value::Table table = value::toTableValue(m_valuestack.topValue());
        table->fill(lastCharactersStored());
        m_valuestack.popValue();
    } else if (name == "xml") {
        m_valuestack.pushOnVectorValue(
            value::XMLFactory::create(m_cdata));
        m_cdata.clear();
    } else if (name == "translator") {
        m_vpzstack.popTranslator(m_cdata);
        m_cdata.clear();
        m_vpzstack.pop();
    } else if (name == "port" and m_vpzstack.top()->isCondition()) {
        value::Set& vals(m_vpzstack.popConditionPort());
        std::vector < value::Value >& lst(getValues());
        for (std::vector < value::Value >::iterator it =
             lst.begin(); it != lst.end(); ++it) {
            vals->addValue(*it);
        }
        m_valuestack.clear();
    } else if (name == "port" and m_vpzstack.top()->isObservablePort()) {
        m_vpzstack.pop();
    } else if (name == "in" or name == "out" or name == "state" or
               name == "init" or name == "structures" or name == "model" or
               name == "submodels" or name == "vle_project" or
               name == "connections" or name == "conditions" or
               name == "condition" or name == "outputs" or name == "dynamics" or
               name == "views" or name == "observables" or
               name == "observable" or name == "experiment" or
               name == "translators" or name == "vle_project") {
        m_vpzstack.pop();
    } else if (name == "destination") {
        m_vpzstack.buildConnection();
    } else if (name == "output") {
        if (not m_cdata.empty()) {
            Output* out;
            out = dynamic_cast < Output* >(m_vpzstack.top());
            if (out) {
                out->setData(m_cdata);
                m_cdata.clear();
            }
        }
        m_vpzstack.pop(); 
    } else if (name == "trame") {
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
    } else if (name == "modeltrame") {
        m_vpzstack.popModelTrame(getValue(0));
        m_valuestack.clear();
    } else if (name == "vle_trame") {
        m_vpzstack.popVleTrame();
        m_isEndTrame = true;
    }
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
         (boost::format("XML Error: %1%") % text));
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
