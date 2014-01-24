/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef VLE_DEVS_EXECUTIVEDBG_HPP
#define VLE_DEVS_EXECUTIVEDBG_HPP

#include <vle/DllDefines.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/version.hpp>

#define DECLARE_EXECUTIVE_DBG(mdl)                                      \
    extern "C" {                                                        \
        VLE_MODULE vle::devs::Dynamics*                                 \
        vle_make_new_executive(const vle::devs::ExecutiveInit& init,    \
                               const vle::devs::InitEventList& events)  \
        {                                                               \
            return new vle::devs::ExecutiveDbg < mdl >(init, events);   \
        }                                                               \
                                                                        \
        VLE_MODULE void                                                 \
        vle_api_level(vle::uint32_t* major,                             \
                      vle::uint32_t* minor,                             \
                      vle::uint32_t* patch)                             \
        {                                                               \
            *major = VLE_MAJOR_VERSION;                                 \
            *minor = VLE_MINOR_VERSION;                                 \
            *patch = VLE_PATCH_VERSION;                                 \
        }                                                               \
    }

namespace vle { namespace devs {

/**
 * @brief An Executive wrapper for the Barros DEVS extension. Provides a
 * mecanism to debug an Executive class.
 */
template < typename UserModel > class ExecutiveDbg : public UserModel
{
public:
    ExecutiveDbg(const ExecutiveInit& init, const InitEventList& events)
        : UserModel(init, events), mCurrentTime(),
        mName(init.model().getCompleteName())
    {
        TraceExtension(fmt(_("                     %1% [DEVS] constructor"))
                       % mName);
    }

    virtual ~ExecutiveDbg()
    {}

    virtual Time init(const Time& time)
    {
        mCurrentTime = time;

        TraceDevs(fmt(_("%1$20.10g %2% [DEVS] init")) % time %
                  mName);

        return UserModel::init(time);
    }

    virtual void output(const Time& time, ExternalEventList& output) const
    {
        TraceDevs(fmt(_("%1$20.10g %2% [DEVS] output")) % time %
                  mName);

        UserModel::output(time, output);

        if (output.empty()) {
            TraceDevs(fmt(
                    _("                .... %1% [DEVS] output returns empty "
                      "output")) % mName);
        } else {
            TraceDevs(fmt(
                    _("                .... %1% [DEVS] output returns %2%")) %
                mName % output);
        }
    }

    virtual Time timeAdvance() const
    {
        TraceDevs(fmt(_("                     %1% [DEVS] ta")) %
                  mName);

        Time time(UserModel::timeAdvance());

        TraceDevs(fmt(_("                .... %1% [DEVS] ta returns %2%")) %
                  mName % time);

        return time;
    }

    virtual void internalTransition(const Time& time)
    {
        mCurrentTime = time;

        TraceDevs(fmt(_("%1$20.10g %2% [DEVS] internal transition")) % time %
                  mName);

        UserModel::internalTransition(time);
    }

    virtual void externalTransition(const ExternalEventList& event,
            const Time& time)
    {
        mCurrentTime = time;

        TraceDevs(fmt(_("%1$20.10g %2% [DEVS] external transition: [%3%]")) %
                  time % mName % event);

        UserModel::externalTransition(event, time);
    }

    virtual void confluentTransitions(
        const Time& time,
        const ExternalEventList& extEventlist)
    {
        TraceDevs(fmt(
                _("%1$20.10g %2% [DEVS] confluent transition: [%3%]")) % time %
            mName % extEventlist);

        UserModel::confluentTransitions(time, extEventlist);
    }

    virtual vle::value::Value* observation(
        const ObservationEvent& event) const
    {
        TraceDevs(fmt(_("%1$20.10g %2% [DEVS] observation: [from: '%3%'"
                        " port: '%4%']")) % event.getTime() % mName
                  % event.getViewName() % event.getPortName());

        return UserModel::observation(event);
    }

    virtual void finish()
    {
        TraceDevs(fmt(_("                     %1% [DEVS] finish")) %
                  mName);

        UserModel::finish();
    }

    /**
     * @brief Build a new devs::Simulator from the dynamics library. Attach
     * to this model information of dynamics, condition and observable.
     * @param model the vpz::AtomicModel reference source of
     * devs::Simulator.
     * @param dynamics the name of the dynamics to attach.
     * @param condition the name of the condition to attach.
     * @param observable the name of the observable to attach.
     * @throw utils::InternalError if dynamics not exist.
     */
    virtual const vpz::AtomicModel* createModel(
        const std::string& name,
        const std::vector < std::string >& inputs,
        const std::vector < std::string >& outputs,
        const std::string& dynamics,
        const std::vector < std::string >& conditions,
        const std::string& observable)
    {
        std::string inputsString, outputsString, conditionsString;

        {
            std::vector < std::string >::const_iterator it;
            for (it = inputs.begin(); it != inputs.end(); ++it) {
                inputsString += *it;
                if (it + 1 != inputs.end()) {
                    inputsString += ",";
                }
            }
        }
        {
            std::vector < std::string >::const_iterator it;
            for (it = outputs.begin(); it != outputs.end(); ++it) {
                outputsString += *it;
                if (it + 1 != outputs.end()) {
                    outputsString += ",";
                }
            }
        }
        {
            std::vector < std::string >::const_iterator it =
                conditions.begin();
            while (it != conditions.begin()) {
                conditionsString += *it;
                ++it;
                if (it != conditions.end()) {
                    conditionsString += ",";
                }
            }
        }

        TraceExtension(fmt(
                _("%1$20.10g %2% [EXE] createModel "
                  "name: %3%, inputs: (%4%), output: (%5%), "
                  "dynamics %6%, conditions: (%7%), observable (%8%)")) %
            mCurrentTime % mName % name % inputsString %
            outputsString % dynamics % conditionsString % observable);

        return UserModel::createModel(name, inputs, outputs, dynamics,
                                      conditions, observable);
    }

    /**
     * @brief Build a new devs::Simulator from the vpz::Classes information.
     * @param classname the name of the class to clone.
     * @param parent the parent of the model.
     * @param modelname the new name of the model.
     * @throw utils::badArg if modelname already exist.
     */
    virtual const vpz::BaseModel* createModelFromClass(
        const std::string& classname,
        const std::string& modelname)
    {
        TraceExtension(fmt(
                _("%1$20.10g %2% [EXE] createModelFromClass "
                  "class: %3%, modelname: %4%")) % mCurrentTime %
            mName % classname % modelname);

        return UserModel::createModelFromClass(classname, modelname);
    }

    /**
     * @brief Delete the specified model from coupled model. All
     * connection are deleted, Simulator are deleted and all events are
     * deleted from event table.
     * @param modelname the name of model to delete.
     */
    virtual void delModel(const std::string& modelname)
    {
        TraceExtension(fmt(
                _("%1$20.10g %2% [EXE] delModel "
                  "model: %3%")) % mCurrentTime % mName %
            modelname);

        UserModel::delModel(modelname);
    }

    virtual void addConnection(const std::string& modelsource,
                               const std::string& outputport,
                               const std::string& modeldestination,
                               const std::string& inputport)
    {
        TraceExtension(fmt(
                _("%1$20.10g %2% [EXE] addConnection "
                  "from model: %3% port: %4% "
                  "to model %5% port: %6%")) % mCurrentTime %
            mName % modelsource % outputport %
            modeldestination % inputport);

        UserModel::addConnection(modelsource, outputport, modeldestination,
                                 inputport);
    }

    virtual void removeConnection(const std::string& modelsource,
                                  const std::string& outputport,
                                  const std::string& modeldestination,
                                  const std::string& inputport)
    {
        TraceExtension(fmt(
                _("%1$20.10g %2% [EXE] removeConnection "
                  "from model: %3% port: %4% "
                  "to model %5% port: %6%")) % mCurrentTime %
            mName % modelsource % outputport %
            modeldestination % inputport);

        UserModel::removeConnection(modelsource, outputport, modeldestination,
                                    inputport);
    }

    virtual void addInputPort(const std::string& modelName,
                              const std::string& portName)
    {
        TraceExtension(fmt(
                _("%1$20.10g %2% [EXE] addInputPort "
                  "model: %3%, port: %4%")) % mCurrentTime %
            mName % modelName % portName);

        UserModel::addInputPort(modelName, portName);
    }

    virtual void addOutputPort(const std::string& modelName,
                               const std::string& portName)
    {
        TraceExtension(fmt(
                _("%1$20.10g %2% [EXE] addOutputPort "
                  "model: %3%, port: %4%")) % mCurrentTime %
            mName % modelName % portName);

        UserModel::addOutputPort(modelName, portName);
    }

    virtual void removeInputPort(const std::string& modelName,
                                 const std::string& portName)
    {
        TraceExtension(fmt(
                _("%1$20.10g %2% [EXE] removeInputPort "
                  "model: %3%, port: %4%")) % mCurrentTime %
            mName % modelName % portName);

        UserModel::removeInputPort(modelName, portName);
    }

    virtual void removeOutputPort(const std::string& modelName,
                                  const std::string& portName)
    {
        TraceExtension(fmt(
                _("%1$20.10g %2% [EXE] removeOutputPort "
                  "model: %3%, port: %4%")) % mCurrentTime %
            mName % modelName % portName);

        UserModel::removeOutputPort(modelName, portName);
    }

private:
    devs::Time mCurrentTime;
    std::string mName;
};

}} // namespace vle devs

#endif
