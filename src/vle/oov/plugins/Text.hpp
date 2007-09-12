/** 
 * @file Text.hpp
 * @brief 
 * @author The vle Development Team
 * @date 2007-07-15
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

#ifndef VLE_OOV_PLUGINS_TEXT_HPP
#define VLE_OOV_PLUGINS_TEXT_HPP

#include <vle/oov/Plugin.hpp>
#include <fstream>
#include <map>
#include <vector>



namespace vle { namespace oov { namespace plugin {

    class Text : public Plugin
    {
    public:
        Text(const std::string& location);

        virtual ~Text();

        virtual void onParameter(const vpz::ParameterTrame& trame);

        virtual void onNewObservable(const vpz::NewObservableTrame& trame);

        virtual void onDelObservable(const vpz::DelObservableTrame& trame);

        virtual void onValue(const vpz::ValueTrame& trame);

        virtual void close(const vpz::EndTrame& trame);

    private:
        std::map < std::string, int >   m_columns;
        std::vector < value::Value >    m_buffer;
        double                          m_time;
        std::ofstream                   m_file;
        bool                            m_isstart;


        void flush(double trame_time);

        void finalFlush(double trame_time);

        /**
         * @brief This function is use to build uniq name to each row of the
         * text output.
         * @param simulator the name of the devs::Model.
         * @param port the name of the state port of the devs::Model.
         * @return a representation of the uniq name.
         */
        inline std::string buildname(const std::string& simulator,
                                     const std::string& port);
    };

    std::string Text::buildname(const std::string& simulator,
                                const std::string& port)
    {
        std::string result(simulator);
        result += '_';
        result += port;
        return result;
    }


    DECLARE_OOV_PLUGIN(vle::oov::plugin::Text);

}}} // namespace vle oov plugin

#endif
