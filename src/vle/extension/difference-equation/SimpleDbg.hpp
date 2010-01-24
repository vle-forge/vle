/**
 * @file vle/extension/difference-equation/SimpleDbg.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
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


#ifndef VLE_EXTENSION_DIFFERENCE_EQUATION_SIMPLE_DBG_HPP
#define VLE_EXTENSION_DIFFERENCE_EQUATION_SIMPLE_DBG_HPP 1

#include <vle/extension/difference-equation/Simple.hpp>
#include <vle/utils/Trace.hpp>

namespace vle { namespace extension { namespace DifferenceEquation {

template < typename UserModel >
class SimpleDbg : public UserModel
{
public:
    SimpleDbg(const devs::DynamicsInit& model,
              const devs::InitEventList& events)
        : UserModel(model, events)
    {
        TraceExtension(fmt(_("                     %1% [DE] constructor"))
                       % UserModel::getModelName());
    }

    virtual ~SimpleDbg() {}

    virtual double compute(const vle::devs::Time& time)
    {
        TraceExtension(fmt(_("%1$20.10g %2% [DE] compute with: %3%"))
                       % time % UserModel::getModelName() % traceVariables());

        double result = UserModel::compute(time);

        TraceExtension(fmt(_("                .... %1% [DE] compute returns "
                             "%2%")) % UserModel::getModelName() % result);

        return result;
    }

    virtual double initValue(const vle::devs::Time& time)
    {
        TraceExtension(fmt(_("%1$20.10g %2% [DE] initValue with: %3%"))
                       % time % UserModel::getModelName() % traceVariables());

        double result = UserModel::initValue(time);

        TraceExtension(fmt(_("                .... %1% [DE] initValue "
                             "returns %2%")) % UserModel::getModelName() %
                       result);

        return result;
    }

private:
    std::string traceVariables() const
    {
        std::string line;

        {
            std::string l(" ");

            for (Simple::Values::const_iterator itv = Simple::values().begin();
                 itv != Simple::values().end(); ++itv) {
                l += (fmt("%1% ") % *itv).str();
            }
            line += (fmt(" %1% = (%2%) ;") % UserModel::name() % l).str();
        }

        for (Simple::ValuesMap::const_iterator it =
                 Simple::externalValues().begin();
             it != Base::externalValues().end(); ++it) {
            std::string l(" ");

            for (Simple::Values::const_iterator itv = it->second.begin();
                 itv != it->second.end(); ++itv) {
                l += (fmt("%1% ") % *itv).str();
            }

            bool r = Simple::receivedValues().find(it->first)->second;

            line += (fmt(" %1%[%2%] = (%3%) ;")
                     % it->first % (r ? "0" : "-1") % l).str();
        }
        return line;
    }
};

}}} // namespace vle extension DifferenceEquation

#define DECLARE_DIFFERENCE_EQUATION_SIMPLE_DBG(mdl)                     \
    extern "C" {                                                        \
        vle::devs::Dynamics*                                            \
        makeNewDynamics(const vle::devs::DynamicsInit& init,            \
                        const vle::devs::InitEventList& events)         \
        {                                                               \
            return new vle::extension::DifferenceEquation::SimpleDbg    \
                < mdl >(init, events);                                  \
        }                                                               \
    }

#define DECLARE_NAMED_DIFFERENCE_EQUATION_SIMPLE_DBG(name, mdl)         \
    extern "C" {                                                        \
        vle::devs::Dynamics*                                            \
        makeNewDynamics##name(const vle::devs::DynamicsInit& init,      \
                              const vle::devs::InitEventList& events)   \
        {                                                               \
            return new vle::extension::DifferenceEquation::SimpleDbg    \
                < mdl >(init, events);                                  \
        }                                                               \
    }

#endif
