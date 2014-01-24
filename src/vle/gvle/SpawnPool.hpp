/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
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
#ifndef VLE_GVLE_SPAWNPOOL_HPP
#define VLE_GVLE_SPAWNPOOL_HPP

#include <vle/gvle/DllDefines.hpp>
#include <vle/utils/Spawn.hpp>
#include <boost/shared_ptr.hpp>
#include <list>

#include <sigc++/sigc++.h>

namespace vle { namespace gvle {

typedef boost::shared_ptr < utils::Spawn > SpawnPtr;
typedef std::list< SpawnPtr > SpawnList;
/**
 * @brief A class to manage spawns objects.
 */
class GVLE_API SpawnPool
{
public:
    SpawnPool();

    ~SpawnPool() {}

    utils::Spawn& addSpawn();

    bool spawnTimer();

private:
    SpawnList mSpawnList;
};

}} // namespace vle gvle

#endif
