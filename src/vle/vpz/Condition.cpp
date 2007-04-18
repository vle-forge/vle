/** 
 * @file vpz/Condition.cpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:28:49 +0100
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

#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/value/Value.hpp>
#include <vle/vpz/Condition.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

Condition::Condition() :
    Base()
{
}

Condition::Condition(const Condition& cond) :
    Base(cond),
    m_modelname(cond.modelname()),
    m_portname(cond.portname()),
    m_value(cond.value())
{
}

Condition& Condition::operator=(const Condition& cond)
{
    if (this != &cond) {
        clearValue();
        m_modelname.assign(cond.modelname());
        m_portname.assign(cond.portname());

        std::vector < value::Value >::const_iterator it;
        for (it = cond.value().begin(); it != cond.value().end(); ++it) {
            m_value.push_back((*it)->clone());
        }
    }
    return *this;
}

Condition::~Condition()
{
    clearValue();
}

void Condition::write(std::ostream& out) const
{
    out << "<condition model_name=\""
        << m_modelname
        << "\" port_name=\""
        << m_portname
        << "\"";

    std::vector < value::Value >::const_iterator it;
    for (it = m_value.begin(); it != m_value.end(); ++it) {
        out << (*it)->toXML();
    }

    out << "</condition>";
}

void Condition::setCondition(const std::string& modelname,
                             const std::string& portname)
{
    AssertI(not modelname.empty());
    AssertI(not portname.empty());

    m_modelname.assign(modelname);
    m_portname.assign(portname);
}

void Condition::addValue(value::Value val)
{
    m_value.push_back(val);
}

void Condition::clearValue()
{
    m_value.clear();
}

value::Value Condition::firstValue() const
{
    Assert(utils::InternalError, not m_value.empty(),
           boost::format("Condition %1% %2% have no firstValue.\n") %
           m_modelname % m_portname);

    //FIXME
    //return m_value[0]->clone();
    return m_value[0]->clone();
}

value::Value Condition::nValue(size_t i) const
{
    Assert(utils::InternalError, i < m_value.size(),
           boost::format("Condition %1% %2% have only %3% and no %4% Value\n") %
           m_modelname % m_portname % m_value.size() % i);

    //FIXME
    //return m_value[i]->clone();
    return m_value[i]->clone();
}

}} // namespace vle vpz
