/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
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

#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/vpz/SaxStackValue.hpp>

namespace vle {
namespace vpz {

bool ValueStackSax::isCompositeParent() const
{
    if (not m_valuestack.empty()) {
        const auto &val = m_valuestack.top();

        return val->isMap() or val->isSet() or val->isMatrix();
    }
    return false;
}

void ValueStackSax::pushInteger()
{
    if (not m_valuestack.empty()) {
        if (not isCompositeParent()) {
            throw utils::SaxParserError(_("Bad file format"));
        }
    }
}

void ValueStackSax::pushBoolean()
{
    if (not m_valuestack.empty()) {
        if (not isCompositeParent()) {
            throw utils::SaxParserError(_("Bad file format"));
        }
    }
}

void ValueStackSax::pushString()
{
    if (not m_valuestack.empty()) {
        if (not isCompositeParent()) {
            throw utils::SaxParserError(_("Bad file format"));
        }
    }
}

void ValueStackSax::pushDouble()
{
    if (not m_valuestack.empty()) {
        if (not isCompositeParent()) {
            throw utils::SaxParserError(_("Bad file format"));
        }
    }
}

void ValueStackSax::pushMap()
{
    if (not m_valuestack.empty()) {
        if (not isCompositeParent()) {
            throw utils::SaxParserError(_("Bad file format"));
        }
    }

    pushOnVectorValue<value::Map>();
}

void ValueStackSax::pushMapKey(const std::string &key)
{
    if (not m_valuestack.empty()) {
        if (not m_valuestack.top()->isMap()) {
            throw utils::SaxParserError(_("Bad file format"));
        }
    }

    m_lastkey.assign(key);
}

void ValueStackSax::pushSet()
{
    if (not m_valuestack.empty()) {
        if (not isCompositeParent()) {
            throw utils::SaxParserError(_("Bad file format"));
        }
    }

    pushOnVectorValue<value::Set>();
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
            throw utils::SaxParserError(_("Bad file format"));
        }
    }

    pushOnVectorValue<value::Matrix>(
        col, row, colmax, rowmax, colstep, rowstep);
}

void ValueStackSax::pushTuple()
{
    if (not m_valuestack.empty()) {
        if (not isCompositeParent()) {
            throw utils::SaxParserError(_("Bad file format"));
        }
    }

    pushOnVectorValue<value::Tuple>();
}

void ValueStackSax::pushTable(const size_t width, const size_t height)
{
    if (not m_valuestack.empty()) {
        if (not isCompositeParent()) {
            throw utils::SaxParserError(_("Bad file format"));
        }
    }

    pushOnVectorValue<value::Table>(width, height);
}

void ValueStackSax::pushXml()
{
    if (not m_valuestack.empty()) {
        if (not isCompositeParent()) {
            throw utils::SaxParserError(_("Bad file format"));
        }
    }
}

void ValueStackSax::pushNull()
{
    if (not m_valuestack.empty()) {
        if (not isCompositeParent()) {
            throw utils::SaxParserError(_("Bad file format"));
        }
    }
}

void ValueStackSax::popValue()
{
    if (not m_valuestack.empty()) {
        m_valuestack.pop();
    }
}

value::Value *ValueStackSax::topValue()
{
    if (m_valuestack.empty()) {
        throw utils::SaxParserError(
            _("Empty sax parser value stack for the top operation"));
    }

    return m_valuestack.top();
}

void ValueStackSax::clear()
{
    while (not m_valuestack.empty())
        m_valuestack.pop();

    m_result.clear();
}

void ValueStackSax::pushResult(std::shared_ptr<value::Value> val)
{
    m_result.push_back(std::move(val));
}

const std::shared_ptr<value::Value> &ValueStackSax::getResult(size_t i) const
{
    if (m_result.size() < i)
        throw utils::SaxParserError(
            (fmt(_("Get result value with to big index %1%.")) % i).str());

    return m_result[i];
}

const std::shared_ptr<value::Value> &ValueStackSax::getLastResult() const
{
    if (m_result.empty())
        throw utils::SaxParserError(
            _("Get last result value with empty result vector"));

    return m_result[m_result.size() - 1];
}
}
} // namespace vle vpz
