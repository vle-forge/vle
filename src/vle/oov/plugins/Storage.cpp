/**
 * @file vle/oov/plugins/Storage.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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


#include <vle/oov/plugins/Storage.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Null.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace vle { namespace oov { namespace plugin {

Storage::Storage(const std::string& location) :
    Plugin(location),
    nb_col(1),
    nb_row(0),
    m_time(-1.0),
    m_isstart(false)    
{
}

Storage::~Storage()
{
}

bool Storage::isSerializable() const
{
    return true;
}

value::Value Storage::serialize() const
{
    value::Set result(value::SetFactory::create());
    result->addValue(value::IntegerFactory::create(nb_col));
    result->addValue(value::IntegerFactory::create(nb_row));
    result->addValue(value::DoubleFactory::create(m_time));

    value::Set id(value::SetFactory::create());
    for (MapPairIndex::const_iterator it = col_access.begin();
         it != col_access.end(); ++it) {
        id->addValue(value::StringFactory::create(it->first.first));
        id->addValue(value::StringFactory::create(it->first.second));
        id->addValue(value::IntegerFactory::create(it->second));
    }
    result->addValue(id);

    value::Set tab(value::SetFactory::create());
    for (ArrayValue::index y = 0; y < nb_row; ++y) {
        value::Set rowdata(value::SetFactory::create());
        for (ArrayValue::index x = 0; x < nb_col; ++x) {
            if (m_values[x][y].get()) {
                rowdata->addValue(m_values[x][y]);
            } else {
                rowdata->addValue(value::NullFactory::create());
            }
        }
        tab->addValue(rowdata);
    }

    result->addValue(tab);

    return result;
}

void Storage::deserialize(value::Value& vals)
{
    col_access.clear();

    value::Set result(value::toSetValue(vals));
    Assert(utils::ArgError, result->size() == 5, "Bad Storage deserialize flows");

    nb_col = value::toInteger(result->getValue(0));
    nb_row = value::toInteger(result->getValue(1));
    m_time = value::toDouble(result->getValue(2));

    value::Set id(value::toSetValue(result->getValue(3)));
    Assert(utils::ArgError,
           id->size() == boost::numeric_cast < size_t >((nb_col - 1)  * 3),
           (boost::format("Bad column (%1%) number for ids (%2%)") % id->size()
            % ((nb_col - 1) * 3)));

    {
        value::VectorValue::const_iterator it = id->begin();
        while (it != id->end()) {
            value::VectorValue::const_iterator model(it);
            value::VectorValue::const_iterator port(it + 1);
            value::VectorValue::const_iterator value(it + 2);
            PairString colref(value::toString(*model), value::toString(*port));
            col_access[colref] = value::toInteger(*value);
            it = it + 3;
        }
    }

    value::Set tab(value::toSetValue(result->getValue(4)));
    Assert(utils::ArgError,
           tab->size() == boost::numeric_cast < size_t >(nb_row),
           "Bad row number of tabs");

    m_values.resize(extents[nb_col][nb_row]);

    {
        value::VectorValue::iterator jt = tab->begin();
        for (ArrayValue::index y = 0; y < nb_row; ++y) {
            value::Set rowdata(value::toSetValue(*jt));

            Assert(utils::ArgError,
                  rowdata->size() == boost::numeric_cast < size_t >(nb_col),
                  (boost::format("Bad column number (%1%) for tab (%2%)") %
                   rowdata->size() % nb_col));

            value::VectorValue::iterator it = rowdata->begin();
            for (ArrayValue::index x = 0; x < nb_col; ++x) {
                if ((*it)->isNull()) {
                    m_values[x][y] = value::Value();
                } else {
                    m_values[x][y] = *it;
                }
                ++it;
            }
            ++jt;
        }
    }
}

std::string Storage::name() const
{
    return std::string("storage");
}

void Storage::onParameter(const vpz::ParameterTrame& /* trame */)
{
}

void Storage::onNewObservable(const vpz::NewObservableTrame& trame)
{
    MapVectorString::const_iterator it;
    PairString colref(trame.name(),trame.port());
    
    it = m_info.find(trame.name());
    
    if (it == m_info.end()){
	m_info[trame.name()] = VectorString();
	model_names.push_back(trame.name());
    }

    m_info[trame.name()].push_back(trame.port());
    col_access[colref] = nb_col;
    nb_col++;
    m_values.resize(extents[nb_col][nb_row + 1]);
}
    
void Storage::onDelObservable(const vpz::DelObservableTrame& /* trame */)
{
}

void Storage::onValue(const vpz::ValueTrame& trame)
{
    if (m_isstart) {
        nextTime(utils::to_double(trame.time()));
    } else {
        if (m_time < .0) {
            m_time = utils::to_double(trame.time());
        } else {
            nextTime(utils::to_double(trame.time()));
            m_isstart = true;
        }
    }
    
    for (vpz::ModelTrameList::const_iterator it = trame.trames().begin();
         it != trame.trames().end(); ++it) {
	PairString colref(it->simulator(), it->port());
	ArrayValue::index ic = col_access[colref];
	m_values[ic][nb_row] = it->value();
    }
}
    
void Storage::close(const vpz::EndTrame& /*trame*/)
{
    nb_row++;
    m_values[0][nb_row - 1] = value::DoubleFactory::create(m_time);
}

Storage::ArrayView Storage::getTime()
{   
    return m_values[boost::indices[0][Range(0, nb_row)]];
}

Storage::ArrayView Storage::getValue(const std::string& model,
				       const std::string& port)
{
    PairString colref(model,port);
    MapPairIndex::const_iterator it = col_access.find(colref);

    Assert(utils::ArgError, it != col_access.end(), boost::format(
            "Pair (model, port) (%1%,%2) does not exist.") % model % port);
    
    ArrayValue::index ic = it -> second;
    return m_values[boost::indices[ic][Range(0, nb_row)] ];
}

void Storage::nextTime(double trame_time)
{   
    if (trame_time != m_time) {
	m_values[0][nb_row] = value::DoubleFactory::create(m_time);
	nb_row++;
	m_values.resize(extents[nb_col][nb_row + 1]);
	m_time = trame_time;
    }
}

const std::vector<value::Value>::size_type Storage::getVectorSize() const 
{
    return nb_row;
}

const Storage::VectorString& Storage::getModelList() const
{
    return model_names;
}

const Storage::VectorString& Storage::getPortList(const std::string& model) const
{
    MapVectorString::const_iterator it = m_info.find(model);

    Assert(utils::ArgError, it != m_info.end(),
	    boost::format("Model %1% does not exist.") % model);

    return it->second;
}

}}} // namespace vle oov plugin
