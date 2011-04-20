/*
 * @file vle/gvle/modeling/difference-equation/Plugin.cpp
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


#include <vle/gvle/modeling/difference-equation/Plugin.hpp>
#include <vle/gvle/modeling/difference-equation/SourceDialog.hpp>
#include <vle/utils/Package.hpp>
#include <boost/regex.hpp>

namespace vle { namespace gvle { namespace modeling { namespace de {

Plugin::Plugin(const std::string& name)
    : ModelingPlugin(name)
{
}

Plugin::~Plugin()
{
}

void Plugin::backup()
{
    mOldMode = mMapping.getMode();

    Parameters::ExternalVariables_t externalVariables =
        mParameters.getExternalVariables();
    for (Parameters::ExternalVariables_t::const_iterator it =
             externalVariables.begin(); it != externalVariables.end(); ++it) {
        mOldExternalVariables.push_back(it->first);
    }
}

void Plugin::generate(graph::AtomicModel& atom,
                      vpz::AtomicModel& model,
                      vpz::Dynamic& dynamic,
                      vpz::Conditions& conditions,
                      vpz::Observables& observables,
                      const std::string& classname,
                      const std::string& namespace_,
                      bool generic)
{
    generatePorts(atom);
    generateCondition(atom, model, conditions);
    generateDynamic(dynamic, classname);
    generateSource(classname, namespace_, generic);
    generateObservables(atom, model, observables);
}

void Plugin::generateDynamic(vpz::Dynamic& dynamic,
                             const std::string& classname)
{
    dynamic.setPackage(utils::Package::package().name());
    dynamic.setLibrary(classname);
    dynamic.setModel("");
}

void Plugin::generateInputPorts(graph::AtomicModel& atom)
{
    if (mOldMode != mMapping.getMode()) {
        if (mOldMode == "name") {
            if (atom.existInputPort("update")) {
                atom.delInputPort("update");
            }
        } else if (mOldMode == "port") {
            for (std::vector < std::string >::const_iterator it =
                     mOldExternalVariables.begin();
                 it != mOldExternalVariables.end(); ++it) {
                if (atom.existInputPort(*it)) {
                    atom.delInputPort(*it);
                }
            }
        } else if (mOldMode == "mapping") {
        }
    }

    if (mMapping.getMode() == "name") {
        if (not atom.existInputPort("update")) {
            atom.addInputPort("update");
        }
    } else if (mMapping.getMode() == "port") {
        Parameters::ExternalVariables_t externalVariables =
            mParameters.getExternalVariables();
        for (Parameters::ExternalVariables_t::const_iterator it =
             externalVariables.begin(); it != externalVariables.end(); ++it) {
            if (not atom.existInputPort(it->first)) {
                atom.addInputPort(it->first);
            }
        }
    } else if (mMapping.getMode() == "mapping") {
    }

    if (not atom.existInputPort("perturb")) {
        atom.addInputPort("perturb");
    }
}

void Plugin::generatePorts(graph::AtomicModel& atom)
{
    generateInputPorts(atom);
    generateOutputPorts(atom);
}

void Plugin::generateSource(const std::string& classname,
                            const std::string& namespace_,
                            bool generic)
{
    utils::Template tpl_(getTemplate());

    tpl_.stringSymbol().append("namespace", namespace_);
    tpl_.stringSymbol().append("classname", classname);

// variables
    generateVariables(tpl_);

// parameters
    tpl_.listSymbol().append("par");
    tpl_.listSymbol().append("val");

    Parameters::Parameters_t parameters = mParameters.getParameters();
    for (std::vector < std::string >::const_iterator it =
             parameters.mNames.begin(); it != parameters.mNames.end(); ++it) {
        std::string name(*it);
        std::map < std::string, double >::const_iterator it2 =
            parameters.mValues.find(name);
        tpl_.listSymbol().append("par", name);
        if (it2 != parameters.mValues.end()) {
            tpl_.listSymbol().append("val",
                                     boost::lexical_cast < std::string >(
                                         it2->second));
        } else {
            tpl_.listSymbol().append("val", "*");
        }
    }

    if (not generic) {
// syncs / nosyncs
        tpl_.listSymbol().append("sync");
        tpl_.listSymbol().append("nosync");

        Parameters::ExternalVariables_t externalVariables =
            mParameters.getExternalVariables();
        for (Parameters::ExternalVariables_t::const_iterator it =
                 externalVariables.begin(); it != externalVariables.end();
             ++it) {
            std::string name(it->first);

            if (it->second) {
                tpl_.listSymbol().append("sync", name);
            } else {
                tpl_.listSymbol().append("nosync", name);
            }
        }
    }

// includes/compute/initValue/user-functions
    tpl_.stringSymbol().append("includes", mIncludes);
    tpl_.stringSymbol().append("compute", mComputeFunction);
    tpl_.stringSymbol().append("initValue", mInitValueFunction);
    tpl_.stringSymbol().append("userFunctions", mUserFunctions);

    std::ostringstream out;
    tpl_.process(out);

    mSource = out.str();
}

void Plugin::onSource()
{
    SourceDialog box(mXml, mIncludes, mComputeFunction, mInitValueFunction,
                     mUserFunctions);

    if (box.run() == Gtk::RESPONSE_ACCEPT) {
        mIncludes = box.getIncludes();
        mComputeFunction = box.getComputeFunction();
        mInitValueFunction = box.getInitValueFunction();
        mUserFunctions = box.getUserFunctions();
    }
}

void Plugin::parseConf(const std::string& conf,
                       std::string& classname,
                       std::string& namespace_,
                       Parameters::Parameters_t& parameters,
                       Parameters::ExternalVariables_t& variables)
{
    std::vector < std::string > lst(5);

    boost::split(lst, conf, boost::is_any_of(";"));

// namespace
    namespace_ = std::string(lst[0], 10, lst[0].size() - 10);

// classname
    classname = std::string(lst[1], 6, lst[1].size() - 6);

// parameters
    {
        std::string pars(lst[2], 4, lst[2].size() - 4);
        std::vector < std::string > lstpar;
        boost::split(lstpar, pars, boost::is_any_of("|"));
        for (std::vector < std::string >::const_iterator it = lstpar.begin();
             it != lstpar.end(); ++it) {
            if (not it->empty()) {
                std::vector < std::string > par;
                boost::split(par, *it, boost::is_any_of(","));
                parameters.mNames.push_back(par[0]);
                if (par[1] != "*") {
                    parameters.mValues[par[0]] =
                        boost::lexical_cast < double >(par[1]);
                }
            }
        }
    }

    if (lst.size() > 3) {
// syncs
        {
            std::string syncs(lst[3], 5, lst[3].size() - 5);
            std::vector < std::string > lstsync;
            boost::split(lstsync, syncs, boost::is_any_of("|"));
            for (std::vector < std::string >::const_iterator it =
                     lstsync.begin(); it != lstsync.end(); ++it) {
                if (not it->empty()) {
                    variables.push_back(make_pair(*it, true));
                }
            }
        }

// nosyncs
        {
            std::string nosyncs(lst[4], 7, lst[4].size() - 7);
            std::vector < std::string > lstnosync;
            boost::split(lstnosync, nosyncs, boost::is_any_of("|"));
            for (std::vector < std::string >::const_iterator it =
                     lstnosync.begin(); it != lstnosync.end(); ++it) {
                if (not it->empty()) {
                    variables.push_back(make_pair(*it, false));
                }
            }
        }
    }
}

std::string Plugin::parseFunction(const std::string& buffer,
                                  const std::string& begin,
                                  const std::string& end,
                                  const std::string& name)
{
    boost::regex tagbegin(begin, boost::regex::grep);
    boost::regex tagend(end, boost::regex::grep);

    boost::sregex_iterator it(buffer.begin(), buffer.end(), tagbegin);
    boost::sregex_iterator jt(buffer.begin(), buffer.end(), tagend);
    boost::sregex_iterator itend;

    if (it == itend or jt == itend) {
        throw utils::ArgError(fmt(_("DifferenceEquation plugin error, " \
                                    "no begin or end tag (%1%)")) % name);
    }

    if ((*it)[0].second >= (*jt)[0].first) {
        throw utils::ArgError(fmt(_("DifferenceEquation plugin error, " \
                                    "bad tag (%1%)")) % name);
    }

    return std::string((*it)[0].second + 1, (*jt)[0].first);
}

void Plugin::parseFunctions(const std::string& buffer)
{
    mIncludes.assign(parseFunction(buffer, "//@@begin:includes@@",
                                          "//@@end:includes@@", "includes"));
    mComputeFunction.assign(parseFunction(buffer, "//@@begin:compute@@",
                                          "//@@end:compute@@", "compute"));
    mInitValueFunction.assign(parseFunction(buffer, "//@@begin:initValue@@",
                                            "//@@end:initValue@@",
                                            "initValue"));
    mUserFunctions.assign(parseFunction(buffer, "//@@begin:user@@",
                                        "//@@end:user@@", "user"));
}

}}}} // namespace vle gvle modeling de
