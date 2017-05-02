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

#include <unordered_map>
#include <vle/devs/Dynamics.hpp>
#include <vle/value/Map.hpp>

namespace vd = vle::devs;
namespace vv = vle::value;
namespace vu = vle::utils;
namespace vg = vle::vpz;

class Constant : public vd::Dynamics
{
    std::unordered_map<std::string, std::shared_ptr<vv::Value>> m_outputs;

public:
    Constant(const vd::DynamicsInit& init, const vd::InitEventList& events)
      : vd::Dynamics(init, events)
    {
        for (auto& port : getModel().getOutputPortList())
            m_outputs[port.first] = std::shared_ptr<vv::Value>();

        for (const auto& event : events) {
            auto it = m_outputs.find(event.first);
            if (m_outputs.find(event.first) != m_outputs.end())
                it->second = std::shared_ptr<vv::Value>(event.second->clone());
            else
                Trace(context(),
                      3,
                      "Constant model %s: no port for data %s",
                      getModelName().c_str(),
                      event.first.c_str());
        }
    }

    ~Constant() override = default;

    virtual vd::Time init(vd::Time /* time */) override
    {
        if (m_outputs.empty()) {
            Trace(context(),
                  3,
                  "Constant model %s: no output port",
                  getModelName().c_str());

            return vd::infinity;
        }

        return 0;
    }

    virtual void output(vd::Time /*time*/,
                        vd::ExternalEventList& output) const override
    {
        for (const auto& port : m_outputs) {
            if (not port.second.get())
                Trace(context(),
                      3,
                      "Constant model %s: no data to sound through port %s",
                      getModelName().c_str(),
                      port.first.c_str());

            output.emplace_back(port.first);
            output.back().attributes() = port.second;
        }
    }

    virtual vd::Time timeAdvance() const override
    {
        return vd::infinity;
    }

    void internalTransition(vd::Time /*time*/) override
    {
        m_outputs.clear();
    }
};

DECLARE_DYNAMICS(Constant)
