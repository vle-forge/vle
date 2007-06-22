/**
 * @file   MatrixTranslator.hpp
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

#ifndef TRANSLATOR_MATRIXTRANSLATOR_HPP
#define TRANSLATOR_MATRIXTRANSLATOR_HPP

#include <vle/vpz/Translator.hpp>
#include <string>
#include <vector>

namespace vle { namespace translator {
  
  class MatrixTranslator : public vle::vpz::Translator
  {
  public:
    MatrixTranslator(const vle::vpz::Project& prj);
    ~MatrixTranslator();
    
    virtual void translate(const std::string& buffer);
    
  protected:
    virtual void translateStructures();
    virtual void translateDynamics();
    virtual void translateTranslators() { }
    virtual void translateConditions();
    virtual void translateViews() { }
  };
  
}}

#endif
