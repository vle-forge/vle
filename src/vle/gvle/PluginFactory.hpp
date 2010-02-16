/**
 * @file vle/gvle/PluginFactory.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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
#define VLE_GVLE_PLUGINFACTORY_HPP

#include <vle/gvle/OutputPlugin.hpp>
#include <vle/gvle/ModelingPlugin.hpp>
#include <boost/noncopyable.hpp>

namespace vle { namespace gvle {

    typedef std::map < std::string, OutputPlugin* > OutputPluginList;
    typedef std::map < std::string, ModelingPlugin* > ModelingPluginList;

    /**
     * @brief The PluginFactory allows to load the outputs and conditions
     * plug-ins (ModelingPlugin and OutputPlugin). At start time, the
     * PluginFactory build a dictionary of available plug-ins. Use the
     * getModeling and getOutput to load plug-in.
     */
    class PluginFactory : public boost::noncopyable
    {
    public:
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
         * @brief Delete all loaded plug-ins and fill the outputs plug-ins list
         * with files readed from conditions and outputs directories.
         */
        void update();

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Manage plug-ins
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Load the dynamic library attached to the output name.
         * @param name The name of the output to load.
         * @throw utils::InternalError if name does not exist or if the plug-in
         * is not loadable.
         */
        void initOutput(const std::string& name);

        /**
         * @brief Load the dynamic library attached to the modeling name.
         * @param name The name of the modeling to load.
         * @throw utils::InternalError if name does not exist or if the plug-in
         * is not loadable.
         */
        void initModeling(const std::string& name);

        /**
         * @brief Delete the plug-in attached to the output name.
         * @param name The name of the output to delete.
         * @throw utils::InternalError if name does not exist.
         */
        void clearOutput(const std::string& name);

        /**
         * @brief Delete the plug-in attached to the modeling name.
         * @param name The name of the modeling to delete.
         * @throw utils::InternalError if name does not exist.
         */
        void clearModeling(const std::string& name);

        /**
         * @brief Delete the plug-in attached to the output name and delete
         * the key from outputs list.
         * @param name The name of the output to delete.
         * @throw utils::InternalError if name does not exist.
         */
        void eraseOutput(const std::string& name);

        /**
         * @brief Delete the plug-in attached to the modeling name and delete
         * the key from modelings list.
         * @param name The name of the modeling to delete.
         * @throw utils::InternalError if name does not exist.
         */
        void eraseModeling(const std::string& name);

        /**
         * @brief Get a reference to the specified output plug-in.
         * @param name The name of the output.
         * @return A reference to the output.
         * @throw utils::InternalError if name does not exist.
         */
        OutputPlugin& getOutput(const std::string& name);

        /**
         * @brief Get a reference to the specified modeling plug-in.
         * @param name The name of the modeling.
         * @return A reference to the modeling.
         * @throw utils::InternalError if name does not exist.
         */
        ModelingPlugin& getModeling(const std::string& name);

        /**
         * @brief Get a const reference to the specified output plug-in.
         * @param name The name of the output.
         * @return A const reference to the output.
         * @throw utils::InternalError if name does not exist.
         */
        const OutputPlugin& getOutput(const std::string& name) const;

        /**
         * @brief Get a const reference to the specified modeling plug-in.
         * @param name The name of the modeling.
         * @return A const reference to the modeling.
         * @throw utils::InternalError if name does not exist.
         */
        const ModelingPlugin& getModeling(const std::string& name) const;

        /**
         * @brief Get a constant reference to the output plug-ins list.
         * @return Get a constant reference.
         */
        const OutputPluginList& outputPlugins() const
        { return m_outs; }

        /**
         * @brief Get a constant reference to the modeling plug-ins list.
         * @return Get a constant reference.
         */
        const ModelingPluginList& modelingPlugins() const
        { return m_mods; }

    private:
        OutputPluginList m_outs;
        ModelingPluginList m_mods;

        void readOutputPlugins();
        void readModelingPlugins();
        void loadOutputPlugin(OutputPluginList::iterator it);
        void loadModelingPlugin(ModelingPluginList::iterator it);

        OutputPluginList::iterator getO(const std::string& name);
        OutputPluginList::const_iterator getO(const std::string& name) const;
        ModelingPluginList::iterator getM(const std::string& name);
        ModelingPluginList::const_iterator getM(const std::string& name) const;
    };

    /**
     * @brief Get a stream representation of this class.
     * @param out The output stream.
     * @param plg The factory.
     * @return The output stream.
     */
    std::ostream& operator<<(std::ostream& out, const PluginFactory& plg);

}} // namespace vle gvle

#endif
