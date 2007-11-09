/** 
 * @file SaxStackVpz.hpp
 * @brief 
 * @author The vle Development Team
 * @date 2007-10-12
 */

/*
 * Copyright (C) 2007 - The vle Development Team
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
        void pushTranslators();
        void pushTranslator(const AttributeList& att);
        void popTranslator(const std::string& cdata);

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
