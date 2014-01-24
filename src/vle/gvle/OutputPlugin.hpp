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


#ifndef VLE_GVLE_OUTPUTPLUGIN_HPP
#define VLE_GVLE_OUTPUTPLUGIN_HPP

#include <vle/gvle/DllDefines.hpp>
#include <vle/vpz/Output.hpp>
#include <vle/vpz/View.hpp>
#include <vle/version.hpp>
#include <gdkmm/pixbuf.h>
#include <boost/shared_ptr.hpp>

#define DECLARE_GVLE_OUTPUTPLUGIN(x)                            \
    extern "C" {                                                \
        GVLE_MODULE vle::gvle::OutputPlugin*                    \
        vle_make_new_gvle_output(const std::string& package,    \
                                 const std::string& library)    \
        {                                                       \
            return new x(package, library);                     \
        }                                                       \
                                                                \
        GVLE_MODULE void                                        \
        vle_api_level(vle::uint32_t* major,                     \
                      vle::uint32_t* minor,                     \
                      vle::uint32_t* patch)                     \
        {                                                       \
            *major = VLE_MAJOR_VERSION;                         \
            *minor = VLE_MINOR_VERSION;                         \
            *patch = VLE_PATCH_VERSION;                         \
        }                                                       \
    }

namespace vle { namespace gvle {

/**
 * @brief A virtual class to define Output plug-ins.
 * @code
 * class TmpSelector : public vle::gvle::OutputPlugin
 * {
 * vle::vpz::Output& m_output;
 *
 * public:
 *   virtual void start(vpz::Output& output, vpz::View&)
 *   {
 *      output.location(Glib::get_tmp_dir());
 *   }
 * };
 * @endcode
 */
class GVLE_API OutputPlugin
{
public:
    OutputPlugin(const std::string& package, const std::string& library);

    virtual ~OutputPlugin() {}

    /**
     * @brief Call to start the graphics interfaces. Use it to show the
     * Gtk::Dialog for instance. This function is called each time user select
     * the plug-in.
     * @param output The output to modify.
     * @param view The view to modify.
     * @return true if successful, otherwise false
     */
    virtual bool start(vpz::Output& output, vpz::View& view) = 0;

    /**
     * @brief Get a reference to the icon of this plug-in. If the plug-in have
     * not icon, or on error, the icon function return an empty pointer.
     * @return A reference to the icon or an empty pointer.
     */
    Glib::RefPtr < Gdk::Pixbuf > icon() const;


    const std::string& getPackage() const { return m_package; }

    const std::string& getLibrary() const { return m_library; }

private:
    /**
     * @brief Pointer to the icon.
     */
    mutable Glib::RefPtr < Gdk::Pixbuf > m_icon;

    /**
     * @brief Name of the package.
     */
    std::string m_package;

    /**
     * @brief Name of the plug-in.
     */
    std::string m_library;
};

typedef boost::shared_ptr < OutputPlugin > OutputPluginPtr;
typedef OutputPlugin* (*GvleOutputPluginSlot)(const std::string&,
                                              const std::string&);

}} // namespace vle gvle

#endif

