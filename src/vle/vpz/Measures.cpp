/** 
 * @file Measures.cpp
 * @brief 
 * @author The vle Development Team
 * @date lun, 13 fév 2006 18:53:56 +0100
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

#include <vle/vpz/Measures.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/XML.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

Measures::Measures()
{
}

Measures::~Measures()
{
}

void Measures::init(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "MEASURES");

    if (xml::exist_children(elt, "OUTPUTS")) {
        xmlpp::Element* outputs = xml::get_children(elt, "OUTPUTS");
        m_outputs.init(outputs);
    }

    xmlpp::Node::NodeList lst = elt->get_children("MEASURE");
    xmlpp::Node::NodeList::iterator it;
    for (it = lst.begin(); it != lst.end(); ++it) {
        Measure m;
        m.init((xmlpp::Element*)(*it));
        Glib::ustring name(xml::get_attribute((*it), "NAME"));

        addMeasure(name, m);
    }

    if (xml::exist_children(elt, "EOVS")) {
        xmlpp::Element* eovs = xml::get_children(elt, "EOVS");
        m_eovs.init(eovs);
    } else {
        buildEOV();
    }
}

void Measures::buildEOV()
{
    if (m_eovs.eovs().empty()) {
        std::map < std::string, Output >::const_iterator it;
        for (it = m_outputs.outputs().begin(); it !=
             m_outputs.outputs().end(); ++it) {
            if ((*it).second.format() == Output::EOV) {
                EOV tmp;
                tmp.setPluginChild();
                tmp.host((*it).second.location());
                ((EOVplugin&)tmp.child()).setEOVplugin((*it).first,
                                                       (*it).second.plugin());

                TRACE1(boost::format("libvpz: buildEOV %1% %2%\n") %
                       (*it).first % (*it).second.plugin());
                m_eovs.addEOV((*it).first, tmp);
            }
        }
    }
}

void Measures::write(xmlpp::Element* elt) const
{
    AssertI(elt);

    if (not m_outputs.outputs().empty() and not m_measures.empty()) {
        xmlpp::Element* xmlmeasures = elt->add_child("MEASURES");
        m_outputs.write(xmlmeasures);

        std::map < std::string, Measure >::const_iterator it;
        for (it = m_measures.begin(); it != m_measures.end(); ++it) {
            xmlpp::Element* xml = xmlmeasures->add_child("MEASURE");
            xml->set_attribute("NAME", (*it).first);
            (*it).second.write(xml);
        }

        m_eovs.write(xmlmeasures);
    }
}

void Measures::addTextStreamOutput(const std::string& name,
                                   const std::string& location)
{
    m_outputs.addTextStream(name, location);
}

void Measures::addSdmlStreamOutput(const std::string& name,
                                   const std::string& location)
{
    m_outputs.addSdmlStream(name, location);
}

void Measures::addEovStreamOutput(const std::string& name,
                                  const std::string& plugin,
                                  const std::string& location)
{
    m_outputs.addEovStream(name, plugin, location);
}

void Measures::clear()
{
    m_outputs.clear();
    m_eovs.clear();
    m_measures.clear();
}

void Measures::delOutput(const std::string& name)
{
    m_outputs.delOutput(name);
}

Measure& Measures::addEventMeasure(const std::string& name,
        const std::string& output)
{
    AssertI(m_outputs.outputs().find(output) != m_outputs.outputs().end());

    Measure m;
    m.setEventMeasure(output);
    return addMeasure(name, m);
}

Measure& Measures::addTimedMeasure(const std::string& name,
        double timestep,
        const std::string& output)
{
    AssertI(m_outputs.outputs().find(output) != m_outputs.outputs().end());

    Measure m;
    m.setTimedMeasure(timestep, output);
    return addMeasure(name, m);
}

void Measures::delMeasure(const std::string& name)
{
    std::map < std::string, Measure >::iterator it = m_measures.find(name);
    if (it != m_measures.end()) {
        m_measures.erase(it);
    }
}

void Measures::addObservableToMeasure(const std::string& measurename,
                                      const std::string& modelname,
                                      const std::string& portname,
                                      const std::string& group,
                                      int index)
{
    std::map < std::string, Measure >::iterator it =
        m_measures.find(measurename);

    Assert(utils::ParseError, it != m_measures.end(),
           boost::format("Measure '%1%' unknow to add observable '%2%'\n") %
                           measurename % (modelname + portname));
    Observable o;
    o.setObservable(modelname, portname, group, index);
    (*it).second.addObservable(o);
}

void Measures::addMeasures(const Measures& m)
{
    m_outputs.addOutputs(m.outputs());

    // FIXME add les eovs.

    std::map < std::string, Measure >::const_iterator it;
    for (it = m.measures().begin(); it != m.measures().end(); ++it) {
        addMeasure((*it).first, (*it).second);
    }

    if (m_eovs.eovs().empty()) {
        buildEOV();
    }
}

Measure& Measures::addMeasure(const std::string& name,
        const Measure& m)
{
    std::map < std::string, Measure >::iterator it = m_measures.find(name);

    Assert(utils::InternalError, it == m_measures.end(),
           boost::format("A Measure have already this name '%1%'\n") % name);

    Assert(utils::InternalError, 
           m_outputs.outputs().find(m.output()) != m_outputs.outputs().end(),
           boost::format("Unknow output name '%1%' for measure '%2%'\n") %
           m.output() % name);
    
    return m_measures[name] = m;
}

const Measure& Measures::findMeasureFromOutput(const std::string& output,
                                               std::string& measure) const
{
    std::map < std::string, Measure >::const_iterator it;
    for (it = m_measures.begin(); it != m_measures.end(); ++it) {
        if ((*it).second.output() == output) {
            measure = (*it).first;
            return (*it).second;
        }
    }
    Throw(utils::InternalError,
          boost::format("Failed to find measure attached to the output '%1%'\n")
          % output);
}

const Measure& Measures::find(const std::string& name) const
{
    std::map < std::string, vpz::Measure >::const_iterator it;
    it = m_measures.find(name);
    Assert(utils::InternalError, it != m_measures.end(),
           boost::format("Unknow measure '%1%'\n") % name);

    return (*it).second;
}

Measure& Measures::find(const std::string& name)
{
    std::map < std::string, Measure >::iterator it = m_measures.find(name);
    Assert(utils::InternalError, it != m_measures.end(),
           boost::format("Unknow measure '%1%'\n") % name);

    return (*it).second;
}

bool Measures::exist(const std::string& name) const
{
    std::map < std::string, Measure >::const_iterator it = m_measures.find(name);
    return it != m_measures.end();
}

}} // namespace vle vpz
