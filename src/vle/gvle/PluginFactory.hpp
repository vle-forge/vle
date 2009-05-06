/**
 * @file vle/gvle/PluginFactory.hpp
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

#ifndef VLE_GVLE_PLUGINFACTORY_HPP
#define VLE_GVLE_PLUGINFACTORY_HPP

#include <vle/gvle/ConditionPlugin.hpp>
#include <vle/gvle/OutputPlugin.hpp>
#include <boost/noncopyable.hpp>

namespace vle { namespace gvle {

    typedef std::map < std::string, ConditionPlugin* > ConditionPluginList;
    typedef std::map < std::string, OutputPlugin* > OutputPluginList;

    /**
     * @brief The PluginFactory allows to load the outputs and conditions
     * plug-ins (ConditionPlugin and OutputPlugin). At start time, the
     * PluginFactory build a dictionary of available plug-ins. Use the
     * getCondition and getOutput to load plug-in.
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

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Manage plug-ins
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Load the dynamic library attached to the condition name.
         * @param name The name of the condition to load.
         * @throw utils::InternalError if name does not exist or if the plug-in
         * is not loadable.
         */
        void initCondition(const std::string& name);

        /**
         * @brief Load the dynamic library attached to the output name.
         * @param name The name of the output to load.
         * @throw utils::InternalError if name does not exist or if the plug-in
         * is not loadable.
         */
        void initOutput(const std::string& name);

        /**
         * @brief Delete the plug-in attached to the condition name.
         * @param name The name of the condition to delete.
         * @throw utils::InternalError if name does not exist.
         */
        void clearCondition(const std::string& name);

        /**
         * @brief Delete the plug-in attached to the output name.
         * @param name The name of the output to delete.
         * @throw utils::InternalError if name does not exist.
         */
        void clearOutput(const std::string& name);

        /**
         * @brief Delete the plug-in attached to the condition name and delete
         * the key from conditions list.
         * @param name The name of the condition to delete.
         * @throw utils::InternalError if name does not exist.
         */
        void eraseCondition(const std::string& name);

        /**
         * @brief Delete the plug-in attached to the output name and delete
         * the key from outputs list.
         * @param name The name of the output to delete.
         * @throw utils::InternalError if name does not exist.
         */
        void eraseOutput(const std::string& name);

        /**
         * @brief Get a reference to the specified condition plug-in.
         * @param name The name of the condition.
         * @return A reference to the condition.
         * @throw utils::InternalError if name does not exist.
         */
        ConditionPlugin& getCondition(const std::string& name);

        /**
         * @brief Get a reference to the specified output plug-in.
         * @param name The name of the output.
         * @return A reference to the output.
         * @throw utils::InternalError if name does not exist.
         */
        OutputPlugin& getOutput(const std::string& name);

        /**
         * @brief Get a const reference to the specified condition plug-in.
         * @param name The name of the condition.
         * @return A const reference to the condition.
         * @throw utils::InternalError if name does not exist.
         */
        const ConditionPlugin& getCondition(const std::string& name) const;

        /**
         * @brief Get a const reference to the specified output plug-in.
         * @param name The name of the output.
         * @return A const reference to the output.
         * @throw utils::InternalError if name does not exist.
         */
        const OutputPlugin& getOutput(const std::string& name) const;

        /**
         * @brief Get a constant reference to the condition plug-ins list.
         * @return Get a constant reference.
         */
        const ConditionPluginList& conditionPlugins() const
        { return m_cnds; }

        /**
         * @brief Get a constant reference to the output plug-ins list.
         * @return Get a constant reference.
         */
        const OutputPluginList& outputPlugins() const
        { return m_outs; }

    private:
        ConditionPluginList m_cnds;
        OutputPluginList m_outs;

        void readConditionPlugins();
        void readOutputPlugins();
        void loadOutputPlugin(OutputPluginList::iterator it);
        void loadConditionPlugin(ConditionPluginList::iterator it);

        ConditionPluginList::iterator getC(const std::string& name);
        ConditionPluginList::const_iterator getC(const std::string& name) const;
        OutputPluginList::iterator getO(const std::string& name);
        OutputPluginList::const_iterator getO(const std::string& name) const;
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
