/**
 * @file src/vle/vpz/Project.hpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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



#ifndef VLE_VPZ_PROJECT_HPP
#define VLE_VPZ_PROJECT_HPP

#include <vle/vpz/Base.hpp>
#include <vle/vpz/Model.hpp>
#include <vle/vpz/Dynamics.hpp>
#include <vle/vpz/Experiment.hpp>
#include <vle/vpz/Classes.hpp>

namespace vle { namespace vpz {

    class Project : public Base
    {
    public:
        Project() :
            m_version(0.5),
            m_instance(-1),
            m_replica(-1)
        { }

        virtual ~Project() { }

        void setProject(const std::string& name, const std::string& date);

        virtual void write(std::ostream& out ) const;

        virtual Base::type getType() const
        { return PROJECT; }

        void clear();

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
          * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
        
        const Model& model() const { return m_model; }
        
        Model& model() { return m_model; }
        
        const Dynamics& dynamics() const { return m_dynamics; }
        
        Dynamics& dynamics() { return m_dynamics; }
        
        const Experiment& experiment() const { return m_experiment; }
        
        Experiment& experiment() { return m_experiment; }
        
        const Classes& classes() const { return m_classes; }
        
        Classes& classes() { return m_classes; }

        void setAuthor(const std::string& name);

        inline const std::string& author() const
        { return m_author; }

        void setDate(const std::string& date);

        inline float version() const
        { return m_version; }

        inline int instance() const
        { return m_instance; }

        inline void setInstance(int instance)
        { m_instance = instance; }

        inline void setReplica(int replica)
        { m_replica = replica; }

        inline int replica() const
        { return m_replica; }

        void setCurrentDate();

        inline void setVersion(float f)
        { if (f >= 0) m_version = f; }

        inline const std::string& date() const
        { return m_date; }

    private:
        std::string     m_author;
        std::string     m_date;
        float           m_version;
        int             m_instance;
        int             m_replica;

        Model           m_model;
        Dynamics        m_dynamics;
        Experiment      m_experiment;
        Classes         m_classes;
    };

}} // namespace vle vpz

#endif
