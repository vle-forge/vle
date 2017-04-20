/*
 * Copyright 2017 INRA
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

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <deque>
#include <fstream>
#include <thread>
#include <vle/devs/Dynamics.hpp>
#include <vle/utils/Filesystem.hpp>
#include <vle/utils/Spawn.hpp>
#include <vle/utils/Tools.hpp>

struct gnuplot_log : vle::utils::Context::LogFunctor
{
    FILE* fp;

    gnuplot_log()
      : fp(nullptr)
    {
    }

    ~gnuplot_log()
    {
        if (fp)
            fclose(fp);
    }

    void write(const vle::utils::Context& ctx,
               int priority,
               const char* file,
               int line,
               const char* fn,
               const char* format,
               va_list args) noexcept override
    {
        if (not fp) {
            auto home = ctx.getHomeDir();
            const std::uintptr_t adr = reinterpret_cast<std::uintptr_t>(&ctx);
            std::string filename("gnuplot-");
            filename += std::to_string(adr);

            fp = fopen(home.string().c_str(), "w");
        }

        if (fp) {
            if (priority == 7)
                fprintf(fp, "Gnuplot log: [Debug] %s:%d %s: ", file, line, fn);
            else if (priority == 6)
                fprintf(fp, "Gnuplot log: %s: ", fn);
            else
                fprintf(fp, "Gnuplot log: [Error] %s: ", fn);

            vfprintf(fp, format, args);
        }
    }
};

struct data_element
{
    data_element(long int index_, vle::devs::Time t_, double value_)
      : t(t_)
      , value(value_)
      , index(index_)
    {
        assert(index >= 0);
    }

    vle::devs::Time t;
    double value;
    long int index;
};

long int
get_index(const std::vector<std::string>& labels,
          const std::string& to_found) noexcept
{
    auto it = std::find(labels.begin(), labels.end(), to_found);

    assert(it != labels.end() && "FIXIT for executive's job");

    return std::distance(labels.begin(), it);
}

void
start_plot(vle::utils::Path command, vle::utils::Path gnuplot_script)
{
    auto ctx = vle::utils::make_context();
    ctx->set_log_priority(3);
    ctx->set_log_function(std::make_unique<gnuplot_log>());

    vle::utils::Spawn spawn(ctx);

    std::vector<std::string> args{ "-c", gnuplot_script.string(), "-p" };

    try {
        if (command.empty()) {
#ifdef _WIN32
            command = ctx->findProgram("gnuplot.exe");
#else
            command = ctx->findProgram("gnuplot");
#endif
        } else if (not command.is_absolute()) {
            command = ctx->findProgram(command.string());
        }

        spawn.start(
          command.string(), vle::utils::Path::current_path().string(), args);
    } catch (const std::exception& e) {
        Trace(ctx,
              3,
              "Plot error: fail to start GNUplot. Data are stored in file "
              "`%s'\n",
              gnuplot_script.string().c_str());
    }

    std::string output;
    std::string error;

    output.reserve(vle::utils::Spawn::default_buffer_size);
    error.reserve(vle::utils::Spawn::default_buffer_size);

    while (not spawn.isfinish()) {
        if (spawn.get(&output, &error)) {
            if (not output.empty()) {
                Trace(ctx, 5, "%s\n", output.c_str());
                output.clear();
            }

            if (not error.empty()) {
                Trace(ctx, 5, "%s\n", error.c_str());
                error.clear();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } else {
            break;
        }
    }

    spawn.wait();

    if (spawn.get(&output, &error)) {
        if (not output.empty()) {
            Trace(ctx, 5, "%s\n", output.c_str());
            output.clear();
        }

        if (not error.empty()) {
            Trace(ctx, 5, "%s\n", error.c_str());
            error.clear();
        }
    }

    std::string msg;
    bool success;

    spawn.status(&msg, &success);

    Trace(ctx, 5, "Plot info: spawn msg %s\n", msg.c_str());

    if (not success) {
        Trace(ctx,
              3,
              "Plot error: fail to start GNUplot. Data are stored in file "
              "`%s'. Error: %s\n",
              gnuplot_script.string().c_str(),
              msg.c_str());
    }
}

class Plot : public vle::devs::Dynamics
{
    std::chrono::time_point<std::chrono::steady_clock> m_time;
    std::thread m_runner;

    vle::utils::Path m_data_file;
    vle::utils::Path m_gp_file;
    vle::utils::Path m_config_file;
    vle::utils::Path m_command;

    std::ofstream m_data_os;

    std::vector<std::string> m_labels;
    std::deque<data_element> m_data;

    vle::devs::Time m_begin;
    std::size_t m_current_row;
    double m_duration;
    bool m_gnuplot_started;

public:
    Plot(const vle::devs::DynamicsInit& init,
         const vle::devs::InitEventList& events)
      : vle::devs::Dynamics(init, events)
      , m_data_file(vle::utils::Path::unique_path("data-%%%%%%%%%%%%.dat"))
      , m_gp_file(vle::utils::Path::unique_path("plot-%%%%%%%%%%%%.gp"))
      , m_config_file(vle::utils::Path::unique_path("conf-%%%%%%%%%%%%"))
      , m_data_os(m_data_file.string())
      , m_current_row{ 0 }
      , m_duration(0.3)
      , m_gnuplot_started(false)
    {
        if (not m_data_os.is_open()) {
            Trace(context(),
                  3,
                  "Plot error: fail to open file `%s' to store data\n",
                  m_data_file.string().c_str());

            return;
        }

        const auto& input_port_list = getModel().getInputPortList();
        for (auto& elem : input_port_list)
            m_labels.emplace_back(elem.first);

        {
            auto it = events.find("gnuplot-duration");
            if (it != events.end() and it->second->isDouble()) {
                auto v = it->second->toDouble().value();
                if (v > 0.0)
                    m_duration = v;
                else
                    Trace(
                      context(), 4, "Plot warning: bad gnuplot-duration\n");
            }
        }

        {
            auto it = events.find("gnuplot-command");
            if (it != events.end() and it->second->isString()) {
                vle::utils::Path v(it->second->toString().value());

                if (v.is_file())
                    m_command = v;
                else
                    Trace(context(), 4, "Plot warning: bad gnuplot-command\n");
            }
        }
    }

    ~Plot() override = default;

    vle::devs::Time init(vle::devs::Time t) override
    {
        m_begin = t;
        m_time = std::chrono::steady_clock::now();

        return vle::devs::Dynamics::init(t);
    }

    void externalTransition(const vle::devs::ExternalEventList& events,
                            vle::devs::Time t) override
    {
        for (const auto& elem : events) {
            auto idx = get_index(m_labels, elem.getPortName());

            if (elem.attributes()->isDouble())
                m_data.emplace_back(idx, t, elem.getDouble().value());
            else if (elem.attributes()->isInteger())
                m_data.emplace_back(idx, t, elem.getInteger().value());
            else if (elem.attributes()->isMap()) {
                auto& m = elem.attributes()->toMap();
                if (m.exist("d_val"))
                    m_data.emplace_back(idx, t, m.getDouble("d_val"));
                else if (m.exist("up"))
                    m_data.emplace_back(idx, t, m.getDouble("up"));
                else
                    Trace(
                      context(), 4, "Plot warning: fail to convert data\n");
            } else
                Trace(context(), 4, "Plot warning: fail to convert data\n");
        }

        auto now = std::chrono::steady_clock::now();
        auto duration =
          std::chrono::duration_cast<std::chrono::duration<double>>(now -
                                                                    m_time);

        if (duration.count() >= m_duration) {
            write(m_current_row, true);

            if (not m_gnuplot_started) {
                m_gnuplot_started = true;
                m_runner = std::thread(start_plot, m_command, m_gp_file);
                m_runner.detach();
            }

            m_time = std::chrono::steady_clock::now();
        }
    }

    vle::devs::Time timeAdvance() const override
    {
        return vle::devs::infinity;
    }

    void write_config(bool reread)
    {
        std::ofstream ofs(m_config_file.string());
        if (not ofs.is_open()) {
            Trace(context(),
                  3,
                  "Plot error: fail to write configuration file `%s'\n",
                  m_config_file.string().c_str());

            return;
        }

        ofs << "a=" << (reread ? 0 : 1) << '\n';
    }

    std::size_t write_data(std::size_t row)
    {
        //
        // Write header: for each input port, the string is used as column
        // label into gnuplot file.
        //

        m_data_os << "# time ";

        for (auto& elem : m_labels)
            m_data_os << '"' << elem << '"' << '\t';

        m_data_os << '\n';

        //
        // Write data: started from the @c row.
        //

        std::vector<std::pair<bool, double>> to_write(m_labels.size());
        for (auto& elem : to_write)
            elem.first = false;

        while (row < m_data.size()) {
            vle::devs::Time t = m_data[row].t;

            m_data_os << t;

            for (auto& elem : to_write)
                elem.first = false;

            assert((std::size_t)m_data[row].index < m_labels.size());

            to_write[m_data[row].index].first = true;
            to_write[m_data[row].index].second = m_data[row].value;

            ++row;

            while (row < m_data.size() and m_data[row].t == t) {
                to_write[m_data[row].index].first = true;
                to_write[m_data[row].index].second = m_data[row].value;
                ++row;
            }

            for (std::size_t it{ 0 }, et{ to_write.size() }; it != et; ++it) {
                m_data_os << '\t';
                if (to_write[it].first)
                    m_data_os << to_write[it].second;
                else
                    m_data_os << '?';

                if (it + 1 == et)
                    m_data_os << '\n';
            }
        }

        return m_data.size();
    }

    void write_plot()
    {
        std::ofstream os(m_gp_file.string());
        if (not os.is_open()) {
            Trace(context(),
                  3,
                  "Plot error: fail to open gp file %s to store gnuplot "
                  "script\n",
                  m_gp_file.string().c_str());

            return;
        }

        os << "load '" << m_config_file.string() << "'\n"
           << std::fixed << "set terminal qt persist\n"
           << "plot '" << m_data_file.string() << "' using 1:2 title '"
           << m_labels[0] << "' w l";

        for (std::size_t i = 1; i < m_labels.size(); ++i)
            os << ", '' using 1:" << i + 2 << " title '" << m_labels[i]
               << "' w l";

        os << "\npause " << m_duration << "\nif (a<1) reread\n";
    }

    void write(std::size_t row, bool reread)
    {
        m_current_row = write_data(row);
        write_plot();
        write_config(reread);
    }

    void finish() override
    {
        if (m_labels.empty()) {
            Trace(context(), 4, "Plot warning: empty data to draw\n");
            return;
        }

        //
        // Force to rewrite the entire data file to be sure all data if write.
        //

        if (m_data_os.is_open()) {
            m_data_os.close();
            m_data_os.open(m_data_file.string());

            if (not m_data_os.is_open()) {
                Trace(context(),
                      3,
                      "Plot error: fail to open file `%s' to store data\n",
                      m_data_file.string().c_str());

                return;
            }
        }

        write(0, false);

        if (not m_gnuplot_started) {
            m_runner = std::thread(start_plot, m_command, m_gp_file);
            m_runner.detach();
        }
    }
};

DECLARE_DYNAMICS(Plot)
