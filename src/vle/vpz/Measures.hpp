/** 
 * @file vpz/Measures.hpp
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

#ifndef VLE_VPZ_MEASURES_HPP
#define VLE_VPZ_MEASURES_HPP

#include <list>
#include <vector>
#include <vle/vpz/Outputs.hpp>
#include <vle/vpz/Measure.hpp>
#include <vle/vpz/EOV.hpp>

namespace vle { namespace vpz {


    class MeasureList : public std::map < std::string, Measure >
    {
    public:
        bool exist(const std::string& name) const;
    };



    class Measures : public Base
    {
    public:
        Measures();

        virtual ~Measures() { }

        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return MEASURES; }

        Output& addTextStreamOutput(const std::string& name,
				    const std::string& location = std::string());

        Output& addSdmlStreamOutput(const std::string& name,
				    const std::string& location = std::string());

        Output& addEovStreamOutput(const std::string& name,
				   const std::string& plugin,
				   const std::string& location = std::string());

        void clear();

        void delOutput(const std::string& name);

        Measure& addEventMeasure(const std::string& name,
                const std::string& output);

        Measure& addTimedMeasure(const std::string& name,
                double timestep,
                const std::string& output);

        void delMeasure(const std::string& name);

        void addObservableToMeasure(const std::string& measurename,
                                    const std::string& name,
                                    const std::string& group = std::string(),
                                    int index = -1);

        void addMeasures(const Measures& m);

        const Measure& findMeasureFromOutput(const std::string& outputname,
                                             std::string& measure) const;

        const Measure& find(const std::string& name) const;
        
        Measure& find(const std::string& name);
        
        bool exist(const std::string& name) const;

        const Outputs& outputs() const
        { return m_outputs; }

        Outputs& outputs()
        { return m_outputs; }

        const MeasureList& measures() const
        { return m_measures; }

        const EOVs& eovs() const
        { return m_eovs; }

        EOVs& eovs()
        { return m_eovs; }

    private:
        Outputs     m_outputs;
        EOVs        m_eovs;
        MeasureList m_measures;
        
        /** 
         * @brief Add a measure into the measures list.
         * 
         * @param m the measure to add
         *
         * @throw Exception::Internal if the measure already exist or if no
         * output was define in outputs lists.
         *
         * @return A reference to the stored object.
         */
        Measure& addMeasure(const Measure& m);

        /** 
         * @brief Build EOV from EOV output when no EOV was defined.
         */
        void buildEOV();
    };

}} // namespace vle vpz

#endif
