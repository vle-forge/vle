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

#ifndef VLE_VPZ_STRUCTURES_HPP
#define VLE_VPZ_STRUCTURES_HPP

#include <vle/DllDefines.hpp>
#include <vle/vpz/Base.hpp>

namespace vle {
namespace vpz {

/**
 * @brief The Structure, only used by the SaxParser.
 */
class VLE_API Structures : public Base
{
public:
    /**
     * @brief Build a new Structures.
     */
    Structures() = default;

    /**
     * @brief Nothing to output.
     */
    void write(std::ostream& /* out */) const override
    {}

    /**
     * @brief Nothing to delete.
     */
    ~Structures() override = default;

    /**
     * @brief Get the type of this class.
     * @return STRUCTURES.
     */
    Base::type getType() const override
    {
        return VLE_VPZ_STRUCTURES;
    }
};
}
} // namespace vle vpz

#endif
