/*
 * @file vle/devs/LocalStreamWriter.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#ifndef VLE_DEVS_LOCALSTREAMWRITER_HPP
#define VLE_DEVS_LOCALSTREAMWRITER_HPP

#include <vle/DllDefines.hpp>
#include <vle/devs/StreamWriter.hpp>
#include <vle/oov/LocalStreamReader.hpp>

namespace vle { namespace devs {

    /**
     * @brief Use a local communication to observe. LocalStreamWriter are just
     * a reference to the oov::LocalStreamReader.
     */
    class VLE_API LocalStreamWriter : public StreamWriter
    {
    public:
        LocalStreamWriter(const utils::ModuleManager& modulemgr)
            : StreamWriter(modulemgr), m_reader(modulemgr)
        {}

        virtual ~LocalStreamWriter()
        {}

        virtual void open(const std::string& plugin,
                          const std::string& package,
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

        virtual void process(Simulator* simulator,
                             const std::string& portname,
                             const devs::Time& time,
                             const std::string& view,
                             value::Value* value);

        virtual oov::PluginPtr close(const devs::Time& time);

        virtual oov::PluginPtr refreshPlugin();

    private:
        LocalStreamWriter(const LocalStreamWriter& other);
        LocalStreamWriter& operator=(const LocalStreamWriter& other);

        oov::LocalStreamReader      m_reader;
    };

}} // namespace vle devs

#endif
