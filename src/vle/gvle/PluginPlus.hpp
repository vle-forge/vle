/**
 * @file vle/gvle/PluginPlus.hpp
 * @author The VLE Development Team
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


#ifndef GUI_PLUGINPLUS_HPP
#define GUI_PLUGINPLUS_HPP

#include <vle/gvle/Modeling.hpp>
#include <gdkmm/pixbuf.h>
#include <string>

namespace vle
{
namespace gvle {

/**
 * @brief A virtual class to define the modelling plugin plus. This type of
 * plugin have an access on the gui::Modeling class.
 * PluginPlus extends Plugin to define a Plugin with entire GVLE
 * reference. This PluginPlus is must dependant on GVLE that Plugin simple.
 *
 * You must add a C-function newPlugin to perform dynamic class loading. An
 * instance of this function for ODE wrapper:
 *
 * @code
 * extern "C"
 * {
 *     static Plugin* newPlugin(void)
 *     {
 *         return new PluginODE();
 *     }
 * }
 *
 * With PluginODE is:
 *
 * class PluginODE : public Plugin
 * {
 * public:
 *     PluginODE();
 *     ...
 * }
 * @endcode
 */
class PluginPlus : public Plugin
{
public:
    /**
     * initialize plugin
     *
     * @param modeling Modeling class to get access to entire GVLE
     * application.
     */
    PluginPlus() : mModeling(NULL) {}

    /**
     * delete all allocated data
     */
    virtual ~PluginPlus() {}

    /**
     * set Modeling reference to allow plugin access to entire GVLE
     * application.
     *
     * @param modeling Modeling reference
     */
    inline void setModeling(Modeling* modeling) {
        mModeling = modeling;
    }

    /**
     * return Modeling reference to get access to entire GVLE
     * application.
     *
     * @return Modeling reference
     */
    inline Modeling* getModeling() {
        return mModeling;
    }

    /**
     * return true if plugin use graphics interface
     *
     * @return true or false
     */
    virtual bool hasGraphicsInterface() = 0;

    /**
     * return true if plugin use a Translator to generate structure
     *
     * @return true if plugin use Translator, false otherwise
     */
    virtual bool useTranslator() = 0;

    /**
     * return description of graphic interface
     *
     * @return a description
     */
    virtual std::string getDescription() = 0;

    /**
     * return the name of formalism like "ODE", "PetriNet"... Be
     * carreful, unique name must be use
     *
     * @return a name
     */
    virtual std::string getFormalismName() = 0;

    /**
     * return a list of category name where plugin can take place, like :
     * - 'Agent' M.A.S. category plugin
     * - 'Counter' counter category
     * - 'Generator' generator category
     * - 'Misc' no category plugin
     * If you return an empty list, then plugin is enabled but you can't
     * select them from gvle plugin list.
     * Use this syntax to declare your category name : [A-Z][a-z]*
     *
     * @return plugin list of string represent category name of plugin
     */
    virtual std::list < std::string > getCategory() = 0;

    /**
     * return an image representing formalism. Image use as icon in GVLE
     * main window. Imperative format are :
     * - 24 x 24 px in width and heigth,
     * - File Format png, bmp or jpeg
     *
     * @return an image icon
     */
    virtual Glib::RefPtr < Gdk::Pixbuf >& getIcon() = 0;

    /**
     * return an image representing formalism. Image use as graphics
     * representation in View window. Impertive format are :
     * - Maximum of 50 x 50 px in width and heigth, all image with superior
     *   width or height are resized
     * - File format png, bmp or jpeg
     *
     * @return an image
     */
    virtual Glib::RefPtr < Gdk::Pixbuf >& getImage() = 0;

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
     * call to get XML information from plugin. Structure and Dynamic
     * must be set with good XML application VLE
     *
     * @param structure param receive structure XML
     * @param dynamic param receive dynamic XML
     * @return true if successful, otherwise false
     */
    virtual bool getXML(std::string& structure, std::string& dynamic) = 0;

    /**
     * call to get XML information from plugin. Structure and Dynamic
     * must be set with good XML application. This function is call when
     * a plugin is already define. Param structure and dynamic contains
     * already existent data to initialise your plugin
     *
     * @param structure precedent XML structure and receive next structure
     * @param dynamic precedent XML dynamic and receive next dynamic
     * @return true if successful, otherwise false
     */
    virtual bool getNextXML(std::string& structure,
                            std::string& dynamic) = 0;

    /**
     * delete graphics interface. finish() is call in last time of
     * application GVLE.
     *
     * @return true if successful, otherwise false
     */
    virtual bool finish() = 0;

protected:
    Modeling*   mModeling;
};

}
} // namespace vle gvle

#endif
