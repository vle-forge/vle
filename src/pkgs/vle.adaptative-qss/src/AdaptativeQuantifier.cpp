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

#include <cmath>
#include <deque>
#include <vector>
#include <vle/devs/Dynamics.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Tuple.hpp>

namespace vd = vle::devs;
namespace vv = vle::value;
namespace vu = vle::utils;
namespace vg = vle::vpz;

class AdaptativeQuantifier : public vd::Dynamics
{
public:
    AdaptativeQuantifier(const vd::DynamicsInit& init,
                         const vd::InitEventList& events)
      : vd::Dynamics(init, events)
    {
        const auto& my_list = getModel().getOutputPortList();

        if (not my_list.empty()) {
            m_output_port_label = (my_list.begin())->first;
            m_has_output_port = true;
        }

        if (my_list.size() > 1)
            Trace(context(),
                  6,
                  "Warning: multiple output ports."
                  " Will use only port %s\n",
                  m_output_port_label.c_str());

        m_adaptative = true;

        if (events.end() != events.find("allow_offsets")) {
            m_adaptative = vv::toBoolean(events.get("allow_offsets"));
        }

        if (m_adaptative) {
            m_adapt_state = POSSIBLE;
        } else {
            m_adapt_state = IMPOSSIBLE;
        }

        m_zero_init_offset = false;
        if (events.end() != events.find("zero_init_offset")) {
            m_zero_init_offset = vv::toBoolean(events.get("zero_init_offset"));
        }

        if (events.end() != events.find("quantum")) {
            m_step_size = vv::toDouble(events.get("quantum"));
        } else {
            Trace(context(),
                  6,
                  "Warning : no quantum value provided for"
                  " Quantifier %s. Using default value (0.1)\n",
                  getModelName().c_str());

            m_step_size = 0.1;
        }

        if (0 >= m_step_size)
            throw vu::ModellingError("Bad quantum value (provided value : %f, "
                                     "should be strictly positive)",
                                     m_step_size);

        if (events.end() != events.find("archive_length")) {
            m_past_length = vv::toInteger(events.get("archive_length"));
        } else {
            m_past_length = 3;
        }

        if (2 >= m_past_length) {
            throw vu::ModellingError(
              "Bad archive length value ( provided value"
              ": %u, should at least 3)",
              m_past_length);
        }
    }

    virtual ~AdaptativeQuantifier() // This is mandatory
    {
    }

    virtual vd::Time init(vd::Time /* time */) override
    {
        m_offset = 0;
        m_state = INIT;
        return vd::infinity;
    }

    virtual void externalTransition(const vd::ExternalEventList& events,
                                    vd::Time time) override
    {
        double val, shifting_factor;
        int cnt;

        if (events.size() > 1)
            Trace(context(),
                  6,
                  "Warning: %s got multiple events at date: %f\n",
                  getModelName().c_str(),
                  time);

        auto it = events.begin();
        while (it != events.end()) {
            val = it->getMap().getDouble("d_val");
            if (INIT == m_state) {
                init_step_number_and_offset(val);
                update_thresholds();
                m_state = RESPONSE;
            } else {
                cnt = 0;
                if ((val > m_upthreshold) || (val < m_downthreshold))
                    Trace(context(),
                          6,
                          "%s: treating out of bonds val: %f "
                          "(quantizer interval : [%f,%f] at date: %f",
                          getModelName().c_str(),
                          val,
                          m_downthreshold,
                          m_upthreshold,
                          time);

                while ((val >= m_upthreshold) || (val <= m_downthreshold)) {
                    cnt++;
                    if (val >= m_upthreshold) {
                        m_step_number++;
                    } else {
                        m_step_number--;
                    }
                    switch (m_adapt_state) {
                    case IMPOSSIBLE:
                        update_thresholds();
                        break;
                    case POSSIBLE:
                        if (val >= m_upthreshold) {
                            store_change(m_step_size, time);
                        } else {
                            store_change(-m_step_size, time);
                        }
                        shifting_factor = shift_quanta();
                        if (0 > shifting_factor)
                            throw vu::ModellingError(
                              "Bad shifting value (value : %f, "
                              "should be strictly positive)\n",
                              shifting_factor);
                        if (1 < shifting_factor)
                            throw vu::ModellingError(
                              "Bad shifting value ( value : %f, "
                              "should be less than 1)\n",
                              shifting_factor);
                        ;
                        if ((0 != shifting_factor) && (1 != shifting_factor)) {
                            if (val >= m_upthreshold) {
                                update_thresholds(shifting_factor, DOWN);
                            } else {
                                update_thresholds(shifting_factor, UP);
                            }
                            Trace(context(),
                                  6,
                                  "Quantifier %s new quantas while "
                                  "treating new val %f at date %f",
                                  getModelName().c_str(),
                                  val,
                                  time);

                            Trace(context(),
                                  6,
                                  "Quantizer interval:  [%f, %f], "
                                  "amplitude: %f "
                                  "(default amplitude: %f)",
                                  m_downthreshold,
                                  m_upthreshold,
                                  (m_upthreshold - m_downthreshold),
                                  (2 * m_step_size));

                            Trace(context(),
                                  6,
                                  "Quantifier %s shifting : %f",
                                  getModelName().c_str(),
                                  shifting_factor);

                            m_adapt_state = DONE;
                        } else {
                            update_thresholds();
                        }
                        break;
                    case DONE: // equiv to reinit
                        init_step_number_and_offset(val);
                        // archive.resize(0);
                        m_adapt_state = POSSIBLE;
                        update_thresholds();
                        break;
                    }
                }

                if (cnt > 1)
                    Trace(context(),
                          6,
                          "Warning : in %s multiple quanta change"
                          " at date : %f %d\n",
                          getModelName().c_str(),
                          time,
                          cnt);

                if (0 == cnt)
                    Trace(context(),
                          6,
                          "Warning : in %s useless ext transition"
                          "call: no quanta change! input val %f (quantizer "
                          "interval : [%f,%f] at date %f\n",
                          getModelName().c_str(),
                          val,
                          m_downthreshold,
                          m_upthreshold,
                          time);
            }
            ++it;
        }
        m_state = RESPONSE;
    }

