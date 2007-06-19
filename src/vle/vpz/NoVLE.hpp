/** 
 * @file vpz/NoVLE.hpp
 * @brief 
 * @author The vle Development Team
 * @date ven, 17 fév 2006 11:55:42 +0100
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

#ifndef VLE_VPZ_NOVLE_HPP
#define VLE_VPZ_NOVLE_HPP

#include <vle/vpz/Base.hpp>
#include <glibmm/module.h>

namespace vle { namespace graph {

    class Model;

}} // namespace vle graph
   
   
namespace vle { namespace vpz {

    class Project;
    class Model;
    class Dynamics;
    class Conditions;
    class Views; 

    class NoVLE : public Base
    {
    public:
        NoVLE(const std::string& name) :
            m_name(name)
        { }

        virtual ~NoVLE() { }

        /** 
         * @brief Write the NoVLE information.
         * @code
         *  <NO_VLE MODEL_NAME="system" TRANSLATOR="lifegame-tr">
         *   <SYSTEM NAME="c" LANGUAGE="c++">
         *    <SIZE L="30" C="30" />
         *    <TIME_STEP VALUE="1" />
         *    <SEED VALUE="6352" />
         *   </SYSTEM>
         *  </NO_VLE>
         * @endcode
         * 
         * @param elt a xml reference to the NO_VLE node.
         *
         * @throw Exception::Internal if elt is null or not on NO_VLE node.
         */
        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return NOVLE; }

        /** 
         * @brief Dynamically open translator library and call method to get
         * Model, Dynamics and Experiment if exist.
         * 
         * @param model 
         * @param dynamics 
         * @param experiment 
         */
        void callTranslator(const Project& prj,
                            graph::Model* mdl,
                            Model& model,
                            Dynamics& dynamics,
                            Conditions& conditions,
                            Views& Views);

        /** 
         * @brief Set new information to the NoVLE.
         * @param library the library use by the novle model.
         * @param data the data to library.
         */
        void setNoVLE(const std::string& library, const std::string& data);

        /** 
         * @brief Set cdata parameters to the novle models.
         * @param cdata the content of the cdata xml element.
         */
        void setData(const std::string& cdata)
        { m_data.assign(cdata); }

        const std::string& name() const
        { return m_name; }

        const std::string& data() const
        { return m_data; }

        const std::string& library() const
        { return m_library; }

    private:
        NoVLE() 
        { }

        std::string             m_name;
        std::string             m_data;
        std::string             m_library;

        /** 
         * @brief Dynamically load the translator library.
         * 
         * @return A reference to the library or NULL.
         */
        Glib::Module* translator();
    };

}} // namespace vle vpz

#endif
