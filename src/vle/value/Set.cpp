/**
 * @file vle/value/Set.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/value/Set.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/XML.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/Deleter.hpp>
#include <vle/utils/Debug.hpp>
#include <boost/utility.hpp>
#include <fstream>
#include <sstream>

namespace vle { namespace value {

Set::Set(const Set& setfactory)
    : Value(setfactory)
{
    m_value.resize(setfactory.size());

    std::transform(setfactory.begin(), setfactory.end(),
                   m_value.begin(), CloneValue());
}

void Set::writeFile(std::ostream& out) const
{
    for (const_iterator it = m_value.begin(); it != m_value.end(); ++it) {
        if (it != m_value.begin()) {
            out << ",";
        }
        if (*it) {
            (*it)->writeFile(out);
        } else {
            out << "NA";
        }
    }
}

void Set::writeString(std::ostream& out) const
{
    out << "(";

    for (const_iterator it = m_value.begin(); it != m_value.end(); ++it) {
        if (it != m_value.begin()) {
            out << ",";
        }
        if (*it) {
            (*it)->writeString(out);
        } else {
            out << "NA";
        }
    }

    out << ")";
}

void Set::writeXml(std::ostream& out) const
{
    out << "<set>";

    for (const_iterator it = m_value.begin(); it != m_value.end(); ++it) {
        if (*it) {
            (*it)->writeXml(out);
        } else {
            out << "<null />";
        }
    }

    out << "</set>";
}

Value* Set::give(const size_type& i)
{
    if (i >= size()) {
        throw utils::ArgError(_("Set: too big index"));
    }

    Value* result = m_value[i];
    m_value[i] = 0;
    return result;
}

void Set::del(const size_type i)
{
    if (i >= size()) {
        throw utils::ArgError(_("Set: too big index"));
    }

    delete m_value[i];
    m_value[i] = 0;
    m_value.erase(begin() + i);
}

void Set::clear()
{
    std::stack < Value* > composite;

    for (iterator it = begin(); it != end(); ++it) {
        if (*it) {
            if (isComposite(*it)) {
                composite.push(*it);
            } else {
                delete *it;
            }
            *it = 0;
        }
    }

    m_value.clear();
    deleter(composite);
}

Set& Set::addSet()
{
    Set* value = new Set();

    m_value.push_back(value);

    return *value;
}

Map& Set::addMap()
{
    Map* value = new Map();

    m_value.push_back(value);

    return *value;
}

Matrix& Set::addMatrix()
{
    Matrix* value = new Matrix();

    m_value.push_back(value);

    return *value;
}

Set& Set::getSet(const size_type& i)
{
    value::Value& value = value::reference(get(i));

    return value::toSetValue(value);
}

Map& Set::getMap(const size_type& i)
{
    value::Value& value = value::reference(get(i));

    return value::toMapValue(value);
}

Matrix& Set::getMatrix(const size_type& i)
{
    value::Value& value = value::reference(get(i));

    return value::toMatrixValue(value);
}

const Set& Set::getSet(const size_type& i) const
{
    const value::Value& value = value::reference(get(i));

    return value::toSetValue(value);
}

const Map& Set::getMap(const size_type& i) const
{
    const value::Value& value = value::reference(get(i));

    return value::toMapValue(value);
}

const Matrix& Set::getMatrix(const size_type& i) const
{
    const value::Value& value = value::reference(get(i));

    return value::toMatrixValue(value);
}

void Set::serializeTxtFile(const Set& set, const std::string& filename)
{
    std::ofstream out(filename.c_str());
    if (not out.is_open()) {
        throw utils::ArgError(fmt(_(
                    "serialize error: cannot open file '%1%'")) % filename);
    }

    boost::archive::text_oarchive oa(out);
    value::Value::registerValues(oa);

    oa << (const value::Set&)set;
}

void Set::serializeTxtBuffer(const Set& set, std::string& buffer)
{
    std::ostringstream out;

    boost::archive::text_oarchive oa(out);
    value::Value::registerValues(oa);

    oa << (const value::Set&)set;

    buffer = out.str();
}

void Set::serializeBinaryFile(const Set& set, const std::string& filename)
{
    std::ofstream out(filename.c_str(), std::ofstream::binary);
    if (not out.is_open()) {
        throw utils::ArgError(fmt(_(
                    "serialize error: cannot open file '%1%'")) % filename);
    }

    boost::archive::text_oarchive oa(out);
    value::Value::registerValues(oa);

    oa << (const value::Set&)set;
}

void Set::serializeBinaryBuffer(const Set& set, std::string& buffer)
{
    std::ostringstream out(std::ostringstream::binary);

    boost::archive::text_oarchive oa(out);
    value::Value::registerValues(oa);

    oa << (const value::Set&)set;

    buffer = out.str();
}

void Set::deserializeTxtFile(Set& set, const std::string& filename)
{
    std::ifstream out(filename.c_str());
    if (not out.is_open()) {
        throw utils::ArgError(fmt(_(
                    "deserialize error: can not open file '%1%'")) % filename);
    }

    boost::archive::text_iarchive ia(out);
    value::Value::registerValues(ia);

    ia >> (value::Set&)set;
}

void Set::deserializeTxtBuffer(Set& set, const std::string& buffer)
{
    std::istringstream out(buffer);

    boost::archive::text_iarchive ia(out);
    value::Value::registerValues(ia);

    ia >> (value::Set&)set;
}

void Set::deserializeBinaryFile(Set& set, const std::string& filename)
{
    std::ifstream out(filename.c_str(), std::ifstream::binary);
    if (not out.is_open()) {
        throw utils::ArgError(fmt(_(
                    "deserialize error: can not open file '%1%'")) % filename);
    }

    boost::archive::text_iarchive ia(out);
    value::Value::registerValues(ia);

    ia >> (value::Set&)set;
}

void Set::deserializeBinaryBuffer(Set& set, const std::string& buffer)
{
    std::istringstream out(buffer, std::istringstream::binary);

    boost::archive::text_iarchive ia(out);
    value::Value::registerValues(ia);

    ia >> (value::Set&)set;
}

}} // namespace vle value

