/**
 * @file vle/oov/plugins/Default.hpp
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

        virtual void close(const vpz::EndTrame& trame);

    private:
        std::ofstream                   m_file;
    };


    DECLARE_OOV_PLUGIN(vle::oov::plugin::Default);

}}} // namespace vle oov plugin

#endif

