/** 
 * @file Project.hpp
 * @brief 
 * @author The vle Development Team
 * @date lun, 06 fév 2006 15:14:31 +0100
 */
/*
 * Copyright (C) 2006 - The vle Development Team
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

#ifndef VLE_VPZ_PROJECT_HPP
#define VLE_VPZ_PROJECT_HPP

#include <vle/vpz/Base.hpp>
#include <vle/vpz/Model.hpp>
#include <vle/vpz/Dynamics.hpp>
#include <vle/vpz/Graphics.hpp>
#include <vle/vpz/Experiment.hpp>
#include <vle/vpz/Classes.hpp>
#include <vle/vpz/NoVLEs.hpp>

namespace vle { namespace vpz {

    class Project : public Base
    {
    public:
        Project() { }

        virtual ~Project() { }

        void setProject(const std::string& name, const std::string& date);

        virtual void init(xmlpp::Element* elt);

        virtual void write(xmlpp::Element* elt) const;

        void clear();

        void expandTranslator();

        bool hasNoVLE() const;

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
          * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
        
        const Model& model() const { return m_model; }
        
        Model& model() { return m_model; }
        
        const Dynamics& dynamics() const { return m_dynamics; }
        
        Dynamics& dynamics() { return m_dynamics; }
        
        const Graphics& graphics() const { return m_graphics; }
        
        Graphics& graphics() { return m_graphics; }
        
        const Experiment& experiment() const { return m_experiment; }
        
        Experiment& experiment() { return m_experiment; }
        
        const Classes& classes() const { return m_classes; }
        
        Classes& classes() { return m_classes; }

        const NoVLEs& novles() const { return m_novles; }

        NoVLEs& novles() { return m_novles; }

        void setName(const std::string& name);

        inline const std::string& name() const
        { return m_name; }

        void setDate(const std::string& date);

        void setCurrentDate();

        inline const std::string& date() const
        { return m_date; }

    private:
        std::string     m_name;
        std::string     m_date;
        Model           m_model;
        Dynamics        m_dynamics;
        Graphics        m_graphics;
        Experiment      m_experiment;
        Classes         m_classes;
        NoVLEs          m_novles;
    };

}} // namespace vle vpz

#endif
