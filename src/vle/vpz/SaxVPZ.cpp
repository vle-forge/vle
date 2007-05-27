/** 
 * @file SaxVPZ.cpp
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

#include <vle/vpz/SaxVPZ.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/vpz/Structures.hpp>
#include <vle/vpz/Model.hpp>
#include <vle/vpz/Port.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/String.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/NoVLEModel.hpp>


namespace vle { namespace vpz {

vpz::Vpz* VpzStackSax::push_vpz(const std::string& author, float version,
                           const std::string& date)
{
    AssertS(utils::SaxParserError, m_stack.empty());
    AssertS(utils::SaxParserError, not m_vpz);

    m_vpz = new vpz::Vpz();
    m_vpz->project().setAuthor(author);
    m_vpz->project().setVersion(version);
    m_vpz->project().setDate(date);
    m_stack.push(m_vpz);

    return m_vpz;
}

void VpzStackSax::push_structure()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_vpz);

    vpz::Structures* structure = new vpz::Structures();
    m_stack.push(structure);
}

void VpzStackSax::push_model(const xmlpp::SaxParser::AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_vpz);
    AssertS(utils::SaxParserError, m_stack.top()->isStructures() or
            m_stack.top()->isSubmodels());

    graph::CoupledModel* parent = 0;

    if (m_stack.top()->isSubmodels()) {
        vpz::Base* sub = pop();
        vpz::Model* tmp = static_cast < vpz::Model* >(m_stack.top());
        parent = static_cast < graph::CoupledModel* >(tmp->model());
        m_stack.push(sub);
    }

    graph::Model* gmdl = 0;

    std::string type(VLESaxParser::get_attribute < std::string >(att, "type"));
    std::string name(VLESaxParser::get_attribute < std::string >(att, "name"));
    if (type == "atomic") {
        std::string cnd(VLESaxParser::get_attribute < std::string >(
                att, "condition"));
        std::string dyn(VLESaxParser::get_attribute < std::string >(
                att, "dynamics"));
        gmdl = new graph::AtomicModel(parent);
        vpz().project().model().atomicModels().insert(std::make_pair(
                reinterpret_cast < graph::AtomicModel* >(gmdl),
                AtomicModel(cnd, dyn)));
    } else if (type == "coupled") {
        gmdl = new graph::CoupledModel(parent);
    } else if (type == "novle") {
        gmdl = new graph::NoVLEModel(parent);
    } else {
        Throw(utils::InternalError, (boost::format(
                        "Unknow model type %1%") % type));
    }
    gmdl->setName(name);

    if (parent) {
        parent->addModel(gmdl);
    }

    vpz::Model* mdl = new vpz::Model();
    mdl->setModel(gmdl);

    if (m_stack.top()->isStructures()) {
        vpz().project().model().setModel(gmdl);
    }

    m_stack.push(mdl);
}

void VpzStackSax::push_port(const xmlpp::SaxParser::AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_vpz);
    AssertS(utils::SaxParserError, m_stack.top()->isIn() or
            m_stack.top()->isOut() or m_stack.top()->isState() or
            m_stack.top()->isInit());

    vpz::Base* type = pop();

    AssertS(utils::SaxParserError, m_stack.top()->isModel());

    vpz::Model* mdl = static_cast < vpz::Model* >(m_stack.top());
    graph::Model* gmdl = mdl->model();
    
    std::string name = VLESaxParser::get_attribute < std::string >(att, "name");

    if (type->isIn()) {
        gmdl->addInputPort(name);
    } else if (type->isOut()) {
        gmdl->addOutputPort(name);
    } else if (type->isState()) {
        gmdl->addStatePort(name);
    } else if (type->isInit()) {
        gmdl->addInitPort(name);
    }
    m_stack.push(type);
}

void VpzStackSax::push_porttype(const Glib::ustring& name)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_vpz);
    AssertS(utils::SaxParserError, m_stack.top()->isModel());

    vpz::Base* prt = 0;
    if (name == "in") {
        prt = new vpz::In();
    } else if (name == "out") {
        prt = new vpz::Out();
    } else if (name == "state") {
        prt = new vpz::State();
    } else if (name == "init") {
        prt = new vpz::Init();
    } else {
        Throw(utils::InternalError, (boost::format("Unknow port type %1%.") %
                                     name));
    }
    m_stack.push(prt);
}

void VpzStackSax::push_submodels()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_vpz);
    AssertS(utils::SaxParserError, m_stack.top()->isModel());

    vpz::Submodels* sub = new vpz::Submodels();
    m_stack.push(sub);
}

void VpzStackSax::push_connections()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_vpz);
    AssertS(utils::SaxParserError, m_stack.top()->isModel());

    vpz::Connections* cnts = new vpz::Connections();
    m_stack.push(cnts);
}

void VpzStackSax::push_connection(const xmlpp::SaxParser::AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_vpz);
    AssertS(utils::SaxParserError, m_stack.top()->isConnections());

    std::string type = VLESaxParser::get_attribute < std::string >(att, "type");
    vpz::Base* cnt = 0;
    
    if (type == "internal") {
        cnt = new vpz::InternalConnection();
    } else if (type == "input") {
        cnt = new vpz::InputConnection();
    } else if (type == "output") {
        cnt = new vpz::OutputConnection();
    } else {
        Throw(utils::InternalError, (boost::format("Unknow connection type %1%")
                                     % type));
    }
    m_stack.push(cnt);
}

void VpzStackSax::push_origin(const xmlpp::SaxParser::AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_vpz);
    AssertS(utils::SaxParserError, m_stack.top()->isInternalConnection() or
            m_stack.top()->isInputConnection() or
            m_stack.top()->isOutputConnection());

    std::string mdl = VLESaxParser::get_attribute < std::string >(att, "model");
    std::string port = VLESaxParser::get_attribute < std::string >(att, "port");

    vpz::Base* orig = new vpz::Origin(mdl, port);
    m_stack.push(orig);
}

void VpzStackSax::push_destination(const xmlpp::SaxParser::AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_vpz);
    AssertS(utils::SaxParserError, m_stack.top()->isOrigin());

    std::string mdl = VLESaxParser::get_attribute < std::string >(att, "model");
    std::string port = VLESaxParser::get_attribute < std::string >(att, "port");

    vpz::Base* dest = new vpz::Destination(mdl, port);
    m_stack.push(dest);
}

void VpzStackSax::build_connection()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_vpz);

    AssertS(utils::SaxParserError, m_stack.top()->isDestination());
    vpz::Destination* dest = static_cast < vpz::Destination* >(pop());

    AssertS(utils::SaxParserError, m_stack.top()->isOrigin());
    vpz::Origin* orig = static_cast < vpz::Origin* >(pop());
    
    AssertS(utils::SaxParserError, m_stack.top()->isInternalConnection() or
            m_stack.top()->isInputConnection() or
            m_stack.top()->isOutputConnection());
    vpz::Base* cnt = pop();

    AssertS(utils::SaxParserError, m_stack.top()->isConnections());
    vpz::Base* cntx = pop();
    
    AssertS(utils::SaxParserError, m_stack.top()->isModel());
    vpz::Model* model = static_cast < vpz::Model* >(m_stack.top());

    graph::CoupledModel* cpl = static_cast < graph::CoupledModel*
        >(model->model());

    if (cnt->isInternalConnection()) {
        cpl->addInternalConnection(orig->model, orig->port, dest->model,
                                   dest->port);
    } else if (cnt->isInputConnection()) {
        cpl->addInputConnection(orig->model, orig->port, dest->model,
                                dest->port);
    } else if (cnt->isOutputConnection()) {
        cpl->addOutputConnection(orig->model, orig->port, dest->model,
                                 dest->port);
    }

    m_stack.push(cntx);
    delete dest;
    delete orig;
    delete cnt;
}

void VpzStackSax::push_dynamics()
{
}

void VpzStackSax::push_dynamic(const xmlpp::SaxParser::AttributeList& /*att*/)
{
}

