/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/vpz/SaxStackValue.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>


namespace vle { namespace vpz {

bool ValueStackSax::isCompositeParent() const
{
    if (not m_valuestack.empty()) {
        const value::Value* val = m_valuestack.top();

        return val->isMap() or val->isSet() or val->isMatrix();
    }
    return false;
}

void ValueStackSax::pushInteger()
{
    if (not m_valuestack.empty()) {
        if (not isCompositeParent()) {
            throw utils::SaxParserError();
        }
    }
}

void ValueStackSax::pushBoolean()
{
    if (not m_valuestack.empty()) {
        if (not isCompositeParent()) {
            throw utils::SaxParserError();
        }
    }
}

void ValueStackSax::pushString()
{
    if (not m_valuestack.empty()) {
        if (not isCompositeParent()) {
            throw utils::SaxParserError();
        }
    }
}

void ValueStackSax::pushDouble()
{
    if (not m_valuestack.empty()) {
        if (not isCompositeParent()) {
            throw utils::SaxParserError();
        }
    }
}

void ValueStackSax::pushMap()
{
    if (not m_valuestack.empty()) {
        if (not isCompositeParent()) {
            throw utils::SaxParserError();
        }
    }

    pushOnVectorValue(value::Map::create());
}

void ValueStackSax::pushMapKey(const std::string& key)
{
    if (not m_valuestack.empty()) {
        if (not m_valuestack.top()->isMap()) {
            throw utils::SaxParserError();
        }
    }

    m_lastkey.assign(key);
}

void ValueStackSax::pushSet()
{
    if (not m_valuestack.empty()) {
        if (not isCompositeParent()) {
            throw utils::SaxParserError();
        }
    }

    pushOnVectorValue(value::Set::create());
}

void ValueStackSax::pushMatrix(value::Matrix::index col,
                               value::Matrix::index row,
                               value::Matrix::index colmax,
                               value::Matrix::index rowmax,
                               value::Matrix::index colstep,
                               value::Matrix::index rowstep)
{
    if (not m_valuestack.empty()) {
        if (not isCompositeParent()) {
            throw utils::SaxParserError();
        }
    }

    pushOnVectorValue(value::Matrix::create(col, row, colmax, rowmax,
                                                   colstep, rowstep));
}

void ValueStackSax::pushTuple()
{
    if (not m_valuestack.empty()) {
        if (not isCompositeParent()) {
            throw utils::SaxParserError();
        }
    }

    pushOnVectorValue(value::Tuple::create());
}

void ValueStackSax::pushTable(const size_t width, const size_t height)
{
    if (not m_valuestack.empty()) {
        if (not isCompositeParent()) {
            throw utils::SaxParserError();
        }
    }

    pushOnVectorValue(value::Table::create(width, height));
}

void ValueStackSax::pushXml()
{
    if (not m_valuestack.empty()) {
        if (not isCompositeParent()) {
            throw utils::SaxParserError();
        }
    }
}

void ValueStackSax::pushNull()
{
    if (not m_valuestack.empty()) {
        if (not isCompositeParent()) {
            throw utils::SaxParserError();
        }
    }
}

void ValueStackSax::popValue()
{
    if (not m_valuestack.empty()) {
        m_valuestack.pop();
    }
}

value::Value* ValueStackSax::topValue()
{
    if (m_valuestack.empty()) {
        throw utils::SaxParserError(
            _("Empty sax parser value stack for the top operation"));
    }

    return m_valuestack.top();
}

void ValueStackSax::pushOnVectorValue(value::Value* val)
{
    if (not m_valuestack.empty()) {
        if (m_valuestack.top()->isSet()) {
            m_valuestack.top()->toSet().add(val);
        } else if (m_valuestack.top()->isMap()) {
            m_valuestack.top()->toMap().add(m_lastkey, val);
        } else if (m_valuestack.top()->isMatrix()) {
            value::Matrix& mx(m_valuestack.top()->toMatrix());
            if (not val->isNull()) {
                mx.addToLastCell(val);
            }
            mx.moveLastCell();
        }
    } else {
        m_result.push_back(val);
    }

    if (val->isSet() or val->isMap() or val->isTuple() or val->isTable() or
        val->isMatrix()) {
        m_valuestack.push(val);
    }

    if (val->isNull()) {
        delete val;
    }
}

void ValueStackSax::clear()
{
    while (not m_valuestack.empty())
        m_valuestack.pop();

    m_result.clear();
}

}} // namespace vle vpz
