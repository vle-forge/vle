/**
 * @file src/vle/oov/OutputMatrix.cpp
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




#include <vle/oov/OutputMatrix.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Null.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace vle { namespace oov {

OutputMatrix::OutputMatrix(int columns,
                           int rows,
                           int resizeColumns,
                           int resizeRows)
{
    m_values = value::MatrixFactory::create(
        1, // one row by default for the time column.
        0, // no data by default.
        (columns == 0) ? 1 : columns, // default size, minimun 1 column.
        rows, resizeColumns, resizeRows);
}

value::Value OutputMatrix::serialize() const
{
    value::Set result(value::SetFactory::create());
    result->addValue(m_values);

    value::Set id(value::SetFactory::create());
    for (MapPairIndex::const_iterator it = m_colAccess.begin();
         it != m_colAccess.end(); ++it) {
        id->addValue(value::StringFactory::create(it->first.first));
        id->addValue(value::StringFactory::create(it->first.second));
        id->addValue(value::IntegerFactory::create(it->second));
    }
    result->addValue(id);

    return result;
}

void OutputMatrix::deserialize(value::Value vals)
{
    m_colAccess.clear();

    value::Set result(value::toSetValue(vals));
    Assert(utils::ArgError, result->size() == 2,
           "Bad OutputMatrix deserialize flows");

    m_values = value::toMatrixValue(result->getValue(0));
    value::Set id(value::toSetValue(result->getValue(1)));

    Assert(utils::ArgError, id->size() % 3 == 0,
           boost::format("Bad id's for the OutputMatrix: %1%") %
           id->toString());

    Assert(utils::ArgError, id->size() / 3 == m_values->columns() - 1,
           boost::format("Bad id's number (%2%) for matrix (%1%)") %
           m_values->columns() % id->size());

    {
        value::VectorValue::const_iterator it = id->begin();
        while (it != id->end()) {
            value::VectorValue::const_iterator model(it);
            value::VectorValue::const_iterator port(it + 1);
            value::VectorValue::const_iterator value(it + 2);
            PairString colref(value::toString(*model), value::toString(*port));
            m_colAccess[colref] = value::toInteger(*value);
            it = it + 3;
        }
    }
}

void OutputMatrix::resize(value::MatrixFactory::size_type columns,
                          value::MatrixFactory::size_type rows)
{
    m_values->resize(columns, rows);
}

void OutputMatrix::updateStep(value::MatrixFactory::size_type resizeColumns,
                              value::MatrixFactory::size_type resizeRows)
{
    m_values->setResizeColumn(resizeColumns);
    m_values->setResizeRow(resizeRows);
}

value::MatrixFactory::index OutputMatrix::addModel(const std::string& model,
                                                   const std::string& port)
{
    PairString colref(model, port);
    
    MapStringList::const_iterator it = m_info.find(model);
    if (it == m_info.end()){
	m_info[model] = StringList();
	m_modelNames.push_back(model);
    }

    m_info[model].push_back(port);
    m_colAccess[colref] = m_values->columns();
    m_values->addColumn();

    return m_values->columns();
}

void OutputMatrix::addValue(const std::string& model,
                            const std::string& port,
                            const value::Value& value)
{
    MapPairIndex::const_iterator it(m_colAccess.find(PairString(model, port)));
    Assert(utils::ArgError, it != m_colAccess.end(), boost::format(
            "OutputMatrix have no couple (%1%, %2%)") % model % port);

    m_values->addValue(it->second, m_values->rows(), value);
}

void OutputMatrix::setLastTime(double value)
{
    m_values->addValue(0, m_values->rows(), 
                       value::DoubleFactory::create(value));
    m_values->addRow();
}

value::MatrixFactory::index OutputMatrix::column(const std::string& model,
                                                 const std::string& port) const
{
    MapStringList::const_iterator it = m_info.find(model);
    Assert(utils::ArgError, it != m_info.end(), boost::format(
            "Unkown model %1%") % model);

    PairString colref(model, port);

    MapPairIndex::const_iterator jt = m_colAccess.find(colref);
    Assert(utils::ArgError, jt != m_colAccess.end(), boost::format(
            "Unknow port %1% for model %2%") % port % model);

    return jt->second;
}

value::MatrixFactory::VectorView
OutputMatrix::getValue(const std::string& model, const std::string& port)
{
    PairString colref(model,port);
    MapPairIndex::const_iterator it = m_colAccess.find(colref);

    Assert(utils::ArgError, it != m_colAccess.end(), boost::format(
            "Pair (model, port) (%1%,%2) does not exist.") % model % port);

    return m_values->column(it->second);
}

value::MatrixFactory::VectorView
OutputMatrix::getValue(value::MatrixFactory::size_type idx)
{
    Assert(utils::ArgError, idx < m_values->columns(), boost::format(
            "Too big index for the matrix: %1%/%2%") % idx
        % m_values->columns());

    return m_values->column(idx);
}

value::MatrixFactory::ConstVectorView
OutputMatrix::getValue(const std::string& model, const std::string& port) const
{
    PairString colref(model,port);
    MapPairIndex::const_iterator it = m_colAccess.find(colref);

    Assert(utils::ArgError, it != m_colAccess.end(), boost::format(
            "Pair (model, port) (%1%,%2) does not exist.") % model % port);

    return m_values->column(it->second);
}

value::MatrixFactory::ConstVectorView
OutputMatrix::getValue(value::MatrixFactory::size_type idx) const
{
    Assert(utils::ArgError, idx < m_values->columns(), boost::format(
            "Too big index for the matrix: %1%/%2%") % idx
        % m_values->columns());

    return m_values->column(idx);
}

const OutputMatrix::StringList&
OutputMatrix::getPortList(const std::string& model) const
{
    MapStringList::const_iterator it = m_info.find(model);

    Assert(utils::ArgError, it != m_info.end(),
	    boost::format("Model %1% does not exist.") % model);

    return it->second;
}

}} // namespace vle oov
