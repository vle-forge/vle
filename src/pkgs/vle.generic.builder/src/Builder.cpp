/*
 * Copyright 2016 INRA
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
#include <random>
#include <functional>
#include <vle/devs/Executive.hpp>
#include <vle/translator/GraphTranslator.hpp>
#include <vle/translator/MatrixTranslator.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Tuple.hpp>

class Builder : public vle::devs::Executive
{
    std::mt19937 generator;
    std::string model_prefix;
    std::string model_classname;
    std::string condition_name;
    std::string graph_type;
    std::string graph_connectivity;
    int model_number;
    bool directed;

    void regular_graph_generator_make_model(
      const vle::translator::regular_graph_generator::node_metrics& n,
      std::string& name,
      std::string& classname)
    {
        if (n.y == -1)
            name = vle::utils::format("%s-%d", model_prefix.c_str(), n.x);
        else
            name =
              vle::utils::format("%s-%d-%d", model_prefix.c_str(), n.x, n.y);

        classname = model_classname;
    }

    void graph_generator_make_model(
      const vle::translator::graph_generator::node_metrics& n,
      std::string& name,
      std::string& classname)
    {
        name = vle::utils::format("%s-%d", model_prefix.c_str(), n.id);
        classname = model_classname;
    }

public:
    vle::translator::graph_generator::connectivity
    graph_generator_connectivity()
    {
        if (graph_connectivity == "in-out")
            return vle::translator::graph_generator::connectivity::IN_OUT;
        if (graph_connectivity == "in")
            return vle::translator::graph_generator::connectivity::IN;
        if (graph_connectivity == "out")
            return vle::translator::graph_generator::connectivity::OUT;
        if (graph_connectivity == "other")
            return vle::translator::graph_generator::connectivity::OTHER;

        throw vle::utils::ModellingError("Unknown graph connectivity: %s",
                                         graph_connectivity.c_str());
    }

    vle::translator::regular_graph_generator::connectivity
    regular_graph_generator_connectivity()
    {
        if (graph_connectivity == "in-out")
            return vle::translator::regular_graph_generator::connectivity::
              IN_OUT;
        if (graph_connectivity == "other")
            return vle::translator::regular_graph_generator::connectivity::
              OTHER;
        if (graph_connectivity == "named")
            return vle::translator::regular_graph_generator::connectivity::
              NAMED;
        throw vle::utils::ModellingError("Unknown graph connectivity: %s",
                                         graph_connectivity.c_str());
    }

    void use_defined_graph_generator(const vle::devs::InitEventList& events)
    {
        vle::utils::Array<bool> userdefined(model_number, model_number);

        {
            const auto& v = events.getTuple("user-defined");
            if (v.size() != vle::utils::numeric_cast<std::size_t>(
                              model_number * model_number))
                throw vle::utils::ModellingError(
                  "User defined model size error");

            int pos = 0;
            for (int i = 0; i != model_number; ++i)
                for (int j = 0; j != model_number; ++j, ++pos)
                    userdefined(i, j) = (v.get(pos) != 0.0);
        }

        vle::translator::graph_generator::parameter param{
            std::bind(&Builder::graph_generator_make_model,
                      this,
                      std::placeholders::_1,
                      std::placeholders::_2,
                      std::placeholders::_3),
            graph_generator_connectivity(),
            directed
        };

        vle::translator::graph_generator gg(param);

        gg.make_graph(*this, model_number, userdefined);
    }

    void use_smallworld_graph_generator(const vle::devs::InitEventList& events)
    {
        vle::translator::graph_generator::parameter param{
            std::bind(&Builder::graph_generator_make_model,
                      this,
                      std::placeholders::_1,
                      std::placeholders::_2,
                      std::placeholders::_3),
            graph_generator_connectivity(),
            directed
        };

        vle::translator::graph_generator gg(param);

        gg.make_smallworld(*this,
                           generator,
                           model_number,
                           events.getInt("k"),
                           events.getDouble("probability"),
                           events.getBoolean("allow-self-loops"));
    }

    void use_scalefree_graph_generator(const vle::devs::InitEventList& events)
    {
        vle::translator::graph_generator::parameter param{
            std::bind(&Builder::graph_generator_make_model,
                      this,
                      std::placeholders::_1,
                      std::placeholders::_2,
                      std::placeholders::_3),
            graph_generator_connectivity(),
            directed
        };

        vle::translator::graph_generator gg(param);

        gg.make_scalefree(*this,
                           generator,
                           model_number,
                           events.getDouble("alpha"),
                           events.getDouble("beta"),
                           events.getBoolean("allow-self-loops"));
    }

    void use_sortederdesrenyi_graph_generator(
      const vle::devs::InitEventList& events)
    {
        vle::translator::graph_generator::parameter param{
            std::bind(&Builder::graph_generator_make_model,
                      this,
                      std::placeholders::_1,
                      std::placeholders::_2,
                      std::placeholders::_3),
            graph_generator_connectivity(),
            directed
        };

        vle::translator::graph_generator gg(param);

        gg.make_sorted_erdos_renyi(*this,
                                   generator,
                                   model_number,
                                   events.getDouble("probability"),
                                   events.getBoolean("allow-self-loops"));
    }

    void use_erdosrenyi_graph_generator(const vle::devs::InitEventList& events)
    {
        vle::translator::graph_generator::parameter param{
            std::bind(&Builder::graph_generator_make_model,
                      this,
                      std::placeholders::_1,
                      std::placeholders::_2,
                      std::placeholders::_3),
            graph_generator_connectivity(),
            directed
        };

        vle::translator::graph_generator gg(param);

        if (events.exist("fraction"))
            gg.make_erdos_renyi(*this,
                                generator,
                                model_number,
                                events.getDouble("fraction"),
                                events.getBoolean("allow-self-loops"));
        else
            gg.make_erdos_renyi(*this,
                                generator,
                                model_number,
                                events.getInt("edges_number"),
                                events.getBoolean("allow-self-loops"));
    }

    void use_graph_generator(const vle::devs::InitEventList& events)
    {
        model_number = events.getInt("model-number");
        std::string generator_name = events.getString("graph-generator");

        if (events.exist("graph-seed"))
            generator.seed(events.getInt("graph-seed"));

        if (generator_name == "defined")
            use_defined_graph_generator(events);
        else if (generator_name == "small-world")
            use_smallworld_graph_generator(events);
        else if (generator_name == "scale-free")
            use_scalefree_graph_generator(events);
        else if (generator_name == "sorted-erdos-renyi")
            use_sortederdesrenyi_graph_generator(events);
        else if (generator_name == "erdos_renyi")
            use_erdosrenyi_graph_generator(events);
        else
            throw vle::utils::ModellingError("Unknown graph gererator: %s",
                                             generator_name.c_str());
    }

    void use_1d_regular_graph_generator(const vle::devs::InitEventList& events)
    {
        vle::translator::regular_graph_generator::parameter param{
            std::bind(&Builder::regular_graph_generator_make_model,
                      this,
                      std::placeholders::_1,
                      std::placeholders::_2,
                      std::placeholders::_3),
            regular_graph_generator_connectivity()
        };

        vle::translator::regular_graph_generator rgg(param);

        const auto& v = events.getSet("mask").value();
        std::vector<std::string> mask;

        std::transform(v.begin(),
                       v.end(),
                       std::back_inserter(mask),
                       [](const auto& v) { return v->toString().value(); });

        rgg.make_1d(*this,
                    events.getInt("lenght"),
                    events.getBoolean("graph-wrap"),
                    mask,
                    events.getInt("x-mask"));
    }

    void use_2d_regular_graph_generator(const vle::devs::InitEventList& events)
    {
        vle::translator::regular_graph_generator::parameter param{
            std::bind(&Builder::regular_graph_generator_make_model,
                      this,
                      std::placeholders::_1,
                      std::placeholders::_2,
                      std::placeholders::_3),
            regular_graph_generator_connectivity()
        };

        vle::translator::regular_graph_generator rgg(param);

        int width_mask = events.getInt("mask-width");
        int height_mask = events.getInt("mask-height");
        vle::utils::Array<std::string> mask(width_mask, height_mask);

        const auto& v = events.getSet("mask").value();

        {
            if (vle::utils::numeric_cast<int>(mask.size()) !=
                width_mask * height_mask)
                throw vle::utils::ModellingError("mask definition error");

            int i = 0, j = 0;

            for (const auto& str : v) {
                mask(i, j) = str->toString().value();
                ++j;

                if (j >= height_mask) {
                    i++;
                    j = 0;
                }
            }
        }

        rgg.make_2d(*this,
                    { { events.getInt("width"), events.getInt("height") } },
                    { { events.getBoolean("wrap-width"),
                        events.getBoolean("wrap-height") } },
                    mask,
                    events.getInt("x-mask"),
                    events.getInt("y-mask"));
    }

    void use_regular_graph_generator(const vle::devs::InitEventList& events)
    {
        int dimension = events.getInt("graph-dimension");

        if (dimension == 1)
            use_1d_regular_graph_generator(events);
        else if (dimension == 2)
            use_2d_regular_graph_generator(events);
        else
            throw vle::utils::ModellingError(
              "Undefined regular graph with %d dimension", dimension);
    }

public:
    Builder(const vle::devs::ExecutiveInit& init,
            const vle::devs::InitEventList& events)
      : vle::devs::Executive(init, events)
    {
        model_prefix = events.getString("model-prefix");
        model_classname = events.getString("model-class");
        condition_name = events.getString("node-condition");
        graph_type = events.getString("graph-type");
        graph_connectivity = events.getString("graph-connectiviy");
        directed = events.getBoolean("graph-directed");

        if (graph_type == "graph")
            use_graph_generator(events);
        else if (graph_type == "regular")
            use_regular_graph_generator(events);
        else
            throw vle::utils::ModellingError("Unknown graph type %s",
                                             graph_type.c_str());
    }

    virtual ~Builder() override
    {
    }
};

DECLARE_EXECUTIVE(Builder)
