/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
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


#ifndef GUI_OBSERVERPLUGIN_HPP
#define GUI_OBSERVERPLUGIN_HPP

#include <string>
#include <list>

namespace vle
{
namespace gvle {

#define DECLARE_OBSERVER_PLUGIN(x) \
    extern "C" { ObserverPlugin* makeNewPlugin(void) { return new x(); } }

/**
 * @brief Define the ObserverPlugin virtual class. It is use to build new
 * Observer plugin.
 * You must add a C-function makeNewPlugin to perform
 * dynamic class loading. An
 * instance of this function for Level:
 *
 * @code
 * extern "C"
 * {
 *     static ObserverPlugin* newPlugin(void)
 *     {
 *         return new LevelPlugin();
 *     }
 * }
 *
 * With LevelPlugin is:
 *
 * class LevelPlugin : public ObserverPlugin
 * {
 * public:
 *     LevelPlugin();
 *     ...
 * }
 * @endcode
 */
class ObserverPlugin
{
public:
    /**
     * initialize plugin
     */
    ObserverPlugin() {}

    /**
     * delete all allocated data
     */
    virtual ~ObserverPlugin() {}

    /**
     * return true if plugin use graphics interface
     *
     * @return true or false
     */
    virtual bool hasGraphicsInterface() = 0;

    /**
     * return description of graphic interface
     *
     * @return a description
     */
    virtual std::string getDescription() = 0;

    /**
     * return the name of observer like "Plot", "Level"... Be carreful,
     * unique name must be use
     *
     * @return a name
     */
    virtual std::string getObserverName() = 0;

    /**
     * return a list of category name where plugin can take place.
     * If you return an empty list, then plugin is enabled but you can't
     * select them from gvle plugin list.
     * Use this syntax to declare your category name : [A-Z][a-z]*
     *
     * @return plugin list of string represent category name of plugin
     */
    virtual std::list < std::string > getCategory() = 0;

    /****************************************************************
     *
     * MANAGE GRAPHICS INTERFACE
     *
     ****************************************************************/

    /**
     * call to initialize graphics interface. init() is call
     * before start.
     *
     * @return true if successful, otherwise false
     */
    virtual bool init() = 0;

    /**
     * call to get XML information from plugin. Structure and Dynamic must
     * be set with good XML application VLE
     *
     * @param parameters param receive parameter XML
     * @return true if successful, otherwise false
     */
    virtual bool getXML(std::string& parameters) = 0;

    /**
     * call to get XML information from plugin. Structure and Dynamic must
     * be set with good XML application. This function is call when a plugin
     * is already define. Param structure and dynamic contains already
     * existent data to initialise your plugin
     *
     * @param parameters param receive parameter XML
     * @return true if successful, otherwise false
     */
    virtual bool getNextXML(std::string& parameters) = 0;

    /**
     * delete graphics interface. finish() is call in last time of
     * application GVLE.
     *
     * @return true if successful, otherwise false
     */
    virtual bool finish() = 0;

    virtual bool run() {
        return true;
    }
};

}
} // namespace vle gvle

#endif
