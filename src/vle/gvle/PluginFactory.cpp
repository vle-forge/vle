/**
 * @file vle/gvle/PluginFactory.cpp
 * @author The vle Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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

#include <vle/gvle/PluginFactory.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Algo.hpp>
#include <vle/utils/i18n.hpp>
#include <glibmm/module.h>
#include <boost/checked_delete.hpp>
#include <cassert>

namespace vle { namespace gvle {

PluginFactory::PluginFactory()
{
    readConditionPlugins();
    readOutputPlugins();
}

PluginFactory::~PluginFactory()
{
    utils::for_each(m_cnds.begin(), m_cnds.end(),
                    boost::checked_deleter < ConditionPlugin >());

    utils::for_each(m_outs.begin(), m_outs.end(),
                    boost::checked_deleter < OutputPlugin >());
}

void PluginFactory::initCondition(const std::string& name)
{
    ConditionPluginList::iterator it = getC(name);
    loadConditionPlugin(it);
}

void PluginFactory::initOutput(const std::string& name)
{
    OutputPluginList::iterator it = getO(name);
    loadOutputPlugin(it);
}

void PluginFactory::clearCondition(const std::string& name)
{
    ConditionPluginList::iterator it = getC(name);
    delete it->second;
    it->second = 0;
}

void PluginFactory::clearOutput(const std::string& name)
{
    OutputPluginList::iterator it = getO(name);
    delete it->second;
    it->second = 0;
}

void PluginFactory::eraseCondition(const std::string& name)
{
    ConditionPluginList::iterator it = getC(name);
    delete it->second;
    it->second = 0;

    m_cnds.erase(it);
}

void PluginFactory::eraseOutput(const std::string& name)
{
    OutputPluginList::iterator it = getO(name);
    delete it->second;
    it->second = 0;

    m_outs.erase(it);
}

ConditionPlugin& PluginFactory::getCondition(const std::string& name)
{
    ConditionPluginList::iterator it = getC(name);

    if (not it->second) {
        loadConditionPlugin(it);
    }

    return *it->second;
}

OutputPlugin& PluginFactory::getOutput(const std::string& name)
{
    OutputPluginList::iterator it = getO(name);

    if (not it->second) {
        loadOutputPlugin(it);
    }

    return *it->second;
}

const ConditionPlugin& PluginFactory::getCondition(const std::string& name) const
{
    ConditionPluginList::const_iterator it = getC(name);

    if (not it->second) {
        throw utils::InternalError(fmt(_(
                "ConditionPlugin '%1%' is not initialized")) % name);
    }

    return *it->second;
}

const OutputPlugin& PluginFactory::getOutput(const std::string& name) const
{
    OutputPluginList::const_iterator it = getO(name);

    if (not it->second) {
        throw utils::InternalError(fmt(_(
                "Outputplugin '%1%' is not initialized")) % name);
    }

    return *it->second;
}

OutputPluginList::iterator PluginFactory::getO(const std::string& name)
{
    OutputPluginList::iterator it = m_outs.find(name);
    if (it == m_outs.end()) {
        throw utils::InternalError(fmt(_(
                "Outputplugin '%1%' is not available")) % name);
    }
    return it;
}

ConditionPluginList::iterator PluginFactory::getC(const std::string& name)
{
    ConditionPluginList::iterator it = m_cnds.find(name);
    if (it == m_cnds.end()) {
        throw utils::InternalError(fmt(_(
                "Condition plugin '%1%' is not available")) % name);
    }
    return it;
}

OutputPluginList::const_iterator PluginFactory::getO(
    const std::string& name) const
{
    OutputPluginList::const_iterator it = m_outs.find(name);
    if (it == m_outs.end()) {
        throw utils::InternalError(fmt(_(
                "Outputplugin '%1%' is not available")) % name);
    }
    return it;
}

ConditionPluginList::const_iterator PluginFactory::getC(
    const std::string& name) const
{
    ConditionPluginList::const_iterator it = m_cnds.find(name);
    if (it == m_cnds.end()) {
        throw utils::InternalError(fmt(_(
                "Condition plugin '%1%' is not available")) % name);
    }
    return it;
}

void PluginFactory::readConditionPlugins()
{
    utils::PathList paths = utils::Path::path().getConditionDirs();
    utils::PathList::iterator it = paths.begin();

    while (it != paths.end()) {
        if (Glib::file_test(*it, Glib::FILE_TEST_EXISTS)) {
            Glib::Dir rep(*it);
            Glib::DirIterator in = rep.begin();
            while (in != rep.end()) {
#ifdef G_OS_WIN32
                if (((*in).find("lib") == 0) &&
                    ((*in).rfind(".dll") == (*in).size() - 4)) {
                    m_cnds[(*in).substr(3, (*in).size() - 7)] = 0;
                }
#else
                if (((*in).find("lib") == 0) &&
                    ((*in).rfind(".so") == (*in).size() - 3)) {
                    m_cnds[(*in).substr(3, (*in).size() - 6)] = 0;
                }
#endif
                in++;
            }
        }
        it++;
    }
}

void PluginFactory::readOutputPlugins()
{
    utils::PathList paths = utils::Path::path().getOutputDirs();
    utils::PathList::iterator it = paths.begin();

    while (it != paths.end()) {
        if (Glib::file_test(*it, Glib::FILE_TEST_EXISTS)) {
            Glib::Dir rep(*it);
            Glib::DirIterator in = rep.begin();
            while (in != rep.end()) {
#ifdef G_OS_WIN32
                if (((*in).find("lib") == 0) &&
                    ((*in).rfind(".dll") == (*in).size() - 4)) {
                    m_outs[(*in).substr(3, (*in).size() - 7)] = 0;
                }
#else
                if (((*in).find("lib") == 0) &&
                    ((*in).rfind(".so") == (*in).size() - 3)) {
                    m_outs[(*in).substr(3, (*in).size() - 6)] = 0;
                }
#endif
                in++;
            }
        }
        it++;
    }
}

void PluginFactory::loadConditionPlugin(ConditionPluginList::iterator it)
{
    assert(it != m_cnds.end());

    for (utils::PathList::const_iterator jt =
         utils::Path::path().getConditionDirs().begin(); jt !=
         utils::Path::path().getConditionDirs().end(); ++jt) {

        std::string filename = Glib::Module::build_path(*jt, it->first);
        Glib::Module mdl(filename);
        if (mdl) {
            mdl.make_resident();

            typedef ConditionPlugin* (*function)(const std::string&);
            void* fctptr = 0;
            ConditionPlugin* plg = 0;

            if (mdl.get_symbol("makeNewConditionPlugin", fctptr)) {
                function fct(utils::pointer_to_function < function >(fctptr));
                plg = fct(it->first);
            } else {
                throw utils::InternalError(fmt(_(
                        "ConditionPlugin '%1%': error opening, %2%")) %
                    it->first % Glib::Module::get_last_error());
            }

            it->second = plg;
            return;
        }
    }
    throw utils::InternalError(fmt(_(
            "ConditionPlugin '%1%': plug-in not found")) % it->first);
}

void PluginFactory::loadOutputPlugin(OutputPluginList::iterator it)
{
    assert(it != m_outs.end());

    for (utils::PathList::const_iterator jt =
         utils::Path::path().getOutputDirs().begin(); jt !=
         utils::Path::path().getOutputDirs().end(); ++jt) {

        std::string filename = Glib::Module::build_path(*jt, it->first);
        Glib::Module mdl(filename);
        if (mdl) {
            mdl.make_resident();

            typedef OutputPlugin* (*function)(const std::string&);
            void* fctptr = 0;
            OutputPlugin* plg = 0;

            if (mdl.get_symbol("makeNewOutputPlugin", fctptr)) {
                function fct(utils::pointer_to_function < function >(fctptr));
                plg = fct(it->first);
            } else {
                throw utils::InternalError(fmt(
                        "OutputPlugin '%1%': error opening, %2%") % it->first %
                    Glib::Module::get_last_error());
            }

            it->second = plg;
            return;
        }
    }
    throw utils::InternalError(fmt(
            "OutputPlugin '%1%': plug-in not found") % it->first);
}

std::ostream& operator<<(std::ostream& out, const PluginFactory& plg)
{
    out << "Condition: ";

    for (ConditionPluginList::const_iterator it =
         plg.conditionPlugins().begin(); it != plg.conditionPlugins().end();
         ++it) {
        out << "(" << it->first << ", " << it->second << ")\n";
    }

    out << "Output: ";
    for (OutputPluginList::const_iterator it = plg.outputPlugins().begin(); it
         != plg.outputPlugins().end(); ++it) {
        out << "(" << it->first << ", " << it->second << ")\n";
    }

    return out;
}

}} // namespace vle gvle
