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
#include <vle/value/Map.hpp>
#include <vle/vpz/AtomicModel.hpp>

namespace vd = vle::devs;
namespace vv = vle::value;
namespace vg = vle::vpz;

class Adder : public vd::Dynamics
{
public:
    Adder(const vd::DynamicsInit& init, const vd::InitEventList& events)
      : vd::Dynamics(init, events)
    {
        m_weights_label = "weights";
        vg::ConnectionList my_list;

        m_has_output_port = false;
        my_list = getModel().getOutputPortList();

        if (my_list.size() > 0) {
            m_output_port_label = (my_list.begin())->first;
            m_has_output_port = true;
        }

        if (my_list.size() > 1)
            Trace(context(),
                  6,
                  "Warning: %s multiple output ports. Will use"
                  " only port %s\n",
                  getModelName().c_str(),
                  m_output_port_label.c_str());

        connected_input_port_list.resize(0);

        my_list = getModel().getInputPortList();
        for (auto& elem : my_list) {
            if ((elem).second.size()) {
                connected_input_port_list.push_back((elem).first);
            }
        }

        if (events.exist(m_weights_label)) {
            const vle::value::Map& mapping = events.getMap(m_weights_label);
            const vle::value::MapValue& lst = mapping.value();
            for (const auto& elem : lst) {
                std::string name = elem.first;
                double weight = vle::value::toDouble(elem.second);

                input_coeffs[name] = weight;
            }

            for (auto& elem : connected_input_port_list) {
                if (0 == input_coeffs.count(elem)) {
                    Trace(context(),
                          6,
                          "Warning: no weight found for input"
                          " port %s of model %s. Assuming 0 value !\n",
                          elem.c_str(),
                          getModelName().c_str());

                    input_coeffs[elem] = 0;
                }
            }
        } else {
            Trace(
              context(),
              6,
              "Warning : no weights values provided for adder %s"
              "Using default value (output is the mean of connected inputs)\n",
              getModelName().c_str());

            for (auto& elem : connected_input_port_list)
                input_coeffs[elem] = 1 / connected_input_port_list.size();
        }
    }

    virtual ~Adder()
    {
    }

    virtual vd::Time init(vd::Time /*time*/) override
    {
        m_state = INIT;
        return vd::infinity;
    }

    virtual void output(vd::Time /*time*/,
                        vd::ExternalEventList& output) const override
    {
        switch (m_state) {
        case INIT:
            break;
        case WAIT:
            break;
        case RESPONSE:
            if (m_has_output_port) {
                output.emplace_back(m_output_port_label);
                output.back().addMap().addDouble("d_val", m_output_value);
            }
        }
    }

    virtual vd::Time timeAdvance() const override
    {
        switch (m_state) {
        case INIT:
            return vd::infinity;
        case WAIT:
            return vd::infinity;
        case RESPONSE:
            return 0;
        }
        return vd::infinity; // useless, only for keeping compiler quiet
    }

    virtual void internalTransition(vd::Time /*time*/) override
    {
        if (RESPONSE == m_state) {
            m_last_output = m_output_value;
        }
        m_state = WAIT;
    }

    virtual void externalTransition(const vd::ExternalEventList& lst,
                                    vd::Time /*time*/) override
    {
        vd::ExternalEventList::const_iterator it;
        for (it = lst.begin(); it != lst.end(); ++it) {
            if (1 < (*it).getMap().value().size())
                Trace(context(),
                      6,
                      "Warning : getting multiple attributes on"
                      " port %s. Using only one\n",
                      (*it).getPortName().c_str());

            input_values[(*it).getPortName()] =
              (*it).getMap().getDouble((*it).getMap().begin()->first);

            switch (m_state) {
            case INIT:
                if (input_values.size() != connected_input_port_list.size())
                    break;
                else {
                    m_output_value = compute_output_val();
                    m_state = RESPONSE;
                    break;
                }
            case WAIT:
            case RESPONSE:
                if (compute_output_val() != m_last_output) {
                    m_output_value = compute_output_val();
                    m_state = RESPONSE;
                } else
                    m_state = WAIT;
            }
        }
    }

    virtual void confluentTransitions(
      vd::Time time,
      const vd::ExternalEventList& events) override
    {
        internalTransition(time);
        externalTransition(events, time);
    }

    std::unique_ptr<vv::Value> observation(
      const vd::ObservationEvent& /*event*/) const override
    {
        return vv::Double::create(m_last_output);
    }

    virtual void finish() override
    {
    }

private:
    typedef enum { INIT, WAIT, RESPONSE } State;
    State m_state;

    typedef std::vector<std::string> t_port_vector;
    t_port_vector connected_input_port_list;

    std::string m_weights_label;
    std::string m_output_port_label;
    bool m_has_output_port;

    std::map<std::string, double> input_coeffs;
    std::map<std::string, double> input_values;

    double m_output_value;
    double m_last_output;

    void dump_input_values()
    {
        Trace(context(), 6, "Input values :");

        for (auto elem : input_values)
            Trace(context(), 6, "%s => %f\n", elem.first.c_str(), elem.second);
    }

    double compute_output_val()
    {
        double acc;
        acc = 0;
        for (auto& elem : connected_input_port_list) {
            acc += input_coeffs[elem] * input_values[elem];
        }
        return acc;
    }
};

DECLARE_DYNAMICS(Adder)
