/** 
 * @file LocalStreamWriter.hpp
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

#ifndef VLE_DEVS_LOCALSTREAMWRITER_HPP
#define VLE_DEVS_LOCALSTREAMWRITER_HPP

#include <vle/devs/StreamWriter.hpp>
#include <vle/oov/OovLocalStreamReader.hpp>



namespace vle { namespace devs {

    /** 
     * @brief Use a local communication to observe. LocalStreamWriter are just
     * a reference to the oov::LocalStreamReader.
     */
    class LocalStreamWriter : public StreamWriter
    {
    public:
        LocalStreamWriter();

        virtual ~LocalStreamWriter();

        virtual void open(const std::string& plugin,
                          const std::string& location,
                          const std::string& file,
                          const std::string& parameters,
                          const devs::Time& time);

        virtual void processNewObservable(Simulator* simulator,
                                          const std::string& portname,
                                          const devs::Time& time,
                                          const std::string& view);

        virtual void processRemoveObservable(Simulator* simulator,
                                             const std::string& portname,
                                             const devs::Time& time,
                                             const std::string& view);

        virtual void process(const StateEvent& event);

        virtual void close(const devs::Time& time);

    private:
        oov::OovLocalStreamReader      m_reader;
    };

}} // namespace vle devs

#endif
