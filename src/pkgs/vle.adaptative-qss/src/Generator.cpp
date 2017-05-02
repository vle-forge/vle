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

#include <vle/devs/Dynamics.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/value/Map.hpp>

namespace vd = vle::devs;
namespace vv = vle::value;
namespace vg = vle::vpz;
namespace vu = vle::utils;

class Generator : public vd::Dynamics
{
public:
    Generator(const vd::DynamicsInit& init, const vd::InitEventList& events)
      : vd::Dynamics(init, events)
    {
        vg::ConnectionList my_list;

        // m_duration = vv::toDouble(events.get("duration"));
        m_duration = 0.1;
        if (events.end() != events.find("source_init_level")) {
            m_val = vv::toDouble(events.get("source_init_level"));
        } else {
            m_val = 1;
            Trace(context(),
                  6,
                  "Warning : Model %s got no init"
                  " output level (source_init_level) : assuming 1\n",
                  getModelName().c_str());
        }

        if (events.end() != events.find("source_trend")) {
            m_trend = vv::toDouble(events.get("source_trend"));
            if (events.end() != events.find("source_quantum")) {
                m_quantum = vv::toDouble(events.get("source_quantum"));
            } else {
                m_quantum = 0.01;
                Trace(context(),
                      6,
                      "Warning : Model %s got no output"
                      " quantum (source_quantum) : assuming 0.01\n",
                      getModelName().c_str());
            }
        } else {
            m_trend = 0;

            Trace(context(),
                  6,
                  "%s got no output trend (source_trend)"
                  " : assuming 0\n",
                  getModelName().c_str());

            // no trend => quantum is useless but..
            m_quantum = 0.01;
        }

        if (0 == m_quantum) {
            throw vu::ModellingError("Model %s has null output quantum.",
                                     getModelName().c_str());
        }

        m_has_output_port = false;
        my_list = getModel().getOutputPortList();

        if (my_list.size() > 0) {
            m_output_port_label = (my_list.begin())->first;
            m_has_output_port = true;
        }

        if (my_list.size() > 1) {
            Trace(context(),
                  6,
                  "Warning: multiple output ports."
                  " Will use only port %s\n",
                  m_output_port_label.c_str());
        }
    }

    virtual ~Generator() // This is mandatory
    {
    }

    virtual vd::Time init(vd::Time /* time */) override
    {
        return 0;
    }

    virtual void output(vd::Time time,
                        vd::ExternalEventList& output) const override
    {
        if (m_has_output_port) {
            const double out_val = m_val + m_trend * time;

            output.emplace_back(m_output_port_label);
            output.back().addMap().addDouble("d_val", out_val);
        }
    }

    void internalTransition(vd::Time time) override
    {
        m_last_output = m_val + m_trend * time;
    }

    virtual vd::Time timeAdvance() const override
    {
        if (0 == m_trend) {
            return vd::infinity;
        } else {
            return fabs(m_quantum / m_trend);
        }
    }

    std::unique_ptr<vv::Value> observation(
      const vd::ObservationEvent& /*event*/) const override
    {
        return vv::Double::create(m_last_output);
    }

private:
    double m_duration;
    double m_val;
    double m_trend;
    double m_quantum;
    double m_last_output;

    std::string m_output_port_label;
    bool m_has_output_port;
};

DECLARE_DYNAMICS(Generator)
