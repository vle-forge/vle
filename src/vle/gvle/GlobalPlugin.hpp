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


#ifndef VLE_GVLE_GLOBALPLUGIN_HPP
#define VLE_GVLE_GLOBALPLUGIN_HPP

#include <vle/DllDefines.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/version.hpp>
#include <gdkmm/pixbuf.h>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#define DECLARE_GVLE_GLOBALPLUGIN(x)                            \
    extern "C" {						\
    VLE_MODULE vle::gvle::GlobalPlugin*				\
    vle_make_new_gvle_global(const std::string& package,	\
			     const std::string& library,	\
			     vle::gvle::GVLE* gvle)	\
        {                                                       \
	  return new x(package, library, gvle);			\
        }                                                       \
                                                                \
        VLE_MODULE void                                         \
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

class GVLE;

/**
 * @brief Define a plug-in to build source code, DEVS atomic model and its
 * experiment infortion (dynamics, conditions, observations).
 */
class VLE_API GlobalPlugin
{
public:

  GlobalPlugin(const std::string& package, const std::string& library,
	       GVLE* gvle)
    : mPackage(package), mLibrary(library), mGVLE(gvle)
  {}

    virtual ~GlobalPlugin()
    { }

    virtual void run()
    { }

    /**
     * @brief Get a reference to the icon of this plug-in. If the plug-in have
     * not icon, or on error, the icon function return an empty pointer.
     * @return A reference to the icon or an empty pointer.
     */
    Glib::RefPtr < Gdk::Pixbuf > icon() const;

protected:
    std::string mPackage;
    std::string mLibrary;

    GVLE* mGVLE;

private:
    /**
     * @brief Pointer to the icon.
     */
    mutable Glib::RefPtr < Gdk::Pixbuf > m_icon;
};

typedef boost::shared_ptr < GlobalPlugin > GlobalPluginPtr;
typedef GlobalPlugin* (*GvleGlobalPluginSlot)(const std::string&,
					      const std::string&,
					      GVLE* gvle);

}} // namespace vle gvle

#endif
