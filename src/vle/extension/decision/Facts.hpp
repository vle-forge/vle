/*
 * @file vle/extension/decision/Facts.hpp
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


#ifndef VLE_EXTENSION_DECISION_FACTS_HPP
#define VLE_EXTENSION_DECISION_FACTS_HPP 1

#include <vle/extension/DllDefines.hpp>
#include <vle/value/Value.hpp>
#include <boost/function.hpp>

namespace vle { namespace extension { namespace decision {

/**
 * @brief Fact is a function which take a value from an external event an
 * change the state of the KnowledgeBase.
 */
typedef boost::function < void (const vle::value::Value&) > Fact;

}}} // namespace vle model decision

#endif
