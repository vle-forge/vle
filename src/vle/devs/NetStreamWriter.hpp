/**
 * @file vle/devs/NetStreamWriter.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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

#include <vle/devs/DllDefines.hpp>
#include <vle/devs/StreamWriter.hpp>
#include <vle/utils/Socket.hpp>

namespace vle { namespace devs {

    class VLE_DEVS_EXPORT NetStreamWriter : public StreamWriter
    {
    public:
        NetStreamWriter();

        virtual ~NetStreamWriter();

        virtual void open(const std::string& plugin,
                          const std::string& location,
                          const std::string& file,
                          value::Value* parameters,
                          const devs::Time& time);

        virtual void processNewObservable(Simulator* simulator,
                                          const std::string& portname,
                                          const devs::Time& time,
                                          const std::string& view);

        virtual void processRemoveObservable(Simulator* simulator,
                                             const std::string& portname,
                                             const devs::Time& time,
                                             const std::string& view);

        virtual void process(ObservationEvent& event);

        virtual oov::PluginPtr close(const devs::Time& time);

        virtual oov::PluginPtr refreshPlugin();

    private:
        oov::PluginPtr getPlugin() const;

        value::Set*      m_paramFrame;
        value::Set*      m_newObsFrame;
        value::Set*      m_delObsFrame;
        value::Set*      m_valueFrame;
        value::Set*      m_closeFrame;
        value::Set*      m_refreshFrame;

        void buildParameters(const std::string& plugin,
                             const std::string& location,
                             const std::string& directory,
                             value::Value* file,
                             const double& time);

        void buildNewObs(const std::string& simulator,
                         const std::string& parents,
                         const std::string& port,
                         const std::string& view,
                         const double& time);

        void buildDelObs(const std::string& simulator,
                         const std::string& parents,
                         const std::string& port,
                         const std::string& view,
                         const double& time);

        void buildValue(const std::string& simulator,
                        const std::string& parents,
                        const std::string& port,
                        const std::string& view,
                        const double& time,
                        value::Value* value);

        void buildClose(const double& time);

        void buildRefresh();

        utils::net::Client*     m_client;
    };

}} // namespace vle devs

#endif
