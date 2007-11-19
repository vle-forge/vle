/** 
 * @file Text.cpp
 * @brief 
 * @author The vle Development Team
 * @date lun, 23 jui 2007 11:48:34 +0200
 */

/*
 * Copyright (C) 2007 - The vle Development Team
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

#include <vle/oov/plugins/Storage.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>

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
