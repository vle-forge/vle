/*
 * @file vle/gvle/modeling/difference-equation/Plugin.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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

namespace vle { namespace gvle { namespace modeling { namespace de {

class Plugin : public ModelingPlugin
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

    TimeStep mTimeStep;
    Mapping mMapping;
    Parameters mParameters;


    void backup();
    void generate(graph::AtomicModel& atom,
                  vpz::AtomicModel& model,
                  vpz::Dynamic& dynamic,
                  vpz::Conditions& conditions,
                  vpz::Observables& observables,
                  const std::string& classname,
                  const std::string& namespace_,
                  bool generic = false);
    virtual void generateCondition(graph::AtomicModel& atom,
                                   vpz::AtomicModel& model,
                                   vpz::Conditions& conditions) =0;
    virtual void generateObservables(graph::AtomicModel& atom,
                                     vpz::AtomicModel& model,
                                     vpz::Observables& observables) =0;
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

}}}} // namespace vle gvle modeling de

#endif
