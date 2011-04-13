/*
 * @file vle/extension/difference-equation/SimpleDbg.hpp
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


#ifndef VLE_EXTENSION_DIFFERENCE_EQUATION_SIMPLE_DBG_HPP
#define VLE_EXTENSION_DIFFERENCE_EQUATION_SIMPLE_DBG_HPP 1

#include <vle/extension/difference-equation/Simple.hpp>
#include <vle/extension/DllDefines.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/version.hpp>

#define DECLARE_DIFFERENCE_EQUATION_SIMPLE_DBG(mdl)                     \
    extern "C" {                                                        \
        VLE_EXTENSION_EXPORT vle::devs::Dynamics*                       \
        vle_make_new_dynamics(const vle::devs::DynamicsInit& init,      \
                        const vle::devs::InitEventList& events)         \
        {                                                               \
            return new vle::extension::DifferenceEquation::SimpleDbg    \
                < mdl >(init, events);                                  \
        }                                                               \
                                                                        \
        VLE_EXTENSION_EXPORT void                                       \
        vle_api_level(boost::uint32_t* major,                           \
                      boost::uint32_t* minor,                           \
                      boost::uint32_t* patch)                           \
        {                                                               \
            *major = VLE_MAJOR_VERSION;                                 \
            *minor = VLE_MINOR_VERSION;                                 \
            *patch = VLE_PATCH_VERSION;                                 \
        }                                                               \
    }

namespace vle { namespace extension { namespace DifferenceEquation {

template < typename UserModel >
class SimpleDbg : public UserModel
{
public:
    SimpleDbg(const devs::DynamicsInit& model,
              const devs::InitEventList& events)
        : UserModel(model, events), mName(model.model().getCompleteName())
    {
        TraceExtension(fmt(_("                     %1% [DE] constructor"))
                       % mName);
    }

    virtual ~SimpleDbg() {}

    virtual double compute(const vle::devs::Time& time)
    {
        TraceExtension(fmt(_("%1$20.10g %2% [DE] compute with: %3%"))
                       % time % mName % traceVariables());

        double result = UserModel::compute(time);

        TraceExtension(fmt(_("                .... %1% [DE] compute returns "
                             "%2%")) % mName % result);

        return result;
    }

    virtual double initValue(const vle::devs::Time& time)
    {
        TraceExtension(fmt(_("%1$20.10g %2% [DE] initValue with: %3%"))
                       % time % mName % traceVariables());

        double result = UserModel::initValue(time);

        TraceExtension(fmt(_("                .... %1% [DE] initValue "
                             "returns %2%")) % mName %
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

    std::string mName;
};

}}} // namespace vle extension DifferenceEquation

#endif
