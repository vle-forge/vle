/*
 * Copyright 2016-2017 INRA
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.  You may
 * obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.  See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <deque>
#include <vle/devs/Dynamics.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Tuple.hpp>

namespace vd = vle::devs;
namespace vv = vle::value;
namespace vg = vle::vpz;
namespace vu = vle::utils;

class Integrator : public vd::Dynamics
{
public:
    Integrator(const vd::DynamicsInit& init, const vd::InitEventList& events)
      : vd::Dynamics(init, events)
    {
        double x_0_val;
        if (events.exist("X_0")) {
            x_0_val = vv::toDouble(events.get("X_0"));
        } else {
            Trace(context(),
                  6,
                  "%s: got no initial value : assuming 0.\n",
                  getModelName().c_str());

            x_0_val = 0;
        }

        m_current_value = x_0_val;

        m_x_dot_port_label = "X_dot";
        m_quanta_port_label = "Quanta";

        vg::ConnectionList my_list;
        my_list = getModel().getInputPortList();

        if (0 == my_list.count(m_x_dot_port_label))
            throw vu::ModellingError("Model %s has no ports %s",
                                     getModelName().c_str(),
                                     m_x_dot_port_label.c_str());

        if (0 == my_list.count(m_quanta_port_label))
            throw vu::ModellingError("Model %s has no ports %s",
                                     getModelName().c_str(),
                                     m_quanta_port_label.c_str());

        m_output_port_label = "I_out";
        my_list = getModel().getOutputPortList();
        if (my_list.size() > 0) {
            m_output_port_label = (my_list.begin())->first;
            m_has_output_port = true;
        }

        if (my_list.size() > 1) {
            Trace(context(),
                  6,
                  "Warning: multiple output ports."
                  " Will use only port %s.\n",
                  m_output_port_label.c_str());
        }
    }

    virtual ~Integrator()
    {
    }

    virtual vd::Time init(vd::Time /* time */) override
    {
        m_state = INIT;
        return 0;
    }

    virtual void externalTransition(const vd::ExternalEventList& events,
                                    vd::Time time) override
    {
        double up_val, down_val;
        double x_dot_val;

        auto it = events.begin();
        while (it != events.end()) {
            if (m_quanta_port_label == (*it).getPortName()) {
                up_val = (*it).getMap().getDouble("up");
                m_upthreshold = up_val;
                down_val = (*it).getMap().getDouble("down");
                m_downthreshold = down_val;
                if (WAIT_FOR_QUANTA == m_state)
                    m_state = RUNNING;
                if (WAIT_FOR_BOTH == m_state)
                    m_state = WAIT_FOR_X_DOT;
            }
            if (m_x_dot_port_label == (*it).getPortName()) {
                x_dot_val = (*it).getMap().getDouble("d_val");
                record_t record;
                record.date = time;
                record.x_dot = x_dot_val;
                archive.push_back(record);
                if (WAIT_FOR_X_DOT == m_state)
                    m_state = RUNNING;
                if (WAIT_FOR_BOTH == m_state)
                    m_state = WAIT_FOR_QUANTA;
            }
            ++it;
        }

        if (RUNNING == m_state) {
            m_current_value = current_value(time);
            m_expected_value = expected_value(time);
        }
    }

    void internalTransition(vd::Time time) override
    {
        record_t record;
        switch (m_state) {
        case RUNNING:
            m_last_output_value = m_expected_value;
            m_last_output_date = time;
            double last_derivative_value;
            last_derivative_value = archive.back().x_dot;
            archive.clear();
            record.date = time;
            record.x_dot = last_derivative_value;
            archive.push_back(record);
            m_current_value = m_expected_value;
            m_state = WAIT_FOR_QUANTA;
            break;
        case INIT:
            m_state = WAIT_FOR_BOTH;
            m_last_output_value = m_current_value;
            m_last_output_date = time;
            break;
        default:
            throw vu::ModellingError(
              "Integrator %s tries an internal transition in state %d",
              getModelName().c_str(),
              m_state);
        }
    }

    virtual void output(vd::Time /*time*/,
                        vd::ExternalEventList& output) const override
    {
        if (m_has_output_port) {
            double outval;
            switch (m_state) {
            case RUNNING:
                outval = m_expected_value;
                output.emplace_back(m_output_port_label);
                output.back().addMap().addDouble("d_val", outval);
                break;
            case INIT:
                outval = m_current_value;
                output.emplace_back(m_output_port_label);
                output.back().addMap().addDouble("d_val", outval);
                break;
            default:
                throw vu::ModellingError(
                  "Integrator %s tries an output transition in state %d ",
                  getModelName().c_str(),
                  m_state);
            }
        }
    }

    virtual vd::Time timeAdvance() const override
    {
        double current_derivative;

        switch (m_state) {
        case RUNNING:
            if (archive.size() < 1)
                throw vu::ModellingError(
                  "Integrator %s in state RUNNING without x dot "
                  "value aviable ",
                  getModelName().c_str());

            current_derivative = archive.back().x_dot;

            if (0 == current_derivative)
                return vd::infinity;

            if (current_derivative > 0) {
                if ((m_upthreshold - m_current_value) < 0)
                    throw vu::ModellingError(
                      "Integrator %s erroneous Ta computation with "
                      "positive x dot (%f)",
                      getModelName().c_str(),
                      current_derivative);

                return (m_upthreshold - m_current_value) / current_derivative;
            } else {
                if ((m_downthreshold - m_current_value) > 0)
                    throw vu::ModellingError(
                      "Integrator %s erroneous Ta computation with "
                      "negative x dot (%f)",
                      getModelName().c_str(),
                      current_derivative);

                return (m_downthreshold - m_current_value) /
                       current_derivative;
            }
            break;
        default:
            return vd::infinity;
            break;
        }
    }

    void confluentTransitions(vd::Time time,
                              const vd::ExternalEventList& externals) override
    {
        internalTransition(time);
        externalTransition(externals, time);
    }

    std::unique_ptr<vv::Value> observation(
      const vd::ObservationEvent& event) const override
    {
        if (event.onPort("value"))
            return vle::value::Double::create(current_value(event.getTime()));

        vd::Time observation_time;
        double value;
        observation_time = event.getTime();
        value = current_value(observation_time);

        auto t = std::unique_ptr<vv::Tuple>(new vv::Tuple());
        t->add(value);
        t->add(m_last_output_value);
        t->add(observation_time == m_last_output_date);
        return std::move(t);
    }

