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
#include <vle/vpz/Model.hpp>
#include <vle/vpz/Dynamics.hpp>
#include <vle/vpz/Experiment.hpp>
#include <glibmm/module.h>

namespace vle { namespace vpz {

    class NoVLE : public Base
    {
    public:
        NoVLE()
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
        void callTranslator(Model& model,
                            Dynamics& dynamics,
                            Experiment& experiment);

        /** 
         * @brief Set new information to the NoVLE.
         * 
         * @param translator the library name.
         * @param xml the content to initialise NoVLE.
         *
         * @throw Exception::Internal if xml or translator if empty.
         */
        void setNoVLE(const std::string& translator, const Glib::ustring& xml);

        const Glib::ustring& xml() const
        { return m_xml; }

        const std::string& translatorname() const
        { return m_translator; }

    private:
        Glib::ustring           m_xml;
        std::string             m_translator;
        
        /** 
         * @brief Dynamically load the translator library.
         * 
         * @return A reference to the library or NULL.
         */
        Glib::Module* translator();
    };
    
}} // namespace vle vpz

#endif
