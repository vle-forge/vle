/** 
 * @file Text.hpp
 * @brief 
 * @author The vle Development Team
 * @date 2007-07-15
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

#ifndef VLE_OOV_PLUGINS_RPLUG_HPP
#define VLE_OOV_PLUGINS_RPLUG_HPP

#include <boost/multi_array.hpp>
#include <vle/oov/Plugin.hpp>
#include <fstream>
#include <map>
#include <vector>


namespace vle { namespace oov { namespace plugin {

    class Storage : public Plugin
    {
    public:
        typedef boost::multi_array<value::Value, 2> ArrayValue;
        typedef ArrayValue::array_view<1>::type ArrayView;
        typedef std::vector < std::string > VectorString;

        Storage(const std::string& location);

        virtual ~Storage();

        virtual void onParameter(const vpz::ParameterTrame& trame);

        virtual void onNewObservable(const vpz::NewObservableTrame& trame);

        virtual void onDelObservable(const vpz::DelObservableTrame& trame);

        virtual void onValue(const vpz::ValueTrame& trame);

        virtual void close(const vpz::EndTrame& trame);


        const VectorString& getModelList() const;

        const VectorString& getPortList(const std::string& model) const;

        Storage::ArrayView getTime();

        Storage::ArrayView getValue(const std::string& model,
                                    const std::string& ports);

        const std::vector < value::Value >::size_type getVectorSize() const;

    private:
        typedef std::pair < std::string, std::string > PairString;

        struct pairCmp {
            bool operator()(const PairString& ps1, const PairString& ps2 ) const
            { return (ps1.first + ps1.second < ps2.first + ps2.second); }
        };

        typedef ArrayValue::index_range Range;
        typedef ArrayValue::extent_gen Extents;

        typedef std::map < std::string, VectorString > MapVectorString;

        typedef  std::map < PairString, ArrayValue::index, pairCmp > MapPairIndex;

        // to store the values
        ArrayValue m_values;
        ArrayValue::index nb_col;
        ArrayValue::index nb_row;

        // to extend step by step the storage
        ArrayValue::extent_gen extents;

        // to store for each model names of map
        MapVectorString m_info;

        // to store each model names
        VectorString model_names;

        // to provide a direct access to a column
        MapPairIndex col_access;

        double                          m_time;
        bool                            m_isstart;    

        void nextTime(double trame_time);
    };

    DECLARE_OOV_PLUGIN(vle::oov::plugin::Storage);

}}} // namespace vle oov plugin

#endif
