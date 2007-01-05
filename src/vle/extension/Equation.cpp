/** 
 * @file extension/Equation.cpp
 * @brief 
 * @author The vle Development Team
 * @date ven, 27 oct 2006 00:06:29 +0200
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

#include <vle/extension/Equation.hpp>
#include <vle/value/Double.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>
#include <cassert>

namespace vle { namespace extension {

    bool Equation::parseXML(xmlpp::Element* p_dynamicsNode)
    {
        std::vector <std::string > v_list;

        // EQUATIONS node
        xmlpp::Element* v_equationNode = utils::xml::get_children(p_dynamicsNode,
                                                                  "EQUATIONS");

        if (!v_equationNode)
            throw utils::ParseError("Excepted EQUATIONS tag.");

        xmlpp::Node::NodeList lst = v_equationNode->get_children("EQUATION");
        xmlpp::Node::NodeList::iterator it = lst.begin();

        m_equationNumber = 0;
        while ( it != lst.end() )
        {
            xmlpp::Element * elt = ( xmlpp::Element* )( *it );
            std::string v_name = utils::xml::get_attribute(elt,"NAME");

            v_list.push_back(v_name);
            ++it;
            ++m_equationNumber;
        }
        m_nameList = new std::string[m_equationNumber];
        m_valueList = new double[m_equationNumber];
        for (unsigned int i = 0; i < m_equationNumber ; i++)
        {
            m_nameList[i] = v_list[i];
            m_valueList[i] = 0;
            m_indexList[v_list[i]] = i;
        }

        // VARIABLES node
        xmlpp::Element* v_variableNode = utils::xml::get_children(p_dynamicsNode,
                                                                  "VARIABLES");

        if (!v_variableNode)
            throw utils::ParseError("Excepted VARIABLES tag.");

        m_number = -1;
        if (utils::xml::exist_attribute(v_variableNode,"NUMBER"))
            m_number = utils::to_int(utils::xml::get_attribute(v_variableNode,"NUMBER"));

        lst = v_variableNode->get_children("VARIABLE");
        it = lst.begin();
        m_variableNumber = 0;
        while ( it != lst.end() )
        {
            xmlpp::Element * elt = ( xmlpp::Element* )( *it );
            std::string v_name = utils::xml::get_attribute(elt,"NAME");

            m_variableNameList.push_back(v_name);
            m_variable[v_name] = 0;
            ++it;
            ++m_variableNumber;
        }
        if (m_number == -1) m_number = m_variableNumber;
        return true;
    }

    void Equation::initEquation(unsigned int p_index,double p_value)
    {
        m_valueList[p_index] = p_value;
    }

    void Equation::initVariable(std::string const & p_name,double p_value)
    {
        m_variable[p_name] = p_value;
    }

    bool Equation::existVariable(std::string const & p_name) const
    {
        return m_variable.find(p_name) != m_variable.end();
    }

    bool Equation::existEquation(std::string const & p_name) const
    {
        return m_indexList.find(p_name) != m_indexList.end();
    }

    double Equation::getVariable(std::string const & p_name) const
    {
        //*** -> Assertion
        assert(m_variable.find(p_name) != m_variable.end());
        //*** <- Assertion

        return m_variable.find(p_name)->second;
    }


    std::string Equation::getVariableName(unsigned int p_index) const
    {
        std::vector < std::string >::const_iterator it = m_variableNameList.begin();
        unsigned int i = 0;

        while (i != p_index && it != m_variableNameList.end())
        {
            ++i;
            ++it;
        }
        return *it;
    }


    void Equation::setVariable(std::string const & p_name,double p_value)
    {
        //*** -> Assertion
        assert(m_variable.find(p_name) != m_variable.end());
        //*** <- Assertion

        m_variable[p_name] = p_value;
    }

    void Equation::setEquation(unsigned int p_index, double p_value)
    { 
        m_valueList[p_index] = p_value; 
    }

    //***********************************************************************
    //***********************************************************************
    //
    //  DEVS Methods
    //
    //***********************************************************************
    //***********************************************************************

    devs::Time Equation::init()
    {
        m_var = 0;
        if (m_variableNumber > 0) 
        {
            m_state = INIT;
            return devs::Time::infinity;
        }
        else 
        {
            m_state = RUN;
            return devs::Time(0);
        }
    }

    devs::Time Equation::getTimeAdvance()
    {
        if (m_state == INIT) return devs::Time::infinity;
        else 
            if (m_state == RUN) return devs::Time(0);
            else return devs::Time(1);
    }

    devs::ExternalEventList* Equation::getOutputFunction(devs::Time const & time)
    {
        devs::ExternalEventList* v_eventList=new devs::ExternalEventList();

        if (m_state == RUN)
            for (unsigned int i = 0; i < m_equationNumber ; i++)
            {
                devs::ExternalEvent* e = new devs::ExternalEvent(m_nameList[i],
                                                                 time,
                                                                 getModel());

                e << devs::attribute(m_nameList[i],m_valueList[i]);
                v_eventList->addEvent(e);
            }
        return v_eventList;
    }


    void Equation::processInternalEvent(devs::InternalEvent* )
    {
        if (m_variableNumber > 0)
        {
            if (m_state == RUN) m_state = NEXT;
            else
                if (m_state == NEXT) m_state = INIT;
        }
        else 
            if (m_state == RUN) 
            {
                compute();
                m_state = NEXT;
            }
            else
                if (m_state == NEXT) m_state = RUN;
    }


    void Equation::processExternalEvent(devs::ExternalEvent* event)
    {
        std::string v_name = event->getPortName();
        double v_value = event->getDoubleAttributeValue(v_name);

        if (existVariable(v_name)) 
        {
            setVariable(v_name,v_value);
            ++m_var;
            if (m_var == (unsigned int)m_number) 
            {
                m_state = RUN;
                m_var = 0;
                compute();
            }
        }
        else
            if (m_indexList.find(v_name) != m_indexList.end())
                setEquation(m_indexList[v_name],v_value);
    }


    void Equation::processInitEvent(devs::InitEvent* event)
    {
        std::string v_name = event->getPortName();
        double v_value = event->getDoubleAttributeValue(v_name);

        initEquation(m_indexList[v_name],v_value);
    }


    bool Equation::processConflict(const devs::InternalEvent& /*internal*/,
                                   const devs::ExternalEventList& /*extEventlist*/)
    {
        return false;
    }

    value::Value Equation::processStateEvent(devs::StateEvent* event) const
    {
        if (existEquation(event->getPortName()))
        {
            std::map < std::string, unsigned int>::const_iterator it = m_indexList.find(event->getPortName());

            return value::DoubleFactory::create(m_valueList[it->second]);
        }
        else return value::ValueBase::empty;
    }

}} // namespace vle extension