private:
    typedef enum {
        INIT,
        WAIT_FOR_QUANTA,
        WAIT_FOR_X_DOT,
        WAIT_FOR_BOTH,
        RUNNING
    } State;
    State m_state;

    std::string m_x_dot_port_label;
    std::string m_quanta_port_label;
    std::string m_output_port_label;
    bool m_has_output_port;

    vd::Time m_last_output_date;

    double m_upthreshold;
    double m_downthreshold;

    double m_last_output_value;
    double m_current_value;
    double m_expected_value;

    struct record_t
    {
        double x_dot;
        vd::Time date;
    };

    std::deque<record_t> archive;

    double current_value(vd::Time time) const
    {
        double val;

        val = m_last_output_value;
        if (archive.size() > 0) {
            for (size_t i = 0; i < archive.size() - 1; i++) {
                val +=
                  (archive[i + 1].date - archive[i].date) * archive[i].x_dot;
            }
            val += (time - archive.back().date) * archive.back().x_dot;
        }
        return val;
    }

    double expected_value(vd::Time /* time */) const
    {
        double current_derivative;
        current_derivative = archive.back().x_dot;
        if (0 == current_derivative) {
            return m_current_value;
        } else if (current_derivative > 0) {
            return m_upthreshold;
        }
        // if (current_derivative<0)
        return m_downthreshold;
    }
};

DECLARE_DYNAMICS(Integrator)
