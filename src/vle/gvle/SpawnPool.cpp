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
#include <vle/gvle/SpawnPool.hpp>
#include <glibmm/main.h>

namespace vle { namespace gvle {

SpawnPool::SpawnPool()
    : mSpawnList()
{
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &SpawnPool::spawnTimer),
                                   1000);
}

utils::Spawn& SpawnPool::addSpawn()
{
    SpawnPtr ptr(new utils::Spawn());

    mSpawnList.push_back(ptr);

    return *(mSpawnList.back().get());
}

bool SpawnPool::spawnTimer()
{
    SpawnList::iterator it;

    it = mSpawnList.begin();
    while (it != mSpawnList.end()) {
        if ((*it)->isfinish()) {
            it = mSpawnList.erase(it);
        } else {
            ++it;
        }
    }
    return true;
}

}} // namespace vle gvle
