/** 
 * @file Default.hpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 24 jui 2007 18:20:30 +0200
 */

/*
 * Copyright (C) 2007 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef VLE_OOV_PLUGINS_DEFAULT_HPP
#define VLE_OOV_PLUGINS_DEFAULT_HPP

#include <vle/oov/Plugin.hpp>
#include <fstream>



namespace vle { namespace oov { namespace plugin {

    class Default : public Plugin
    {
    public:
        Default(const std::string& location);

        virtual ~Default();

        virtual void onParameter(const vpz::ParameterTrame& trame);

        virtual void onNewObservable(const vpz::NewObservableTrame& trame);

        virtual void onDelObservable(const vpz::DelObservableTrame& trame);

        virtual void onValue(const vpz::ValueTrame& trame);

        virtual void close();

    private:
        std::ofstream                   m_file;
    };


    DECLARE_OOV_PLUGIN(vle::oov::plugin::Default);

}}} // namespace vle oov plugin

#endif

