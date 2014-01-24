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


#ifndef VLE_OOV_CAIRO_PLUGIN_HPP
#define VLE_OOV_CAIRO_PLUGIN_HPP

#include <vle/DllDefines.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/value/Map.hpp>
#include <boost/shared_ptr.hpp>
#include <cairomm/cairomm.h>

namespace vle { namespace oov {

/**
 * @brief The vle::oov::CairoPlugin is a class to develop cairo output steam
 * plugins. Do not forget the plugin declaration:
 * @code
 * DECLARE_OOV_PLUGIN(Gnuplot);
 * @endcode
 */
class VLE_API CairoPlugin : public Plugin
{
public:
    /**
     * @brief Default constructor of the CairoPlugin.
     * @param location this string represents the name of the default
     * directory for a devs::LocalStreamWriter or a host:port:directory for
     * a devs::DistantStreamWriter.
     */
    CairoPlugin(const std::string& location)
        : Plugin(location), m_framenumber(0), m_need(false),
        m_init(true), m_copydone(false)
    {}

    /**
     * @brief Nothing to delete.
     */
    virtual ~CairoPlugin()
    {}

    /**
     * @brief Build the Cairo::Context and the Cairo::ImageSurface. The size
     * of the images are taken from the preferredSize function.
     */
    void init();

    ///
    ////
    ///

    /**
     * @brief Return true, CairoPlugin is a Cairo plugin.
     * @return true.
     */
    virtual bool isCairo() const
    { return true; }

    /**
     * @brief Use to react to an external event for instance, from an
     * graphical user interface. Example:
     * @code
     * onSignal("switchcolor", value::MapFactory::create());
     * @endcode
     * @param method the name of the method.
     * @param params the value::Map of parameters.
     */
    virtual void onSignal(const std::string& /* method */,
                          const value::Map& /* params */)
    {}

    /**
     * @brief Use to react to a modification of the external Cairo::Surface.
     * @param width the new width of the Cairo::Surface.
     * @param height the new height of the Cairo::Surface.
     */
    virtual void onSize(int /* width */, int /* height */)
    {}

    /**
     * @brief Use to define the prefered size of the CairoSurface of the
     * CairoPlugin.
     * @param width output parameter for the prefered width of the
     * internal Cairo::Surface.
     * @param height output parameter for the prefred height of the
     * internal Cairo::Surface.
     */
    virtual void preferredSize(int& /* width */, int& /* height */) = 0;

    ///
    ////
    ///

    /**
     * @brief Call this function to append an order to build new stored
     * image.
     */
    void needCopy()
    { m_need = true; m_copydone = false; }

    void needInit()
    { m_init = true; m_copydone = false; }

    /**
     * @brief Get the state of the image buffer copy.
     * @return True if the
     */
    bool isCopyDone() const
    { return m_copydone; }

    /**
     * Get the next frame number.
     *
     * The \c getNextFrameNumber increments the number of frame and
     * returns the currect frame number.
     *
     * @return An integer between 0 and MAX_INT.
     */
    unsigned int getNextFrameNumber()
    {
        unsigned int i = m_framenumber++;
        return i;
    }

    /**
     * @brief Get a reference to the latest image produce by this plugin.
     * @return A reference to an ImageSurface.
     */
    Cairo::RefPtr < Cairo::ImageSurface > stored()
    { return m_store; }

    /**
     * @brief Call this function to copy the drawing ImageSurface into the
     * stored ImageSurface for the public API.
     */
    void copy();

protected:
    /**
     * @brief Context to draw onto the m_img ImageSurface.
     */
    Cairo::RefPtr < Cairo::Context > m_ctx;

    /**
     * @brief ImageSurface to draw.
     */
    Cairo::RefPtr < Cairo::ImageSurface > m_img;

    /**
     * @brief The public ImageSurface to write.
     */
    Cairo::RefPtr < Cairo::ImageSurface > m_store;

private:
    /// A integer to inform the index of the current frame.
    unsigned int m_framenumber;

    /**
     * @brief A boolean to inform CairoPlugin to copy or not the
     * ImageSurface into the stored ImageSurface.
     */
    bool m_need;

    /**
     * @brief A boolean to inform CairoPlugin to re-initialize all the
     * Cairo::Surface.
     */
    bool m_init;

    /**
     * @brief A boolean to inform user that a new copy as been build and
     * can be get with stored() function.
     */
    bool m_copydone;
};

/**
 * @brief This typedef is used by the StreamReader to automatically destroy
 * CairoPlugin at the end of the simulation.
 */
typedef boost::shared_ptr < CairoPlugin > CairoPluginPtr;

/**
 * @brief Convert a PluginPtr reference to a CairoPluginPtr reference.
 * @param plg The PluginPtr to convert.
 * @return The reference to the CairoPluginPtr or 0 if convert failed.
 */
inline CairoPluginPtr toCairoPlugin(const PluginPtr& plg)
{ return boost::dynamic_pointer_cast < CairoPlugin >(plg); }

}} // namespace vle oov

#endif
