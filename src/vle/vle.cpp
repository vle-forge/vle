/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
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


#include <vle/vle.hpp>
#include <libxml/parser.h>

namespace vle {

/* From libxml2 website: http://xmlsoft.org/threads.html
 *
 * Starting with 2.4.7, libxml2 makes provisions to ensure that concurrent
 * threads can safely work in parallel parsing different documents. There
 * is however a couple of things to do to ensure it:
 *
 * - configure the library accordingly using the --with-threads options
 * - call xmlInitParser() in the "main" thread before using any of the
 *   libxml2 API (except possibly selecting a different memory allocator)
 */

Init::Init()
{
    xmlInitParser();
}

Init::~Init()
{
    xmlCleanupParser();
}

} // namespace vle
