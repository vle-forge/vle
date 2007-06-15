/** 
 * @file vpz/Views.hpp
 * @brief 
 * @author The vle Development Team
 * @date lun, 13 fév 2006 18:31:21 +0100
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

#ifndef VLE_VPZ_VIEWS_HPP
#define VLE_VPZ_VIEWS_HPP

#include <list>
#include <vector>
#include <vle/vpz/Outputs.hpp>
#include <vle/vpz/Views.hpp>

namespace vle { namespace vpz {


    class Views : public Base, public std::map < std::string, View >
    {
    public:
        Views()
        { }

        virtual ~Views()
        { }

        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return VIEWS; }

        //
        ///
        //// Manage outputs objets.
        ///
        //

        Output& addTextStreamOutput(const std::string& name,
                                    const std::string& location,
                                    const std::string& data);

        Output& addSdmlStreamOutput(const std::string& name,
                                    const std::string& location,
                                    const std::string& data);

        Output& addEovStreamOutput(const std::string& name,
                                   const std::string& plugin,
                                   const std::string& location,
                                   const std::string& data);
        
        void delOutput(const std::string& name);
        
        inline const Outputs& outputs() const
        { return m_outputs; }

        inline Outputs& outputs()
        { return m_outputs; }

        //
        ///
        //// Manages views objets.
        ///
        //
        
        void add(const Views& views);

        View& add(const View& view);

        void clear();

        View& addEventView(const std::string& name,
                           const std::string& output,
                           const std::string& library,
                           const std::string& data);

        View& addTimedView(const std::string& name,
                           double timestep,
                           const std::string& output,
                           const std::string& library,
                           const std::string& data);

        void del(const std::string& name);

        const View& get(const std::string& name) const;
        
        View& get(const std::string& name);
        
        inline bool exist(const std::string& name) const
        { return find(name) != end(); }

    private:
        Outputs     m_outputs;
    };

}} // namespace vle vpz

#endif
