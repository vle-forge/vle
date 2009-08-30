/**
 * @file vle/value/Deleter.hpp
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


#ifndef VLE_VALUE_DELETER_HPP
#define VLE_VALUE_DELETER_HPP

#include <vle/value/Value.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/XML.hpp>
#include <vle/value/DllDefines.hpp>
#include <stack>

namespace vle { namespace value {

    inline void deleter(std::stack < Value* >& composite)
    {
        while (not composite.empty()) {
            Value* val = composite.top();
            composite.pop();

            switch (val->getType()) {
            case Value::BOOLEAN:
            case Value::INTEGER:
            case Value::DOUBLE:
            case Value::NIL:
            case Value::STRING:
            case Value::TUPLE:
            case Value::TABLE:
            case Value::XMLTYPE:
                delete val;
                break;
            case Value::SET:
                {
                    for (Set::iterator it = static_cast < Set* >
                         (val)->begin(); it != static_cast < Set* >
                         (val)->end(); ++it) {
                        if (*it) {
                            if (Value::isComposite(*it)) {
                                composite.push(*it);
                            } else {
                                delete *it;
                                *it = 0;
                            }
                        }
                    }
                    static_cast < Set* >(val)->value().clear();
                    delete val;
                }
                break;
            case Value::MAP:
                {
                    for (Map::iterator it = static_cast < Map* >
                         (val)->begin(); it != static_cast < Map* >
                         (val)->end(); ++it) {
                        if (it->second) {
                            if (Value::isComposite(it->second)) {
                                composite.push(it->second);
                            } else {
                                delete it->second;
                                it->second = 0;
                            }
                        }
                    }
                    static_cast < Map* >(val)->value().clear();
                    delete val;
                }
                break;
            case Value::MATRIX:
                {
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
                                    static_cast < Matrix* >
                                        (val)->value()[col][row] = 0;
                                }
                            }
                        }
                    }
                    delete val;
                }
                break;
            }
        }
    }

}} // namespace vle value

#endif
