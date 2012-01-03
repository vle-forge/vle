/*
 * @file vle/gvle/PluginFactory.hpp
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


#ifndef VLE_GVLE_PLUGINFACTORY_HPP
#define VLE_GVLE_PLUGINFACTORY_HPP 1

#include <vle/gvle/OutputPlugin.hpp>
#include <vle/gvle/ModelingPlugin.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/utils/Module.hpp>
#include <boost/noncopyable.hpp>

namespace vle { namespace gvle {

/**
 * @brief The PluginFactory allows to load the outputs and conditions
 * plug-ins (ModelingPlugin and OutputPlugin). At start time, the
 * PluginFactory build a dictionary of available plug-ins. Use the
 * getModeling and getOutput to load plug-in.
 */
class PluginFactory : public boost::noncopyable
{
public:
    template < class T >
    class Plugin
    {
    public:
        Plugin(const std::string& name, const std::string& path,
               const std::string& package, T* plugin)
            : mName(name), mPath(path), mPackage(package), mPlugin(plugin)
        {}

        /**
         * @brief Represents the public string of the plugin. For instance, for
         * the generic plugin Storage, name is equal to `storage (generic)'.
         *
         * @return A string representation of the plug-in.
         */
        std::string string() const
        { return Plugin::generic(mName, mPackage); }

        const std::string& path() const { return mPath; }
        const std::string& name() const { return mName; }
        const std::string& package() const { return mPackage; }
        T* plugin() const { return mPlugin; }

        static std::string generic(const std::string& name,
                                   const std::string& package)
        {
            std::string result(name);

            result += " (";
            result += (package.empty() ? "generic" : package);
            result += ")";

            return result;
        }

    private:
        std::string mName; /**< Represents the name of the plug-in. For
                             example, for the generic plugin Storage,
                             pluginname is equal to `storage' and for package
                             plug'ins caview, pluginname is equal to
                             `caview'. */

        std::string mPath; /**< The complete key path of the dynamic library.
                             For instance, for the generic plugin Store, path
                             is equal to
                             `$prefix/usr/lib/vle-1.0/stream/libstorage.so'. */

        std::string mPackage; /**< Represents the name of the package. Empty if
                                the plug-in is generic. */

        T* mPlugin; /**< A pointer to the oov, modeling or output plug-in
                      loaded from the shared library. */
    };

    typedef Plugin < OutputPlugin > OutputPlg;
    typedef Plugin < ModelingPlugin > ModelingPlg;
    typedef Plugin < oov::Plugin > OovPlg;

    typedef std::map < std::string, OutputPlg > OutputPluginList;
    typedef std::map < std::string, ModelingPlg > ModelingPluginList;
    typedef std::map < std::string, OovPlg > OovPluginList;

    /**
     * @brief Fill the conditions and outputs plug-ins list with files
     * readed from conditions and outputs directories.
     */
    PluginFactory();

    /**
     * @brief Delete all loaded plug-ins.
     */
    ~PluginFactory();

    /**
     * @brief Try to read new oov, gvle-output or gvle-modeling plug-ins.
     */
    void update();

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage plug-ins
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Get a reference to the specified output plug-in.
     * @param name The name of the output.
     * @return A reference to the output.
     * @throw utils::InternalError if name does not exist.
     */
    PluginFactory::OutputPlg& getOutput(const std::string& name)
    { return getO(name)->second; }

    /**
     * @brief Get a const reference to the specified output plug-in.
     * @param name The name of the output.
     * @return A const reference to the output.
     * @throw utils::InternalError if name does not exist.
     */
    const PluginFactory::OutputPlg& getOutput(const std::string& name) const
    { return getO(name)->second; }

    /**
     * @brief Get a reference to the specified Oov plug-in.
     * @param name The name of the Oov-plugin.
     * @return A reference to the Oov-plugin.
     * @throw utils::InternalError if name does not exist.
     */
    PluginFactory::OovPlg& getOov(const std::string& name)
    { return getD(name)->second; }

    /**
     * @brief Get a const reference to the specified Oov plug-in.
     * @param name The name of the Oov-plugin.
     * @return A const reference to the Oov-plugin.
     * @throw utils::InternalError if name does not exist.
     */
    const PluginFactory::OovPlg& getOov(const std::string& name) const
    { return getD(name)->second; }

    /**
     * @brief Get a reference to the specified modeling plug-in.
     * @param name The name of the modeling.
     * @return A reference to the modeling.
     * @throw utils::InternalError if name does not exist.
     */
    PluginFactory::ModelingPlg& getModeling(const std::string& name)
    { return getM(name)->second; }

    /**
     * @brief Get a const reference to the specified modeling plug-in.
     * @param name The name of the modeling.
     * @return A const reference to the modeling.
     * @throw utils::InternalError if name does not exist.
     */
    const PluginFactory::ModelingPlg& getModeling(
        const std::string& name) const
    { return getM(name)->second; }

    /**
     * @brief Get a constant reference to the output plug-ins list.
     * @return Get a constant reference.
     */
    const OutputPluginList& outputPlugins() const { return m_outs; }

    /**
     * @brief Get a constant reference to the modeling plug-ins list.
     * @return Get a constant reference.
     */
    const ModelingPluginList& modelingPlugins() const { return m_mods; }

    /**
     * @brief Get a constant reference to the modeling plug-ins list.
     * @return Get a constant reference.
     */
    const OovPluginList& oovPlugins() const { return m_oov; }

private:
    OutputPluginList m_outs;
    ModelingPluginList m_mods;
    OovPluginList m_oov;

    void readOutputPlugins();
    void readModelingPlugins();
    void readOovPlugins();

    OutputPluginList::iterator getO(const std::string& name);
    OutputPluginList::const_iterator getO(const std::string& name) const;
    ModelingPluginList::iterator getM(const std::string& name);
    ModelingPluginList::const_iterator getM(const std::string& name) const;
    OovPluginList::iterator getD(const std::string& name);
    OovPluginList::const_iterator getD(const std::string& name) const;
};

}} // namespace vle gvle

#endif
