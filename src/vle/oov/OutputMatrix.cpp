/**
 * @file vle/oov/OutputMatrix.cpp
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
    m_values = value::Matrix::create(
        1, // one row by default for the time column.
        0, // no data by default.
        (columns == 0) ? 1 : columns, // default size, minimun 1 column.
        rows, resizeColumns, resizeRows);
}

OutputMatrix::~OutputMatrix()
{
    delete m_values;
}

value::Value* OutputMatrix::serialize() const
{
    value::Set* result(value::Set::create());
    result->add(m_values);

    value::Set* id(value::Set::create());
    for (MapPairIndex::const_iterator it = m_colAccess.begin();
         it != m_colAccess.end(); ++it) {
        id->add(value::String::create(it->first.first));
        id->add(value::String::create(it->first.second));
        id->add(value::Integer::create(it->second));
    }
    result->add(id);

    return result;
}

void OutputMatrix::deserialize(const value::Value& vals)
{
    m_colAccess.clear();

    const value::Set& result(vals.toSet());

    if (result.size() != 2) {
        throw utils::ArgError(_("Bad OutputMatrix deserialize flows"));
    }

    m_values = value::toMatrixValue(result.get(0).clone());
    const value::Set& id(result.get(1).toSet());

    if (id.size() % 3 != 0) {
        throw utils::ArgError(fmt(
                _("Bad id's for the OutputMatrix: '%1%'")) % id);
    }

    if (id.size() / 3 != m_values->columns() - 1) {
        throw utils::ArgError(fmt(
                _("Bad id's number (%2%) for matrix (%1%)")) %
            m_values->columns() % id.size());
    }

    {
        value::VectorValue::const_iterator it = id.begin();
        while (it != id.end()) {
            value::VectorValue::const_iterator model(it);
            value::VectorValue::const_iterator port(it + 1);
            value::VectorValue::const_iterator value(it + 2);
            PairString colref((*model)->toString().value(),
                              (*port)->toString().value());
            m_colAccess[colref] = (*value)->toInteger().value();
            it = it + 3;
        }
    }
}

void OutputMatrix::resize(value::Matrix::size_type columns,
                          value::Matrix::size_type rows)
{
    m_values->resize(columns, rows);
}

void OutputMatrix::updateStep(value::Matrix::size_type resizeColumns,
                              value::Matrix::size_type resizeRows)
{
    m_values->setResizeColumn(resizeColumns);
    m_values->setResizeRow(resizeRows);
}

value::Matrix::index OutputMatrix::addModel(const std::string& model,
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

    if (it == m_colAccess.end()) {
        throw utils::ArgError(fmt(
            _("OutputMatrix have no couple (%1%, %2%)")) % model % port);
    }

    m_values->add(it->second, m_values->rows(), value);
}

void OutputMatrix::addValue(const std::string& model,
                            const std::string& port,
                            value::Value* value)
{
    MapPairIndex::const_iterator it(m_colAccess.find(PairString(model, port)));

    if (it == m_colAccess.end()) {
        throw utils::ArgError(fmt(
                _("OutputMatrix have no couple (%1%, %2%)")) % model % port);
    }

    m_values->add(it->second, m_values->rows(), value);
}

void OutputMatrix::setLastTime(double value)
{
    value::Matrix::size_type row(m_values->rows());
    m_values->addRow();
    m_values->add(0, row, value::Double::create(value));
}

value::Matrix::index OutputMatrix::column(const std::string& model,
                                                 const std::string& port) const
{
    MapStringList::const_iterator it = m_info.find(model);

    if (it == m_info.end()) {
        throw utils::ArgError(fmt(_("Unkown model %1%")) % model);
    }

    PairString colref(model, port);

    MapPairIndex::const_iterator jt = m_colAccess.find(colref);

    if (jt == m_colAccess.end()) {
        throw utils::ArgError(fmt(
                _("Unknow port %1% for model %2%")) % port % model);
    }

    return jt->second;
}

value::VectorView
OutputMatrix::getValue(const std::string& model, const std::string& port)
{
    PairString colref(model,port);
    MapPairIndex::const_iterator it = m_colAccess.find(colref);

    if (it == m_colAccess.end()) {
        throw utils::ArgError(fmt(
                _("Pair (model, port) (%1%,%2%) does not exist.")) % model %
            port);
    }

    return m_values->column(it->second);
}

value::VectorView
OutputMatrix::getValue(value::Matrix::size_type idx)
{
    if (idx >= m_values->columns()) {
        throw utils::ArgError(fmt(
                _("Too big index for the matrix: %1%/%2%")) % idx %
            m_values->columns());
    }

    return m_values->column(idx);
}

value::ConstVectorView
OutputMatrix::getValue(const std::string& model, const std::string& port) const
{
    PairString colref(model,port);
    MapPairIndex::const_iterator it = m_colAccess.find(colref);

    if (it == m_colAccess.end()) {
        throw utils::ArgError(fmt(
                _("Pair (model, port) (%1%,%2%) does not exist.")) % model %
            port);
    }

    return m_values->column(it->second);
}

value::ConstVectorView
OutputMatrix::getValue(value::Matrix::size_type idx) const
{
    if (idx >= m_values->columns()) {
        throw utils::ArgError(fmt(
                _("Too big index for the matrix: %1%/%2%")) % idx
        % m_values->columns());
    }

    return m_values->column(idx);
}

const OutputMatrix::StringList&
OutputMatrix::getPortList(const std::string& model) const
{
    MapStringList::const_iterator it = m_info.find(model);

    if (it == m_info.end()) {
        throw utils::ArgError(fmt(_("Model %1% does not exist.")) % model);
    }

    return it->second;
}

}} // namespace vle oov
