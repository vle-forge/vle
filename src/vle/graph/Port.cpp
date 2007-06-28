/**
 * @file graph/Port.cpp
 * @author The VLE Development Team.
 * @brief Represent a DEVS port for DEVS model. This class is member of the
 * DEVS graph.
 */

/*
 * Copyright (C) 2004, 05, 06 - The vle Development Team
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

#include <vle/graph/Port.hpp>
#include <vle/graph/Model.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/value/Value.hpp>



namespace vle { namespace graph {

Port::Port(Model* model, const std::string& name) :
    m_model(model),
    m_name(name),
    m_connection(0)
{
    Assert(utils::InternalError, model, boost::format(
            "Bad model address while building port %1%\n") % name);
}

const std::string& Port::getModelName() const
{
    return m_model->getName();
}

void Port::addConnection(Model* model, Port* port)
{
    Assert(utils::InternalError, model and port, boost::format(
            "Bad model or port adresse while building connection on port "
            "%1%\n") % m_name); 

    if (m_connection == 0) {
        m_connection = new VectorConnection;
    }

    Connection* cnt = new Connection(m_model, this, model, port);
    m_connection->push_back(cnt);
}

bool Port::existConnection() const
{
    if (m_connection == 0 or m_connection->size() == 0)
        return false;

    return true;
}

void Port::delConnection(Model* model, Port* port)
{
    Assert(utils::InternalError, m_connection, boost::format(
            "Remove a connection without existence in port %1%\n") % m_name);
    Assert(utils::InternalError, model and port, boost::format(
            "Bad model or port adresse while building connection on port "
            "%1%\n") % m_name);

    VectorConnection::iterator previous = m_connection->begin();
    for (VectorConnection::iterator it = m_connection->begin();
         it != m_connection->end(); ++it) {

        if ((*it)->getDestinationModel() == model and
            (*it)->getDestinationPort() == port) {
            delete (*it);
            m_connection->erase(it);
            it = previous;
        } else {
            previous = it;
        }
    }
}

void Port::delConnection(Model* model)
{
    Assert(utils::InternalError, m_connection, boost::format(
            "Remove a connection without existence in port %1%\n") % m_name);
    Assert(utils::InternalError, model, boost::format(
            "Bad model adresse while building connection on port "
            "%1%\n") % m_name);

    VectorConnection::iterator previous = m_connection->begin();
    for (VectorConnection::iterator it = m_connection->begin();
         it != m_connection->end(); ++it) {

        if ((*it)->getDestinationModel() == model) {
            delete (*it);
            m_connection->erase(it);
            it = previous;
        } else {
            previous = it;
        }
    }
}

void Port::delConnections()
{
    if (m_connection) {
        for (VectorConnection::iterator it = m_connection->begin(); 
             it != m_connection->end(); ++it) {
            delete *it;
        }
        m_connection->clear();
        delete m_connection;
        m_connection = 0;
    }
}

}} // namespace vle graph
