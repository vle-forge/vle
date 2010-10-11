/*
 * @file vle/value/Deleter.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#include <vle/value/Deleter.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/XML.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/Matrix.hpp>

namespace vle { namespace value {

void deleter(std::stack < Value* >& composite)
{
    while (not composite.empty()) {
        Value* val = composite.top();
        composite.pop();

        switch (val->getType()) {
        case Value::SET:
            for (Set::iterator it = static_cast < Set* >
                 (val)->begin(); it != static_cast < Set* >
                 (val)->end(); ++it) {
                if (*it) {
                    if (Value::isComposite(*it)) {
                        composite.push(*it);
                    } else {
                        delete *it;
                    }
                    *it = 0;
                }
            }
            static_cast < Set* >(val)->value().clear();
            break;
        case Value::MAP:
            for (Map::iterator it = static_cast < Map* >
                 (val)->begin(); it != static_cast < Map* >
                 (val)->end(); ++it) {
                if (it->second) {
                    if (Value::isComposite(it->second)) {
                        composite.push(it->second);
                    } else {
                        delete it->second;
                    }
                    it->second = 0;
                }
            }
            static_cast < Map* >(val)->value().clear();
            break;
        case Value::MATRIX:
            for (Matrix::size_type row = 0; row < static_cast <
                 Matrix* > (val)->rows(); ++row) {
                for (Matrix::size_type col = 0; col < static_cast <
                     Matrix* > (val)->columns(); ++col) {
                    if (static_cast < Matrix* >
                        (val)->value()[col][row]) {
                        if (Value::isComposite(static_cast < Matrix* >
                                               (val)->value()[col][row])) {
                            composite.push(static_cast < Matrix* >
                                           (val)->value()[col][row]);
                        } else {
                            delete static_cast < Matrix* >
                                (val)->value()[col][row];
                        }
                        static_cast < Matrix* >
                            (val)->value()[col][row] = 0;
                    }
                }
            }
            break;
        default:
            break;
        }
        delete val;
    }
}

}} // namespace vle value

