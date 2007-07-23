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
#include <vle/vpz/Observables.hpp>
#include <vle/vpz/View.hpp>

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

        Output& addLocalStreamOutput(const std::string& name,
                                     const std::string& location,
                                     const std::string& plugin);

        Output& addDistantStreamOutput(const std::string& name,
                                       const std::string& location,
                                       const std::string& plugin);

        void delOutput(const std::string& name);
        
        inline const Outputs& outputs() const
        { return m_outputs; }

        inline Outputs& outputs()
        { return m_outputs; }

        //
        ///
        //// Manage observables objects.
        ///
        //

        Observable& addObservable(const Observable& obs);

        Observable& addObservable(const std::string& name);

        void delObservable(const std::string& name);

        inline const Observables& observables() const
        { return m_observables; }

        inline Observables& observables()
        { return m_observables; }

        //
        ///
        //// Manage views objects.
        ///
        //
        
        void add(const Views& views);

        View& add(const View& view);

        void clear();

        View& addEventView(const std::string& name,
                           const std::string& output,
                           const std::string& library);

        View& addTimedView(const std::string& name,
                           double timestep,
                           const std::string& output,
                           const std::string& library);

        void del(const std::string& name);

        const View& get(const std::string& name) const;
        
        View& get(const std::string& name);
        
        inline bool exist(const std::string& name) const
        { return find(name) != end(); }

    private:
        Outputs         m_outputs;
        Observables     m_observables;
    };

}} // namespace vle vpz

#endif