    void internalTransition(vd::Time /*time*/) override
    {
        switch (m_state) {
        case INIT:
            break;
        case IDLE:
            break;
        case RESPONSE:
            m_state = IDLE;
            break;
        }
    }

    void confluentTransitions(vd::Time time,
                              const vd::ExternalEventList& externals) override
    {
        internalTransition(time);
        externalTransition(externals, time);
    }

    virtual void output(vd::Time /*time*/,
                        vd::ExternalEventList& output) const override
    {
        if (m_has_output_port) {
            output.emplace_back(m_output_port_label);
            auto& m = output.back().addMap();
            m.addDouble("up", m_upthreshold);
            m.addDouble("down", m_downthreshold);
        }
    }

    virtual vd::Time timeAdvance() const override
    {
        switch (m_state) {
        case INIT:
        case IDLE:
            return vd::infinity;
        case RESPONSE:
            return 0.0;
        }
        return vd::infinity; // useless. shut up compiler warning !
    }

    std::unique_ptr<vv::Value> observation(
      const vd::ObservationEvent& /*event*/) const override
    {
        auto ptr = std::unique_ptr<vv::Value>(vv::Tuple::create());
        auto& t = ptr->toTuple();
        if (INIT != m_state) {
            t.add(m_downthreshold);
            t.add(m_upthreshold);
            t.add(m_upthreshold - m_downthreshold);
            t.add(m_adapt_state);
        }
        return ptr;
    }

private:
    typedef enum { INIT, IDLE, RESPONSE } State;
    State m_state;

    typedef enum { IMPOSSIBLE, POSSIBLE, DONE } Adapt_State;
    Adapt_State m_adapt_state;

    typedef enum { UP, DOWN } Direction;

    bool m_adaptative;
    bool m_zero_init_offset;

    double m_offset;
    double m_step_size;

    long int m_step_number;
    double m_upthreshold;
    double m_downthreshold;

    struct record_t
    {
        double value;
        vd::Time date;
        ;
    };
    std::deque<record_t> archive;

    unsigned int m_past_length;

    std::string m_output_port_label;
    bool m_has_output_port;

    void update_thresholds()
    {
        m_upthreshold = m_offset + m_step_size * (m_step_number + 1);
        m_downthreshold = m_offset + m_step_size * (m_step_number - 1);
    }

    void update_thresholds(double factor)
    {
        m_upthreshold =
          m_offset + m_step_size * (m_step_number + (1 - factor));
        m_downthreshold =
          m_offset + m_step_size * (m_step_number - (1 - factor));
    }

    void update_thresholds(double factor, Direction d)
    {
        if (UP == d) {
            m_upthreshold =
              m_offset + m_step_size * (m_step_number + (1 - factor));
            m_downthreshold = m_offset + m_step_size * (m_step_number - 1);
        } else {
            m_upthreshold = m_offset + m_step_size * (m_step_number + 1);
            m_downthreshold =
              m_offset + m_step_size * (m_step_number - (1 - factor));
        }
    }

    void init_step_number_and_offset(double value)
    {
        m_step_number = floor(value / m_step_size);
        if (m_zero_init_offset) {
            m_offset = 0;
        } else {
            m_offset = value - m_step_number * m_step_size;
        }
    }

    double shift_quanta()
    {
        double factor;
        factor = 0; // return 0 is shift failure.
        if (oscillating(m_past_length - 1) &&
            (0 != (archive.back().date - archive.front().date))) {
            Trace(context(),
                  7,
                  "Oscillating, archive size = %lu (m_past_length = %u) ",
                  vle::utils::numeric_cast<unsigned long int>(archive.size()),
                  m_past_length);
            double acc;
            double local_estim;
            int cnt;
            acc = 0;
            cnt = 0;

            for (size_t i = 0; i < archive.size() - 2; ++i) {
                if (0 != (archive[i + 2].date - archive[i].date)) {
                    if ((archive.back().value * archive[i + 1].value) >
                        0) // same direction as last move
                    {
                        local_estim =
                          1 -
                          (archive[i + 1].date - archive[i].date) /
                            (archive[i + 2].date - archive[i].date);
                    } else {
                        local_estim = (archive[i + 1].date - archive[i].date) /
                                      (archive[i + 2].date - archive[i].date);
                    }

                    acc += local_estim;
                    cnt++;
                }
            }

            acc = acc / cnt;
            factor = acc;
            archive.resize(0);
        }

        return factor;
    }

    void store_change(double val, vd::Time time)
    {
        record_t record;
        record.date = time;
        record.value = val;
        archive.push_back(record);

        while (archive.size() > m_past_length) {
            archive.pop_front();
        }
    }

    bool oscillating(unsigned int range)
    {
        if ((range + 1) > archive.size())
            return false;

        for (size_t i = archive.size() - range; i < archive.size() - 1; i++)
            if (0 < (archive[i].value * archive[i + 1].value))
                return false;

        return true;
    }

    bool monotonous(unsigned int range)
    {
        if ((range + 1) > archive.size())
            return false;

        for (size_t i = 0; i < range; i++)
            if (0 > (archive[i].value * archive[i + 1].value))
                return false;

        return true;
    }
};

DECLARE_DYNAMICS(AdaptativeQuantifier)
