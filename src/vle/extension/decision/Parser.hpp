/*
 * @file vle/extension/decision/Parser.hpp
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


#ifndef VLE_EXTENSION_DECISION_PARSER_HPP
#define VLE_EXTENSION_DECISION_PARSER_HPP 1

#include <vle/extension/DllDefines.hpp>
#include <vle/extension/decision/KnowledgeBase.hpp>
#include <vle/extension/decision/Plan.hpp>

namespace vle { namespace extension { namespace decision {

class KnowledgeBase;

class VLE_EXTENSION_EXPORT Parser
{
public:
    /**
     * @brief Build a boost::Spirit parser, fill the structure Plan from the
     * string an try to assign the Plan to the KnowledgeBase.
     *
     * @param content The content to transform into KnowledgeBase.
     *
     * @throw utils::ArgError if an error appear.
     */
    Parser(const std::string& content);

    /**
     * @brief Build a boost::Spirit parser, fill the structure Plan from the
     * stream an try to assign the Plan to the KnowledgeBase.
     *
     * @param stream An input stream to transform into KnowledgeBase.
     *
     * @throw utils::ArgError if an error appear.
     */
    Parser(std::istream& stream);


    const Plan& plan() const { return mPlan; }
    Plan& plan() { return mPlan; }

private:
    Plan mPlan;
};

}}} // namespace vle model decision

#endif