vpz::Base* VpzStackSax::pop()
{
    vpz::Base* top = m_stack.top();
    m_stack.pop();
    return top;
}

vpz::Vpz& VpzStackSax::vpz()
{
    Assert(utils::InternalError, m_vpz, "VPZ stack sax have empty VPZ object");
    return *m_vpz;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

VLESaxParser::VLESaxParser() :
    m_vpz(0),
    m_isValue(false),
    m_isVPZ(false)
{
}

VLESaxParser::~VLESaxParser()
{
}

void VLESaxParser::clear_parser_state()
{
    m_valuestack.clear();
    m_lastCharacters.clear();
    m_isValue = false;
    m_isVPZ = false;
    m_isTrame = false;
}

void VLESaxParser::on_start_document()
{
    clear_parser_state();
}

void VLESaxParser::on_end_document()
{
}

void VLESaxParser::on_start_element(
    const Glib::ustring& name,
    const xmlpp::SaxParser::AttributeList& att)
{
    clearLastCharactersStored();
    if (name == "boolean") {
        m_valuestack.push_boolean();
    } else if (name == "integer") {
        m_valuestack.push_integer();
    } else if (name == "double") {
        m_valuestack.push_double();
    } else if (name == "string") {
        m_valuestack.push_string();
    } else if (name == "set") {
        m_valuestack.push_set();
    } else if (name == "map") {
        m_valuestack.push_map();
    } else if (name == "key") {
        m_valuestack.push_map_key(get_attribute < Glib::ustring >(att, "name"));
    } else if (name == "tuple") {
        m_valuestack.push_tuple();
    } else if (name == "table") {
        m_valuestack.push_table(
            get_attribute < value::TableFactory::index >(att, "width"),
            get_attribute < value::TableFactory::index >(att, "height"));
    } else if (name == "vle_project") {
        AssertS(utils::SaxParserError, not m_isValue and not m_isTrame);
        m_isVPZ = true;
        m_vpz = m_vpzstack.push_vpz(
            get_attribute < std::string >(att, "author"),
            get_attribute < float >(att, "version"),
            get_attribute < std::string >(att, "date"));

    } else if (name == "structures") {
        m_vpzstack.push_structure();
    } else if (name == "model") {
        m_vpzstack.push_model(att);
    } else if (name == "in" or name == "out" or name == "state" or
               name == "init") {
        m_vpzstack.push_porttype(name);
    } else if (name == "port") {
        m_vpzstack.push_port(att);
    } else if (name == "submodels") {
        m_vpzstack.push_submodels();
    } else if (name == "connections") {
        m_vpzstack.push_connections();
    } else if (name == "connection") {
        m_vpzstack.push_connection(att);
    } else if (name == "origin") {
        m_vpzstack.push_origin(att);
    } else if (name == "destination") {
        m_vpzstack.push_destination(att);
    } else if (name == "dynamics") {
        m_vpzstack.push_dynamics();
    } else if (name == "dynamic") {
        m_vpzstack.push_dynamic(att);
    } else {
        Throw(utils::SaxParserError,
              (boost::format("Unknow element %1%") % name));
    }
}

void VLESaxParser::on_end_element(const Glib::ustring& name)
{
    if (name == "boolean") {
        m_valuestack.push_on_vector_value(
            value::BooleanFactory::create(
                utils::to_boolean(lastCharactersStored())));
    } else if (name == "integer") {
        m_valuestack.push_on_vector_value(
            value::IntegerFactory::create(
                utils::to_int(lastCharactersStored())));
    } else if (name == "double") {
        m_valuestack.push_on_vector_value(
            value::DoubleFactory::create(
                utils::to_double(lastCharactersStored())));
    } else if (name == "string") {
        m_valuestack.push_on_vector_value(
            value::StringFactory::create(
                utils::to_string(lastCharactersStored())));
    } else if (name == "key") {
        // FIXME delete test
    } else if (name == "set" or name == "map") {
        m_valuestack.pop_value();
    } else if (name == "tuple") {
        value::Tuple tuple = value::to_tuple(m_valuestack.top_value());
        tuple->fill(lastCharactersStored());
        m_valuestack.pop_value();
    } else if (name == "table") {
        value::Table table = value::to_table(m_valuestack.top_value());
        table->fill(lastCharactersStored());
        m_valuestack.pop_value();
    } else if (name == "in" or name == "out" or name == "state" or name ==
               "init" or name == "structures" or name == "model" or name ==
               "submodels" or name == "vle_project" or name == "connections") {
        m_vpzstack.pop();
    } else if (name == "destination") {
        m_vpzstack.build_connection();
    }
}

void VLESaxParser::on_characters(const Glib::ustring& characters)
{
    addToCharacters(characters);
}

void VLESaxParser::on_comment(const Glib::ustring& /* text */)
{
}

void VLESaxParser::on_warning(const Glib::ustring& text)
{
    DTRACE(text);
}

void VLESaxParser::on_error(const Glib::ustring& text)
{
    Throw(utils::SaxParserError, text);
}

void VLESaxParser::on_fatal_error(const Glib::ustring& text)
{
    Throw(utils::SaxParserError, text);
}

void VLESaxParser::on_cdata_block(const Glib::ustring& /* text */)
{
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

const std::vector < value::Value >& VLESaxParser::get_values() const
{
    return m_valuestack.get_results();
}

const value::Value& VLESaxParser::get_value(const size_t pos) const
{
    return m_valuestack.get_result(pos);
}

const Glib::ustring& VLESaxParser::lastCharactersStored() const
{
    return m_lastCharacters;
}

void VLESaxParser::clearLastCharactersStored()
{
    m_lastCharacters.clear();
}

void VLESaxParser::addToCharacters(const Glib::ustring& characters)
{
    m_lastCharacters.append(characters);
}

}} // namespace vle vpz
