/** 
 * @file vpz/Measure.cpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:31:58 +0100
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

#include <vle/vpz/Measure.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

Measure::Measure()
{
}

Measure::~Measure()
{
}

void Measure::init(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "MEASURE");

    m_observables.clear();

    Glib::ustring type(xml::get_attribute(elt, "TYPE"));

    if (type == "timed") {
        setTimedMeasure(xml::get_double_attribute(elt, "TIME_STEP"),
                        xml::get_attribute(elt, "OUTPUT"));
    } else if (type == "event") {
        setEventMeasure(xml::get_attribute(elt, "OUTPUT"));
    } else {
        Throw(utils::InternalError,
              boost::format("Unknow Measure type %1%\n") % type);
    }

    xmlpp::Node::NodeList lst = elt->get_children("OBSERVABLE");
    xmlpp::Node::NodeList::iterator it = lst.begin();
    while (it != lst.end()) {
        Observable o;
        o.init((xmlpp::Element*)(*it));
        addObservable(o);
        ++it;
    }
}

void Measure::write(xmlpp::Element* elt) const
{
    AssertI(elt);
    AssertI(elt->get_name() == "MEASURE");

    elt->set_attribute("OUTPUT", m_output);

    switch (m_type) {
    case Measure::EVENT:
        elt->set_attribute("TYPE", "event");
        break;

    case Measure::TIMED:
        elt->set_attribute("TYPE", "timed");
        elt->set_attribute("TIME_STEP", utils::to_string(m_timestep));
        break;
    }

    std::list < Observable >::const_iterator it = m_observables.begin();
    while (it != m_observables.end()) {
        (*it).write(elt);
        ++it;
    }
}

void Measure::setEventMeasure(const std::string& output)
{
    AssertI(not output.empty());

    m_output.assign(output);
    m_type = Measure::EVENT;
}

void Measure::setTimedMeasure(double timestep,
                              const std::string& output)
{
    AssertI(not output.empty());
    AssertI(timestep > 0);

    m_output.assign(output);
    m_type = Measure::TIMED;
    m_timestep = timestep;
}

void Measure::addObservable(const Observable& o)
{
    std::list < Observable >::const_iterator it = m_observables.begin();
    while (it != m_observables.end()) {
        if ((*it).modelname() == o.modelname() and
            (*it).portname() == o.portname()) {
            Throw(utils::InternalError,
                  boost::format("Add already existing Observable %1% %2%\n") %
                  o.modelname() % o.portname());
        }


        ++it;
    }
    m_observables.push_back(o);
}

void Measure::addObservable(const std::string& modelname,
                            const std::string& portname,
                            const std::string& group,
                            int index)
{
    vpz::Observable o;
    o.setObservable(modelname, portname, group, index);
    addObservable(o);
}

void Measure::delObservable(const std::string& modelname,
                            const std::string& portname,
                            const std::string& group,
                            int index)
{
    Observable o;
    o.setObservable(modelname, portname, group, index);

    std::list < Observable >::iterator it;
    it = std::find(m_observables.begin(), m_observables.end(), o);

    if (it != m_observables.end()) {
        m_observables.erase(it);
    }
}

const Observable& Measure::getObservable(const std::string& modelname,
                                         const std::string& portname,
                                         const std::string& group,
                                         int index) const
{
    Observable o;
    o.setObservable(modelname, portname, group, index);

    std::list < Observable >::const_iterator it;
    it = std::find(m_observables.begin(), m_observables.end(), o);

    Assert(utils::InternalError, it != m_observables.end(),
           boost::format("Observable not found, name %1%, port %2%\n") %
           modelname % portname);

    return *it;
}

bool Measure::existObservable(const std::string& modelname,
                              const std::string& portname,
                              const std::string& group,
                              int index) const
{
    Observable o;
    o.setObservable(modelname, portname, group, index);

    std::list < Observable >::const_iterator it;
    it = std::find(m_observables.begin(), m_observables.end(), o);

    return it != m_observables.end();
}

}} // namespace vle vpz
