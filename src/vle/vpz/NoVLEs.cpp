/** 
 * @file vpz/NoVLEs.cpp
 * @brief 
 * @author The vle Development Team
 * @date mer, 22 fév 2006 12:23:26 +0100
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

#include <vle/vpz/NoVLEs.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

//void NoVLEs::init(xmlpp::Element* elt)
//{
//AssertI(elt);
//AssertI(elt->get_name() == "NO_VLES");
//
//m_novles.clear();
//
//xmlpp::Node::NodeList lst = elt->get_children("NO_VLE");
//xmlpp::Node::NodeList::iterator it;
//for (it = lst.begin(); it != lst.end(); ++it) {
//NoVLE novle;
//novle.init((xmlpp::Element*)(*it));
//addNoVLE(xml::get_attribute((xmlpp::Element*)(*it),
//"MODEL_NAME"), novle);
//}
//}

void NoVLEs::write(std::ostream& out) const
{
    out << "<no_vles>";

    std::map < std::string, NoVLE >::const_iterator it;
    for (it = m_novles.begin(); it != m_novles.end(); ++it) {
        out << "<no_vle translator=\"" << (*it).second.translatorname() 
            << "\" model_name=\"" << (*it).first << "\" >";
        (*it).second.write(out);
        out << "</no_vle>";
    }

    out << "</no_vles>";
}

void NoVLEs::addNoVLE(const std::string& modelname, const NoVLE& novle)
{
    AssertI(m_novles.find(modelname) == m_novles.end());

    m_novles[modelname] = novle;
}

void NoVLEs::clear()
{
    m_novles.clear();
}

void NoVLEs::delNoVLE(const std::string& modelname)
{
    std::map < std::string, NoVLE >::iterator it;
    if ((it = m_novles.find(modelname)) != m_novles.end()) {
        m_novles.erase(it);
    }
}

void NoVLEs::fusion(Model& /* model */, Dynamics& dynamics,
                    Experiment& experiment)
{
    std::map < std::string, NoVLE >::iterator it;
    for (it = m_novles.begin(); it != m_novles.end(); ++it) {
        Model m;
        Dynamics d;
        Experiment e;

        (*it).second.callTranslator(m, d, e);          
        //model.addModel((*it).first, m); // FIXME
        dynamics.addDynamics(d);
        experiment.addMeasures(e.measures());
        experiment.addConditions(e.conditions());
    }
}

const NoVLE& NoVLEs::find(const std::string& novle) const
{
    std::map < std::string, NoVLE >::const_iterator it = m_novles.find(novle);
    AssertI(it != m_novles.end());
    return (*it).second;
}

NoVLE& NoVLEs::find(const std::string& novle)
{
    std::map < std::string, NoVLE >::iterator it = m_novles.find(novle);
    AssertI(it != m_novles.end());
    return (*it).second;
}

}} // namespace vle vpz

