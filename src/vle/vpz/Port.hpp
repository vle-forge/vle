/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#ifndef VLE_VPZ_PORT_HPP
#define VLE_VPZ_PORT_HPP

#include <vle/DllDefines.hpp>
#include <vle/vpz/Base.hpp>

namespace vle {
namespace vpz {

/**
 * @brief The In port, only used by the SaxParser.
 */
class VLE_API In : public Base
{
public:
    /**
     * @brief Build a new In.
     */
    In()
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~In()
    {
    }

    /**
     * @brief Nothing to output.
     */
    virtual void write(std::ostream& /* out */) const override
    {
    }

    /**
     * @brief Get the type of this class.
     * @return IN.
     */
    virtual Base::type getType() const override
    {
        return VLE_VPZ_IN;
    }
};

/**
 * @brief The Out port, only used by the SaxParser.
 */
class VLE_API Out : public Base
{
public:
    /**
     * @brief Build a new Out.
     */
    Out()
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~Out()
    {
    }

    /**
     * @brief Nothing to output.
     */
    virtual void write(std::ostream& /* out */) const override
    {
    }

    /**
     * @brief Get the type of this class.
     * @return OUT.
     */
    virtual Base::type getType() const override
    {
        return VLE_VPZ_OUT;
    }
};

/**
 * @brief The Init port, only used by the SaxParser.
 */
class VLE_API Init : public Base
{
public:
    /**
     * @brief Build a new Init.
     */
    Init()
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~Init()
    {
    }

    /**
     * @brief Nothing to output.
     */
    virtual void write(std::ostream& /* out */) const override
    {
    }

    /**
     * @brief Get the type of this class.
     * @return INIT.
     */
    virtual Base::type getType() const override
    {
        return VLE_VPZ_INIT;
    }
};

/**
 * @brief The State port, only used by the SaxParser.
 */
class VLE_API State : public Base
{
public:
    /**
     * @brief Build a new State.
     */
    State()
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~State()
    {
    }

    /**
     * @brief Nothing to output.
     */
    virtual void write(std::ostream& /* out */) const override
    {
    }

    /**
     * @brief Get the type of this class.
     * @return STATE.
     */
    virtual Base::type getType() const override
    {
        return VLE_VPZ_STATE;
    }
};

/**
 * @brief The Port, only used by the SaxParser.
 */
class VLE_API Port : public Base
{
public:
    /**
     * @brief Build a new Port.
     */
    Port()
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~Port()
    {
    }

    /**
     * @brief Nothing to output.
     */
    virtual void write(std::ostream& /* out */) const override
    {
    }

    /**
     * @brief Get the type of this class.
     * @return PORT.
     */
    virtual Base::type getType() const override
    {
        return VLE_VPZ_PORT;
    }
};
}
} // namespace vle vlz

#endif
