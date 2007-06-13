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

bool MeasureList::exist(const std::string& name) const
{
    return find(name) != end();
}

Measures::Measures()
{
}

void Measures::buildEOV()
{
    if (m_eovs.eovs().empty()) {
        Outputs::const_iterator it;
        for (it = m_outputs.begin(); it != m_outputs.end(); ++it) {
            if ((it->second).format() == Output::EOV) {
                EOV tmp;
                tmp.setPluginChild();
                tmp.host((it->second).location());
                ((EOVplugin&)tmp.child()).setEOVplugin(it->second.name(),
                                                       it->second.plugin());

                TRACE1(boost::format("libvpz: buildEOV %1% %2%\n") %
                       it->second.name() % it->second.plugin());
                m_eovs.addEOV(it->second.name(), tmp);
            }
        }
    }
}

void Measures::write(std::ostream& out) const
{
    if (not m_outputs.empty() and not m_measures.empty()) {
        out << "<measures>";
        m_outputs.write(out);

        for (MeasureList::const_iterator it = m_measures.begin(); it !=
             m_measures.end(); ++it) {
            it->second.write(out);
        }

        out << "</measures>";
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
    m_outputs.del(name);
}

Measure& Measures::addEventMeasure(const std::string& name,
        const std::string& output)
{
    AssertI(not m_measures.exist(output));

    Measure m;
    m.setName(name);
    m.setEventMeasure(output);
    return addMeasure(m);
}

Measure& Measures::addTimedMeasure(const std::string& name,
        double timestep,
        const std::string& output)
{
    AssertI(not m_measures.exist(output));

    Measure m;
    m.setName(name);
    m.setTimedMeasure(timestep, output);
    return addMeasure(m);
}

void Measures::delMeasure(const std::string& name)
{
    MeasureList::iterator it = m_measures.find(name);
    if (it != m_measures.end()) {
        m_measures.erase(it);
    }
}

void Measures::addObservableToMeasure(const std::string& measurename,
                                      const std::string& name,
                                      const std::string& group,
                                      int index)
{
    MeasureList::iterator it = m_measures.find(measurename);
    Assert(utils::SaxParserError, it != m_measures.end(),
           (boost::format("Measures %1% does not exist.") % measurename));

    Observable o;
    o.setObservable(name, group, index);
    it->second.addObservable(o);
}

void Measures::addMeasures(const Measures& m)
{
    m_outputs.add(m.outputs());

    for (MeasureList::const_iterator it = m.measures().begin(); it !=
         m.measures().end(); ++it) {
        addMeasure(it->second);
    }

    if (m_eovs.eovs().empty()) {
        buildEOV();
    }
}

Measure& Measures::addMeasure(const Measure& m)
{
    MeasureList::const_iterator it = m_measures.find(m.name());
    Assert(utils::SaxParserError, it == m_measures.end(),
           boost::format("A Measure have already this name '%1%'\n") %
           m.name());

    Assert(utils::SaxParserError, m_outputs.exist(m.output()),
           boost::format("Unknow output name '%1%' for measure '%2%'\n") %
           m.output() % m.name());
    
    return (m_measures[m.name()] = m);
}

const Measure& Measures::findMeasureFromOutput(const std::string& outputname,
                                               std::string& measure) const
{
    for (MeasureList::const_iterator it = m_measures.begin(); it !=
         m_measures.end(); ++it) {
        if (it->second.existObservable(outputname)) {
            measure.assign(it->second.name());
            return it->second;
        }
    }

    Throw(utils::SaxParserError, 
          (boost::format("Failed to find measure attached to the output '%1%'\n")
          % outputname));
}

const Measure& Measures::find(const std::string& name) const
{
    MeasureList::const_iterator it = m_measures.find(name);
    Assert(utils::SaxParserError, it != m_measures.end(),
           boost::format("Unknow measure '%1%'\n") % name);

    return it->second;
}

Measure& Measures::find(const std::string& name)
{
    MeasureList::iterator it = m_measures.find(name);
    Assert(utils::SaxParserError, it != m_measures.end(),
           boost::format("Unknow measure '%1%'\n") % name);

    return it->second;
}

bool Measures::exist(const std::string& name) const
{
    return m_measures.find(name) != m_measures.end();
}

}} // namespace vle vpz
