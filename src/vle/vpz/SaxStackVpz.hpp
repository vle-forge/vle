/**
 * @file src/vle/vpz/SaxStackVpz.hpp
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




#ifndef VLE_VPZ_SAXSTACKVPZ_HPP
#define VLE_VPZ_SAXSTACKVPZ_HPP

#include <vle/value/Value.hpp>
#include <vle/vpz/Trame.hpp>
#include <libxml++/libxml++.h>
#include <stack>

namespace vle { namespace vpz {

    /**
     *  Typedef the libxml++ sax AttributList
     */
    typedef xmlpp::SaxParser::AttributeList AttributeList;

    class Vpz;
    class Trame;

    class SaxStackVpz
    {
    public:
        SaxStackVpz(Vpz& vpz) :
            m_vpz(vpz)
        { }

        void clear();

        ~SaxStackVpz();

        vpz::Vpz* pushVpz(const AttributeList& name);
        void pushStructure();
        void pushModel(const AttributeList& att);
        void pushPort(const AttributeList& att);
        void pushPortType(const Glib::ustring& name);
        void pushSubModels();
        void pushConnections();
        void pushConnection(const AttributeList& att);
        void pushOrigin(const AttributeList& att);
        void pushDestination(const AttributeList& att);
        void buildConnection();
        void pushDynamics();
        void pushDynamic(const AttributeList& att);
        void pushExperiment(const AttributeList& att);
        void pushReplicas(const AttributeList& att);
        void pushConditions();
        void pushCondition(const AttributeList& att);
        void pushConditionPort(const AttributeList& att);
        void pushViews();
        void pushOutputs();
        void pushOutput(const AttributeList& att);
        void pushView(const AttributeList& att);
        void pushAttachedView(const AttributeList& att);
        void pushObservables();
        void pushObservable(const AttributeList& att);
        void pushObservablePort(const AttributeList& att);
        void pushObservablePortOnView(const AttributeList& att);
        void pushClasses();
        void pushClass(const AttributeList& att);
        void popClasses();
        void popClass();

        void pushVleTrame();
        void pushTrame(const AttributeList& att);
        void popTrame();
        void pushModelTrame(const AttributeList& att);
        void popModelTrame(const value::Value& value);
        void popVleTrame();

        value::Set& popConditionPort();
        vpz::Base* pop();
        const vpz::Base* top() const;
        vpz::Base* top();

        inline vpz::Vpz& vpz()
        { return m_vpz; }

        inline const TrameList& trame() const
        { return m_trame; }

        inline TrameList& trame()
        { return m_trame; }

        friend std::ostream& operator<<(
            std::ostream& out, const SaxStackVpz& stack);

    private:
        std::stack < vpz::Base* >       m_stack;
        vpz::Vpz&                       m_vpz;
        TrameList                       m_trame;
    };

}} // namespace vle vpz

#endif
