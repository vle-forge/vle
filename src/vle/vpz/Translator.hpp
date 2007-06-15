/** 
 * @file vpz/Translator.hpp
 * @brief Translator base class to develop translator plugin.
 * @author The vle Development Team
 * @date mar, 28 fév 2006 15:01:46 +0100
 */

/*
 * Copyright (C) 2004, 05, 06 - The vle Development Team
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef VLE_VPZ_TRANSLATOR_HPP
#define VLE_VPZ_TRANSLATOR_HPP

#include <vle/vpz/Model.hpp>
#include <vle/vpz/Dynamics.hpp>
#include <vle/vpz/Conditions.hpp>
#include <vle/vpz/Views.hpp>
#include <vle/vpz/Project.hpp>

#define DECLARE_TRANSLATOR(x) \
    extern "C" { \
        vle::vpz::Translator* makeNewTranslator( \
            const vle::vpz::Project& prj) \
        { return new x(prj); } }

namespace vle { namespace vpz {

    /**
     * @brief Class to translate original XML application into VLE XML
     * application.  Translation use tag NO_VLE from VLE project XML and
     * translate it.
     */
    class Translator
    {
    public :
        Translator(const Project& prj) :
            m_project(prj)
        { }

        virtual ~Translator()
        { }

        virtual void translate(const std::string& buffer) = 0;

        const Model& model() const
        { return m_model; }

        const Dynamics& dynamics() const
        { return m_dynamics; }

        const Conditions& conditions() const
        { return m_conditions; }

        const Views& views() const
        { return m_views; }

    protected:
        const Project&  m_project;
        Model           m_model;
        Dynamics        m_dynamics;
        Conditions      m_conditions;
        Views           m_views;
    };

}} // namespace vle vpz

#endif
