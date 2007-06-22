/**
 * @file   MatrixTranslator.cpp
 * @author The VLE Development Team.
 * @date   jeudi 15 avril 2004
 *
 * @brief	 Virtual Laboratory Environment - VLE Project
 * Copyright (C) 2004 LIL Laboratoire d'Informatique du Littoral
 *
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <MatrixTranslator.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Rand.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <boost/format.hpp>

using namespace vle;

namespace vle { namespace translator {

MatrixTranslator::MatrixTranslator(const vle::vpz::Project& prj) :
    Translator(prj)
{
}

MatrixTranslator::~MatrixTranslator()
{
}

    void MatrixTranslator::translate(const std::string& /*buffer*/)
{
  // XML parsing

  // translate
  translateStructures();
  translateDynamics();
  translateTranslators();
  translateConditions();
  translateViews();
}

void MatrixTranslator::translateStructures()
{
}
    
void MatrixTranslator::translateDynamics()
{
}

void MatrixTranslator::translateConditions()
{
}
    
}}
