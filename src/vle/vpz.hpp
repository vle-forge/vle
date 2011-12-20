/*
 * @file vle/vpz.hpp
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


#ifndef VLE_GLOBAL_VPZ_VPZ_HPP
#define VLE_GLOBAL_VPZ_VPZ_HPP

#ifndef VLE_VPZ_ATOMICMODELS_HPP
#include <vle/vpz/AtomicModels.hpp>
#endif

#ifndef VLE_VPZ_BASE_HPP
#include <vle/vpz/Base.hpp>
#endif

#ifndef VLE_VPZ_CLASSES_HPP
#include <vle/vpz/Classes.hpp>
#endif

#ifndef VLE_VPZ_CLASS_HPP
#include <vle/vpz/Class.hpp>
#endif

#ifndef VLE_VPZ_CONDITION_HPP
#include <vle/vpz/Condition.hpp>
#endif

#ifndef VLE_VPZ_CONDITIONS_HPP
#include <vle/vpz/Conditions.hpp>
#endif

#ifndef VLE_VPZ_DLLDEFINES_HPP
#include <vle/vpz/DllDefines.hpp>
#endif

#ifndef VLE_VPZ_DYNAMIC_HPP
#include <vle/vpz/Dynamic.hpp>
#endif

#ifndef VLE_VPZ_DYNAMICS_HPP
#include <vle/vpz/Dynamics.hpp>
#endif

#ifndef VLE_VPZ_EXPERIMENT_HPP
#include <vle/vpz/Experiment.hpp>
#endif

#ifndef VLE_VPZ_MODEL_HPP
#include <vle/vpz/Model.hpp>
#endif

#ifndef VLE_VPZ_OBSERVABLE_HPP
#include <vle/vpz/Observable.hpp>
#endif

#ifndef VLE_VPZ_OBSERVABLES_HPP
#include <vle/vpz/Observables.hpp>
#endif

#ifndef VLE_VPZ_OUTPUT_HPP
#include <vle/vpz/Output.hpp>
#endif

#ifndef VLE_VPZ_OUTPUTS_HPP
#include <vle/vpz/Outputs.hpp>
#endif

#ifndef VLE_VPZ_PORT_HPP
#include <vle/vpz/Port.hpp>
#endif

#ifndef VLE_VPZ_PROJECT_HPP
#include <vle/vpz/Project.hpp>
#endif

#ifndef VLE_VPZ_SAXPARSER_HPP
#include <vle/vpz/SaxParser.hpp>
#endif

#ifndef VLE_VPZ_SAXSTACKVALUE_HPP
#include <vle/vpz/SaxStackValue.hpp>
#endif

#ifndef VLE_VPZ_SAXSTACKVPZ_HPP
#include <vle/vpz/SaxStackVpz.hpp>
#endif

#ifndef VLE_VPZ_STRUCTURES_HPP
#include <vle/vpz/Structures.hpp>
#endif

#ifndef VLE_VPZ_VIEW_HPP
#include <vle/vpz/View.hpp>
#endif

#ifndef VLE_VPZ_VIEWS_HPP
#include <vle/vpz/Views.hpp>
#endif

#ifndef VLE_VPZ_VPZ_HPP
#include <vle/vpz/Vpz.hpp>
#endif


namespace vle {

    /**
     * The VPZ library is use to parse, modifiy, write experiment file VPZ.
     *
     * The namespace vpz is use to parse and to write the VPZ (vle project file
     * zipped) XML file. vpz uses the Sax parser of the libxml-2.0 to read VPZ
     * format and Value format.
     */
    namespace vpz {

    } // namespace vpz

} // namespace vle

#endif
