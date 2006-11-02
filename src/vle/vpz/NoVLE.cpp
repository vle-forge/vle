/** 
 * @file vpz/NoVLE.cpp
 * @brief 
 * @author The vle Development Team
 * @date mer, 22 fév 2006 16:29:01 +0100
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

#include <vle/vpz/NoVLE.hpp>
#include <vle/vpz/Translator.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Path.hpp>

namespace vle { namespace vpz {
    
using namespace vle::utils;

void NoVLE::init(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "NO_VLE");

    if (xml::has_children(elt)) {
        setNoVLE(xml::get_attribute(elt, "TRANSLATOR"),
                 xml::write_to_string(elt));
    } else {
        setNoVLE(xml::get_attribute(elt, "TRANSLATOR"),
                 "");
    }
}

void NoVLE::write(xmlpp::Element* elt) const
{
    AssertI(elt);
    AssertI(elt->get_name() == "NO_VLE");

    elt->set_attribute("TRANSLATOR", m_translator);
    xml::import_children_nodes_without_parent(elt, m_xml);
}

void NoVLE::setNoVLE(const std::string& translator, const Glib::ustring& xml)
{
    AssertI(not xml.empty());
    AssertI(not translator.empty());

    m_xml.assign(xml);
    m_translator.assign(translator);
}

void NoVLE::callTranslator(Model& model, Dynamics& dynamics,
                           Graphics& graphics, Experiment& experiment)
{
    xmlpp::DomParser* dom = new xmlpp::DomParser;
    try {
        dom->parse_memory(m_xml);
    } catch (const xmlpp::exception& e) {
        Throw(utils::ParseError,
              boost::format("Translator XML for '%1%' problem: %2%\n") %
              m_translator % e.what());
    }

    Glib::Module* module = translator();
    void* makeNewTranslator;

    bool symb = module->get_symbol("makeNewTranslator", makeNewTranslator);
    Assert(utils::ParseError, symb,
           boost::format("Error in '%1%', function 'makeNewTranslator' not "
                         "found '%2%'") % m_translator %
           Glib::Module::get_last_error());

    Translator* call;
    call = ((Translator*(*)(xmlpp::Element*))(makeNewTranslator))
        (dom->get_document()->get_root_node());

    Assert(utils::ParseError, call,
           boost::format("Error in '%1%', function 'makeNewTranslator': "
                           "problem allocation a new Translator '%2%'") %
                           m_translator % Glib::Module::get_last_error());

    call->translate();
    model.initFromModel(call->getStructure()->get_root_node());
    dynamics.initFromModels(call->getDynamics()->get_root_node());

    if (call->getGraphics())
        graphics.initFromModels(call->getGraphics()->get_root_node());

    if (call->getExperiment())
        experiment.initFromExperiment(call->getExperiment()->get_root_node());

    delete call;
    delete module;
    delete dom;
}

Glib::Module* NoVLE::translator()
{
    std::string file = Glib::Module::build_path(
        utils::Path::path().getDefaultTranslatorDir(), m_translator);

    Glib::Module* module = new Glib::Module(file);
    if (not (*module)) {
        std::string err(Glib::Module::get_last_error());
        delete module;

        std::string ufile = Glib::Module::build_path(
                utils::Path::path().getUserTranslatorDir(), m_translator);
        
        module = new Glib::Module(ufile);
        if (not (*module)) {
            std::string err2(Glib::Module::get_last_error());
            delete module;

            Glib::ustring error((boost::format(
                        "Error opening plugin %1% or %2% translator %3% "
                        "with error:") % file % ufile % m_translator).str());

            error += "\n";
            error += err;
            error += "\n";
            error += err2;

            Throw(utils::FileError, error);
        }
    }
    return module;
}

}} // namespace vle vpz
