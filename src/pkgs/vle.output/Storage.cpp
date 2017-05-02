/*
 * @file vle/oov/plugins/Storage.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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

#include <list>
#include <map>
#include <string>
#include <vector>
#include <vle/devs/Time.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/String.hpp>

namespace vle {
namespace oov {
namespace plugin {

static std::string
buildKey(const std::string& parent,
         const std::string& simulator,
         const std::string& port)
{
    std::string result;

    result.reserve(parent.size() + simulator.size() + port.size() + 3);

    result = parent;
    result += ':';
    result += simulator;
    result += '.';
    result += port;

    return result;
}

typedef value::Matrix::index Index;

typedef std::map<std::string, Index> MapPairIndex;

enum StorageHeaderType
{
    STORAGE_HEADER_NONE, /**< No header are provided, ie. the
                          * names of the simulators are hidden  */
    STORAGE_HEADER_TOP   /**< Header (the names of the colums
                          * are stored into the first line of
                          * the matrix results. */
};

class Storage : public Plugin
{
public:
    Storage(const std::string& location)
      : Plugin(location)
      , m_matrix(nullptr)
      , m_time(devs::negativeInfinity)
      , m_headertype(STORAGE_HEADER_NONE)
    {
    }

    /**
     * We do not remove the m_matrix attribute. The @c
     * manager::Simulation can retrieve it from the @c
     * devs::RootCoordinator. Users have in charge to freed the
     * m_matrix.
     */
    virtual ~Storage()
    {
    }

    /**
     * Return a clone of the current matrix
     * The pointer is given in the finish function
     */
    virtual std::unique_ptr<value::Matrix> matrix() const override
    {
        if (m_matrix) {
            return std::unique_ptr<value::Matrix>(
              new value::Matrix(*m_matrix));
        }
        return {};
    }

    virtual std::string name() const override
    {
        return std::string("storage");
    }

    ///
    ////
    ///

    virtual void onParameter(const std::string& /*plugin*/,
                             const std::string& /*location*/,
                             const std::string& /*file*/,
                             std::unique_ptr<value::Value> parameters,
                             const double& /*time*/) override
    {
        int rzcolumns = 100, rzrows = 100;

        if (parameters and parameters->isMap()) {
            const value::Map& map = parameters->toMap();

            if (map.exist("inc_columns")) {
                rzcolumns = map.getInt("inc_columns");
            }

            if (map.exist("inc_rows")) {
                rzrows = map.getInt("inc_rows");
            }

            if (map.exist("header")) {
                std::string type = map.getString("header");
                if (type == "top") {
                    m_headertype = STORAGE_HEADER_TOP;
                }
            }

            parameters.reset();
        }
        if (m_headertype == STORAGE_HEADER_TOP) {
            m_matrix.reset(
              new value::Matrix(1, 1, rzcolumns, rzrows, rzcolumns, rzrows));
            m_matrix->add(
              0,
              0,
              std::unique_ptr<value::Value>(new vle::value::String("time")));
        } else {
            m_matrix.reset(
              new value::Matrix(1, 0, rzcolumns, rzrows, rzcolumns, rzrows));
        }
    }

    virtual void onNewObservable(const std::string& simulator,
                                 const std::string& parent,
                                 const std::string& port,
                                 const std::string& /*view*/,
                                 const double& /*time*/) override
    {
        std::string key = buildKey(parent, simulator, port);
        Index idx = m_matrix->columns();

        m_colAccess.insert(std::make_pair(key, m_matrix->columns()));

        m_matrix->addColumn();

        if (m_headertype == STORAGE_HEADER_TOP) {
            m_matrix->add(
              idx,
              0,
              std::unique_ptr<value::Value>(new vle::value::String(key)));
        }
    }

    virtual void onDelObservable(const std::string& /*simulator*/,
                                 const std::string& /*parent*/,
                                 const std::string& /*port*/,
                                 const std::string& /*view*/,
                                 const double& /*time*/) override
    {
    }

    virtual void onValue(const std::string& simulator,
                         const std::string& parent,
                         const std::string& port,
                         const std::string& /*view*/,
                         const double& time,
                         std::unique_ptr<value::Value> value) override
    {
        nextTime(time);

        if (not simulator.empty()) {
            std::string key = buildKey(parent, simulator, port);

            MapPairIndex::const_iterator it = m_colAccess.find(key);
            m_matrix->set(it->second, m_matrix->rows() - 1, std::move(value));
        }
    }

    virtual std::unique_ptr<value::Matrix> finish(
      const double& /*time*/) override
    {
        return std::move(m_matrix);
    }

private:
    std::unique_ptr<value::Matrix> m_matrix;
    MapPairIndex m_colAccess;
    double m_time;
    StorageHeaderType m_headertype;

    inline void nextTime(double trame_time)
    {
        if (trame_time != m_time) {
            m_time = trame_time;
            setLastTime();
        }
    }

    inline void setLastTime()
    {
        value::Matrix::size_type row(m_matrix->rows());
        m_matrix->addRow();
        m_matrix->add(
          0,
          row,
          std::unique_ptr<value::Value>(new vle::value::Double(m_time)));
    }
};
}
}
} // namespace vle oov plugin

DECLARE_OOV_PLUGIN(vle::oov::plugin::Storage)
