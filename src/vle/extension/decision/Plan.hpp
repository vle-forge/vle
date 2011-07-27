/*
 * @file vle/extension/decision/Plan.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
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


#ifndef VLE_EXTENSION_DECISION_PLAN_HPP
#define VLE_EXTENSION_DECISION_PLAN_HPP 1

#include <vle/extension/DllDefines.hpp>
#include <vle/extension/decision/Activities.hpp>
#include <vle/extension/decision/Facts.hpp>
#include <vle/extension/decision/Rules.hpp>
#include <vle/utils/Parser.hpp>
#include <string>
#include <istream>

namespace vle { namespace extension { namespace decision {

class KnowledgeBase;

/**
 * @brief A Plan stores Rules, Activites (with the PrecedencesGraph). The
 * functions Facts, Predicates, AcknowledgeFunctions, OutputFunctions and
 * UpdateFunctions must be defined.
 */
class VLE_EXTENSION_EXPORT Plan
{
public:
    Plan(KnowledgeBase& kb)
        : mKb(kb)
    {}

    Plan(KnowledgeBase& kb, const std::string& buffer);

    Plan(KnowledgeBase& kb, std::istream& stream);

    void fill(const std::string& buffer);

    void fill(std::istream& stream);

    const Rules& rules() const { return mRules; }
    const Activities& activities() const { return mActivities; }
    Rules& rules() { return mRules; }
    Activities& activities() { return mActivities; }

private:
    void fill(const utils::Block& root);
    void fillRules(const utils::Block::BlocksResult& rules);
    void fillActivities(const utils::Block::BlocksResult& activities);
    void fillTemporal(const utils::Block::BlocksResult& temporals,
                      Activity& activity);
    void fillPrecedences(const utils::Block::BlocksResult& precedences);

    KnowledgeBase& mKb;
    Rules mRules;
    Activities mActivities;
};

}}} // namespace vle model decision

#endif
