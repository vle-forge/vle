/**
 * @file vle/vpz.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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


#ifndef VLE_COMMON_VPZ_VPZ_HPP
#define VLE_COMMON_VPZ_VPZ_HPP

#include <vle/vpz/AtomicModels.hpp>
#include <vle/vpz/Base.hpp>
#include <vle/vpz/Classes.hpp>
#include <vle/vpz/Class.hpp>
#include <vle/vpz/Condition.hpp>
#include <vle/vpz/Conditions.hpp>
#include <vle/vpz/Dynamic.hpp>
#include <vle/vpz/Dynamics.hpp>
#include <vle/vpz/Experiment.hpp>
#include <vle/vpz/Model.hpp>
#include <vle/vpz/Observable.hpp>
#include <vle/vpz/Observables.hpp>
#include <vle/vpz/Output.hpp>
#include <vle/vpz/Outputs.hpp>
#include <vle/vpz/Port.hpp>
#include <vle/vpz/Project.hpp>
#include <vle/vpz/Replicas.hpp>
#include <vle/vpz/SaxParser.hpp>
#include <vle/vpz/SaxStackValue.hpp>
#include <vle/vpz/SaxStackVpz.hpp>
#include <vle/vpz/Structures.hpp>
#include <vle/vpz/View.hpp>
#include <vle/vpz/Views.hpp>
#include <vle/vpz/Vpz.hpp>

namespace vle {

    /**
     * @brief The namespace vpz is use to parse and to write the VPZ (vle
     * project file zipped) XML file. vpz uses the Sax parser of the libxml-2.0
     * to read VPZ format, Value format and Trame format from a stream.
     */
    namespace vpz {

    } // namespace vpz

} // namespace vle

#endif
