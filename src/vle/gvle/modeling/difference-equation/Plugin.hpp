/**
 * @file vle/gvle/modeling/DifferenceEquation/Plugin.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2009 ULCO http://www.univ-littoral.fr
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


#ifndef VLE_GVLE_MODELING_DIFFERENCEEQUATION_PLUGIN_HPP
#define VLE_GVLE_MODELING_DIFFERENCEEQUATION_PLUGIN_HPP

#include <vle/gvle/ModelingPlugin.hpp>
#include <vle/gvle/modeling/difference-equation/Mapping.hpp>
#include <vle/gvle/modeling/difference-equation/TimeStep.hpp>
#include <vle/gvle/modeling/difference-equation/Parameters.hpp>
#include <vle/utils/Template.hpp>
#include <vle/vpz/Dynamic.hpp>
#include <gtkmm/dialog.h>

namespace vle { namespace gvle { namespace modeling {

class Plugin : public ModelingPlugin, public TimeStep,
               public Mapping, public Parameters
{
public:
    Plugin(const std::string& name);
    virtual ~Plugin();

    void onSource();
protected:
    Glib::RefPtr<Gnome::Glade::Xml> mXml;

    std::string                 mOldMode;
    std::vector < std::string > mOldExternalVariables;

    std::string mComputeFunction;
    std::string mInitValueFunction;
    std::string mUserFunctions;

    void backup();
    void generate(graph::AtomicModel& atom,
                  vpz::AtomicModel& model,
                  vpz::Dynamic& dynamic,
                  vpz::Conditions& conditions,
                  const std::string& classname,
                  const std::string& namespace_,
                  bool generic = false);
    virtual void generateCondition(graph::AtomicModel& atom,
                                   vpz::AtomicModel& model,
                                   vpz::Conditions& conditions) =0;
    virtual void generateOutputPorts(graph::AtomicModel& atom) =0;
    virtual void generateVariables(utils::Template& tpl_) =0;
    virtual std::string getTemplate() const =0;
    void parseConf(const std::string& conf,
                   std::string& classname,
                   std::string& namespace_,
                   Parameters::Parameters_t& parameters,
                   Parameters::ExternalVariables_t& variables);
    void parseFunctions(const std::string& buffer);

private:
    void generateDynamic(vpz::Dynamic& dynamic,
                         const std::string& classname);
    void generatePorts(graph::AtomicModel& atom);
    void generateInputPorts(graph::AtomicModel& atom);
    void generateSource(const std::string& classname,
                        const std::string& namespace_,
                        bool generic);
    std::string parseFunction(const std::string& buffer,
                              const std::string& begin,
                              const std::string& end,
                              const std::string& name);
};

}}} // namespace vle gvle modeling

#endif
