/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#include <vle/translator/MatrixTranslator.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/i18n.hpp>

namespace vle {
namespace translator {

regular_graph_generator::regular_graph_generator(const parameter& params)
  : m_params(params)
  , m_metrics{ -1 }
{
}

regular_graph_generator::graph_metrics
regular_graph_generator::metrics() const
{
    return m_metrics;
}

void
make_1d_no_wrap(vle::devs::Executive& executive,
                regular_graph_generator::connectivity connectivity,
                const std::vector<std::string>& modelnames,
                int length,
                const std::vector<std::string>& mask,
                int x_mask)
{
    const int mask_length = vle::utils::numeric_cast<int>(mask.size());

    for (int i = 0; i != length; ++i) {
        const int i_min = std::max(0, i - x_mask);
        const int i_max = std::min(length, i + mask_length - x_mask);
        const int x_mask_min = std::max(0, x_mask - i);

        for (int x = i_min, m = x_mask_min; x != i_max; ++x, ++m) {
            if (not mask[m].empty()) {
                switch (connectivity) {
                case regular_graph_generator::connectivity::IN_OUT:
                    executive.addOutputPort(modelnames[i], "out");
                    executive.addInputPort(modelnames[x], "in");

                    executive.addConnection(
                      modelnames[i], "out", modelnames[x], "in");
                    break;

                case regular_graph_generator::connectivity::OTHER:
                    executive.addOutputPort(modelnames[i], modelnames[x]);
                    executive.addInputPort(modelnames[x], modelnames[i]);

                    executive.addConnection(modelnames[i],
                                            modelnames[x],
                                            modelnames[x],
                                            modelnames[i]);
                    break;

                case regular_graph_generator::connectivity::NAMED:
                    executive.addOutputPort(modelnames[i], mask[m]);
                    executive.addInputPort(modelnames[x], mask[m]);

                    executive.addConnection(
                      modelnames[i], mask[m], modelnames[x], mask[m]);
                    break;
                }
            }
        }
    }
}

void
make_1d_wrap(vle::devs::Executive& executive,
             regular_graph_generator::connectivity connectivity,
             const std::vector<std::string>& modelnames,
             int length,
             const std::vector<std::string>& mask,
             int x_mask)
{
    const int mask_length = vle::utils::numeric_cast<int>(mask.size());

    for (int i = 0; i != length; ++i) {
        const int x_min = i - x_mask;
        const int x_max = i + mask_length - x_mask;

        for (int x = x_min, m = 0; x != x_max; ++x, ++m) {
            int p = x;

            if (x < 0)
                p = length + x;
            else if (x >= length)
                p = x % length;

            if (not mask[m].empty()) {
                switch (connectivity) {
                case regular_graph_generator::connectivity::IN_OUT:
                    executive.addOutputPort(modelnames[i], "out");
                    executive.addInputPort(modelnames[p], "in");

                    executive.addConnection(
                      modelnames[i], "out", modelnames[p], "in");
                    break;

                case regular_graph_generator::connectivity::OTHER:
                    executive.addOutputPort(modelnames[i], modelnames[p]);
                    executive.addInputPort(modelnames[p], modelnames[i]);

                    executive.addConnection(modelnames[i],
                                            modelnames[p],
                                            modelnames[p],
                                            modelnames[i]);
                    break;

                case regular_graph_generator::connectivity::NAMED:
                    executive.addOutputPort(modelnames[i], mask[m]);
                    executive.addInputPort(modelnames[p], mask[m]);

                    executive.addConnection(
                      modelnames[i], mask[m], modelnames[p], mask[m]);
                    break;
                }
            }
        }
    }
}

void
regular_graph_generator::make_1d(vle::devs::Executive& executive,
                                 int length,
                                 bool wrap,
                                 const std::vector<std::string>& mask,
                                 int x_mask)
{
    if (length <= 0 or mask.size() == 0 or
        utils::numeric_cast<int>(mask.size()) <= x_mask)
        throw vle::utils::ArgError(
          _("regular_graph_generator: bad model parameters"));

    std::vector<std::string> modelnames(length);
    std::string name, classname;
    regular_graph_generator::node_metrics metrics{ -1, -1, -1 };

    for (int i = 0; i != length; ++i) {
        metrics.x = i;
        m_params.make_model(metrics, name, classname);
        executive.createModelFromClass(classname, name);
        modelnames[i] = name;
    }

    m_metrics.vertices = length;

    if (not wrap)
        make_1d_no_wrap(
          executive, m_params.type, modelnames, length, mask, x_mask);
    else
        make_1d_wrap(
          executive, m_params.type, modelnames, length, mask, x_mask);
}

void
apply_mask(vle::devs::Executive& executive,
           const regular_graph_generator::parameter& params,
           const utils::Array<std::string>& modelnames,
           int c,
           int r,
           const std::array<int, 2>& length,
           const utils::Array<std::string>& mask,
           int x_mask,
           int y_mask)
{
    const int mask_columns = utils::numeric_cast<int>(mask.columns());
    const int mask_rows = utils::numeric_cast<int>(mask.rows());
    const int x_min = std::max(0, c - x_mask);
    const int x_max = std::min(length.front(), c + mask_columns - x_mask);
    const int y_min = std::max(0, r - y_mask);
    const int y_max = std::min(std::get<1>(length), r + mask_rows - y_mask);

    const int x_mask_min = std::max(0, x_mask - c);
    const int y_mask_min = std::max(0, y_mask - r);

    for (int y = y_min, n = y_mask_min; y != y_max; ++y, ++n) {
        for (int x = x_min, m = x_mask_min; x != x_max; ++x, ++m) {
            if (not mask(m,n).empty()) {
                switch (params.type) {
                case regular_graph_generator::connectivity::IN_OUT:
                    executive.addOutputPort(modelnames(c, r), "out");
                    executive.addInputPort(modelnames(x, y), "in");

                    executive.addConnection(
                      modelnames(c, r), "out", modelnames(x, y), "in");
                    break;

                case regular_graph_generator::connectivity::OTHER:
                    executive.addOutputPort(modelnames(c, r), modelnames(x, y));
                    executive.addInputPort(modelnames(x, y), modelnames(c, r));

                    executive.addConnection(modelnames(c, r),
                                            modelnames(x, y),
                                            modelnames(x, y),
                                            modelnames(c, r));
                    break;

                case regular_graph_generator::connectivity::NAMED:
                    executive.addOutputPort(modelnames(c, r), mask(m, n));
                    executive.addInputPort(modelnames(x, y), mask(m, n));

                    executive.addConnection(
                      modelnames(c, r), mask(m, n), modelnames(x, y), mask(m, n));
                    break;
                }
            }
        }
    }
}

void
apply_wrap_mask(vle::devs::Executive& executive,
                const std::array<bool, 2>& wrap,
                const regular_graph_generator::parameter& params,
                const utils::Array<std::string>& modelnames,
                int c,
                int r,
                const std::array<int, 2>& length,
                const utils::Array<std::string>& mask,
                int x_mask,
                int y_mask)
{
    const int mask_columns = utils::numeric_cast<int>(mask.columns());
    const int mask_rows = utils::numeric_cast<int>(mask.rows());
    const int x_min = c - x_mask;
    const int x_max = c + mask_columns - x_mask;
    const int y_min = r - y_mask;
    const int y_max = r + mask_rows - y_mask;

    for (int y = y_min, n = 0; y != y_max; ++y, ++n) {
        int p = y;
        if (std::get<1>(wrap) == true) {
            if (y < 0)
                p = std::get<1>(length) + y;
            else if (y >= std::get<1>(length))
                p = y % std::get<1>(length);
        } else
            continue;

        for (int x = x_min, m = 0; x != x_max; ++x, ++m) {
            int q = x;
            if (std::get<0>(wrap) == true) {
                if (x < 0)
                    q = length.front() + x;
                else if (x >= length.front())
                    q = x % length.front();
            } else
                continue;

            if (not mask(m,n).empty()) {
                switch (params.type) {
                case regular_graph_generator::connectivity::IN_OUT:
                    executive.addOutputPort(modelnames(c, r), "out");
                    executive.addInputPort(modelnames(q, p), "in");

                    executive.addConnection(
                      modelnames(c, r), "out", modelnames(q, p), "in");
                    break;

                case regular_graph_generator::connectivity::OTHER:
                    executive.addOutputPort(modelnames(c, r), modelnames(q, p));
                    executive.addInputPort(modelnames(q, p), modelnames(c, r));

                    executive.addConnection(modelnames(c, r),
                                            modelnames(q, p),
                                            modelnames(q, p),
                                            modelnames(c, r));
                    break;

                case regular_graph_generator::connectivity::NAMED:
                    executive.addOutputPort(modelnames(c, r), mask(m, n));
                    executive.addInputPort(modelnames(q, p), mask(m, n));

                    executive.addConnection(
                      modelnames(c, r), mask(m, n), modelnames(q, p), mask(m, n));
                    break;
                }
            }
        }
    }
}

void
regular_graph_generator::make_2d(vle::devs::Executive& executive,
                                 const std::array<int, 2>& length,
                                 const std::array<bool, 2>& wrap,
                                 const utils::Array<std::string>& mask,
                                 int x_mask,
                                 int y_mask)
{
    if (length.front() * std::get<1>(length) <= 0 or x_mask < 0 or y_mask < 0 or
        utils::numeric_cast<std::size_t>(x_mask) >= mask.columns() or
        utils::numeric_cast<std::size_t>(y_mask) >= mask.rows())
        throw vle::utils::ArgError(
          _("regular_graph_generator: bad parameters"));

    utils::Array<std::string> modelnames(length.front(), std::get<1>(length));
    std::string name, classname;
    regular_graph_generator::node_metrics metrics{ -1, -1, -1 };

    m_metrics.vertices = length.front() * std::get<1>(length);

    for (int c = 0; c != length.front(); ++c) {
        for (int r = 0; r != std::get<1>(length); ++r) {
            metrics.x = c;
            metrics.y = r;
            m_params.make_model(metrics, name, classname);
            executive.createModelFromClass(classname, name);
            modelnames(c, r) = name;
        }
    }

    if (not std::get<0>(wrap) and not std::get<1>(wrap))
        for (int c = 0; c != length.front(); ++c)
            for (int r = 0; r != std::get<1>(length); ++r)
                apply_mask(executive,
                           m_params,
                           modelnames,
                           c,
                           r,
                           length,
                           mask,
                           x_mask,
                           y_mask);
    else
        for (int c = 0; c != length.front(); ++c)
            for (int r = 0; r != std::get<1>(length); ++r)
                apply_wrap_mask(executive,
                                wrap,
                                m_params,
                                modelnames,
                                c,
                                r,
                                length,
                                mask,
                                x_mask,
                                y_mask);
}
}
}
