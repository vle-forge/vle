/** 
 * @file vpz/Measures.cpp
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

////void Measures::init(xmlpp::Element* elt)
//{
//AssertI(elt);
//AssertI(elt->get_name() == "MEASURES");
//
//if (xml::exist_children(elt, "OUTPUTS")) {
//xmlpp::Element* outputs = xml::get_children(elt, "OUTPUTS");
//m_outputs.init(outputs);
//}
//
//xmlpp::Node::NodeList lst = elt->get_children("MEASURE");
//xmlpp::Node::NodeList::iterator it;
//for (it = lst.begin(); it != lst.end(); ++it) {
//Measure m;
//m.init((xmlpp::Element*)(*it));
//Glib::ustring name(xml::get_attribute((*it), "NAME"));
//
//addMeasure(name, m);
//}
//
//if (xml::exist_children(elt, "EOVS")) {
//xmlpp::Element* eovs = xml::get_children(elt, "EOVS");
//m_eovs.init(eovs);
//} else {
//buildEOV();
//}
//}

void Measures::buildEOV()
{
    if (m_eovs.eovs().empty()) {
        std::list < Output >::const_iterator it;
        for (it = m_outputs.outputs().begin(); it !=
             m_outputs.outputs().end(); ++it) {
            if ((*it).format() == Output::EOV) {
                EOV tmp;
                tmp.setPluginChild();
                tmp.host((*it).location());
                ((EOVplugin&)tmp.child()).setEOVplugin((*it).name(),
                                                       (*it).plugin());

                TRACE1(boost::format("libvpz: buildEOV %1% %2%\n") %
                       (*it).name() % (*it).plugin());
                m_eovs.addEOV((*it).name(), tmp);
            }
        }
    }
}

void Measures::write(std::ostream& out) const
{
    if (not m_outputs.outputs().empty() and not m_measures.empty()) {
        out << "<measures>";
        m_outputs.write(out);

        std::copy(m_measures.begin(), m_measures.end(),
                  std::ostream_iterator < Measure >(out, "\n"));

        out << "</measures>";

        //m_eovs.write(xmlmeasures);
        //FIXME FIXME FIXME 
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
    AssertI(not m_outputs.exist(output));

    Measure m;
    m.setName(name);
    m.setEventMeasure(output);
    return addMeasure(m);
}

Measure& Measures::addTimedMeasure(const std::string& name,
        double timestep,
        const std::string& output)
{
    AssertI(not m_outputs.exist(output));

    Measure m;
    m.setName(name);
    m.setTimedMeasure(timestep, output);
    return addMeasure(m);
}

void Measures::delMeasure(const std::string& name)
{
    std::remove_if(m_measures.begin(), m_measures.end(), MeasureHasName(name));
}

void Measures::addObservableToMeasure(const std::string& measurename,
                                      const std::string& modelname,
                                      const std::string& portname,
                                      const std::string& group,
                                      int index)
{
    MeasureList::iterator it;
    it = std::find_if(m_measures.begin(), m_measures.end(),
                      MeasureHasName(measurename));

    Assert(utils::ParseError, it != m_measures.end(),
           boost::format("Measure '%1%' unknow to add observable '%2%'\n") %
                           measurename % (modelname + portname));
    Observable o;
    o.setObservable(modelname, portname, group, index);
    (*it).addObservable(o);
}

void Measures::addMeasures(const Measures& m)
{
    m_outputs.addOutputs(m.outputs());

    // FIXME add les eovs.

    MeasureList::const_iterator it;
    for (it = m.measures().begin(); it != m.measures().end(); ++it) {
        addMeasure(*it);
    }

    if (m_eovs.eovs().empty()) {
        buildEOV();
    }
}

Measure& Measures::addMeasure(const Measure& m)
{
    MeasureList::iterator it;
    it = std::find_if(m_measures.begin(), m_measures.end(),
                      MeasureHasName(m.name()));

    Assert(utils::InternalError, it == m_measures.end(),
           boost::format("A Measure have already this name '%1%'\n") %
           m.name());

    Assert(utils::InternalError, m_outputs.exist(m.output()),
           boost::format("Unknow output name '%1%' for measure '%2%'\n") %
           m.output() % m.name());
    
    m_measures.push_back(m);
    return *m_measures.rbegin();
}

const Measure& Measures::findMeasureFromOutput(const std::string& outputname,
                                               std::string& measure) const
{
    MeasureList::const_iterator it;
    std::find_if(m_measures.begin(), m_measures.end(),
                 MeasureHasOutput(outputname));

    Assert(utils::InternalError, it != m_measures.end(),
          boost::format("Failed to find measure attached to the output '%1%'\n")
          % outputname);
    
    measure = (*it).name();

    return *it;
}

const Measure& Measures::find(const std::string& name) const
{
    MeasureList::const_iterator it = std::find_if(m_measures.begin(),
                                                  m_measures.end(),
                                                  MeasureHasName(name));

    Assert(utils::InternalError, it != m_measures.end(),
           boost::format("Unknow measure '%1%'\n") % name);

    return (*it);
}

Measure& Measures::find(const std::string& name)
{
    MeasureList::iterator it = std::find_if(m_measures.begin(),
                                            m_measures.end(),
                                            MeasureHasName(name));

    Assert(utils::InternalError, it != m_measures.end(),
           boost::format("Unknow measure '%1%'\n") % name);

    return (*it);
}

bool Measures::exist(const std::string& name) const
{
    MeasureList::const_iterator it = std::find_if(m_measures.begin(),
                                                  m_measures.end(),
                                                  MeasureHasName(name));

    return it != m_measures.end();
}

}} // namespace vle vpz
