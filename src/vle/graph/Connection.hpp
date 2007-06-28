/**
 * @file graph/Connection.hpp
 * @author The VLE Development Team.
 * @brief Represent a DEVS connection between two models: internal, input or
 * output. This class is member of the DEVS Graph.
 */

/*
 * Copyright (C) 2005, 05, 06 - The vle Development Team
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

#ifndef VLE_GRAPH_CONNECTION_HPP
#define VLE_GRAPH_CONNECTION_HPP

#include <vector>



namespace vle { namespace graph {

    class Model;
    class Port;
    class Connection;
    
    typedef std::vector < Connection* > VectorConnection;

    /**
     * @brief Represent a DEVS connection between two models: internal, input
     * or output. This class is member of the DEVS Graph.
     *
     */
    class Connection
    {
    public:
        Connection();

        Connection(Model* originModel,
                   Port* originPort,
                   Model* destinationModel,
                   Port* destinationPort);

	~Connection()
        { }

        inline void setOriginModel(Model* mdl)
        { m_originModel = mdl; }

        inline void setOriginPort(Port* port)
        { m_originPort = port; }

        inline void setDestinationModel(Model* mdl)
        { m_destinationModel = mdl; }

        inline void setDestinationPort(Port* port)
        { m_destinationPort = port; }



        inline Model* getOriginModel() const
        { return m_originModel; }

        inline Port* getOriginPort() const
        { return m_originPort; }

        inline Model* getDestinationModel() const
        { return m_destinationModel; }

        inline Port* getDestinationPort() const
        { return m_destinationPort; }

    private:
        Model*  m_originModel;
        Port*   m_originPort;
        Model*  m_destinationModel;
        Port*   m_destinationPort;
    };

}} // namespace vle graph

#endif
