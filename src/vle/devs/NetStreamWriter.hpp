/**
 * @file src/vle/devs/NetStreamWriter.hpp
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




#ifndef VLE_DEVS_NETSTREAMWRITER_HPP
#define VLE_DEVS_NETSTREAMWRITER_HPP

#include <vle/devs/StreamWriter.hpp>
#include <vle/utils/Socket.hpp>



namespace vle { namespace devs {

    class NetStreamWriter : public StreamWriter
    {
    public:
        NetStreamWriter();

        virtual ~NetStreamWriter();

        virtual void open(
                    const std::string& plugin,
                    const std::string& location,
                    const std::string& file,
                    const std::string& parameters,
                    const devs::Time& time);

        virtual void processNewObservable(
                    Simulator* simulator,
                    const std::string& portname,
                    const devs::Time& time,
                    const std::string& view);

        virtual void processRemoveObservable(
                    Simulator* simulator,
                    const std::string& portname,
                    const devs::Time& time,
                    const std::string& view);

        virtual void process(
                    const ObservationEvent& event);

        virtual oov::PluginPtr close(
                    const devs::Time& time);

    private:
        oov::PluginPtr getPlugin() const;

        utils::net::Client*     m_client;
    };

}} // namespace vle devs

#endif
