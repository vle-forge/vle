/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
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
    In() = default;

    /**
     * @brief Nothing to delete.
     */
    ~In() override = default;

    /**
     * @brief Nothing to output.
     */
    void write(std::ostream& /* out */) const override
    {}

    /**
     * @brief Get the type of this class.
     * @return IN.
     */
    Base::type getType() const override
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
    Out() = default;

    /**
     * @brief Nothing to delete.
     */
    ~Out() override = default;

    /**
     * @brief Nothing to output.
     */
    void write(std::ostream& /* out */) const override
    {}

    /**
     * @brief Get the type of this class.
     * @return OUT.
     */
    Base::type getType() const override
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
    Init() = default;

    /**
     * @brief Nothing to delete.
     */
    ~Init() override = default;

    /**
     * @brief Nothing to output.
     */
    void write(std::ostream& /* out */) const override
    {}

    /**
     * @brief Get the type of this class.
     * @return INIT.
     */
    Base::type getType() const override
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
    State() = default;

    /**
     * @brief Nothing to delete.
     */
    ~State() override = default;

    /**
     * @brief Nothing to output.
     */
    void write(std::ostream& /* out */) const override
    {}

    /**
     * @brief Get the type of this class.
     * @return STATE.
     */
    Base::type getType() const override
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
    Port() = default;

    /**
     * @brief Nothing to delete.
     */
    ~Port() override = default;

    /**
     * @brief Nothing to output.
     */
    void write(std::ostream& /* out */) const override
    {}

    /**
     * @brief Get the type of this class.
     * @return PORT.
     */
    Base::type getType() const override
    {
        return VLE_VPZ_PORT;
    }
};
}
} // namespace vle vlz

#endif
