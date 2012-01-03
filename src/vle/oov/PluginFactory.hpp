/*
 * @file vle/oov/PluginFactory.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
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


#ifndef VLE_OOV_PLUGINFACTORY_HPP
#define VLE_OOV_PLUGINFACTORY_HPP 1

#include <vle/oov/DllDefines.hpp>
#include <vle/oov/Plugin.hpp>
#include <string>

namespace vle { namespace oov {

/**
 * @brief PluginFactory is used to build boost::shared_ptr to the oov::Plugin
 * dynamically load from the dynamics libraries where the symbol
 * `makeNewOovPlugin' exists.
 *
 */
class VLE_OOV_EXPORT PluginFactory
{
public:
    /** 
     * @brief Define the pointer to function of the oov::Plugin plug-in.
     * @param The output location of the oov::Plugin.
     * @return A pointer to the oov::Plugin newly build.
     */
    typedef Plugin* (*function)(const std::string&);

    /**
     * @brief Constructor to load oov::Plugin from pathname.
     * @param plugin the name of the oov::Plugin to load.
     * @param path the name of the path where the oov::Plugin is.
     * @throw utils::InternalError if the oov::Plugin does not exist in the
     * pathname.
     */
    PluginFactory(const std::string& path, const std::string& plugin)
        : mPlugin(plugin), mPath(path)
    {}

    ~PluginFactory()
    {}

    PluginPtr build(const std::string& location);

private:
    std::string mPlugin; /**< The name of the plugin, for instance `storage' or
                           `file'. */
    std::string mPath; /**< The path of the plugin, for instance a global
                         plugin or a package plugin path. */
};

}} // namespace vle oov

#endif
