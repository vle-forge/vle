/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/translator/MatrixTranslator.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <boost/cast.hpp>

namespace vle { namespace translator {

MatrixTranslator::~MatrixTranslator()
{
    if (m_init) {
        delete[] m_init;
    }
}

const vpz::AtomicModel*
MatrixTranslator::getModel(const std::string& name) const
{
    std::map < std::string, const vpz::AtomicModel* >::const_iterator it;
    it = m_models.find(name);

    if (it == m_models.end()) {
        throw utils::InternalError(fmt(
                _("MatrixTranslator: unknow model '%1%'")) % name);
    }

    return it->second;
}

unsigned int MatrixTranslator::getSize(unsigned int i) const
{
    std::map < unsigned int, unsigned int >::const_iterator it(
        m_size.find(i));

    if (it == m_size.end()) {
        throw utils::InternalError(fmt(
                _("MatrixTranslator: unknow size '%1%'")) % i);
    }

    return it->second;
}

bool MatrixTranslator::existModel(unsigned int i, unsigned int j)
{
    if (m_dimension == 0) {
        return true;
    } else {
        if (m_dimension == 1) {
            return (!m_init or m_init[i + (j - 1) * m_size[0] - 1] != 0);
        } else {
            return false;
        }
    }
}

std::string MatrixTranslator::getDynamics(unsigned int i, unsigned int j)
{
    if (m_multiple) {
        if (m_dimension == 0) {
            return (fmt("dyn_cell_%1%") % m_init[i]).str();
        } else {
            if (m_dimension == 1) {
                return (fmt("dyn_cell_%1%")
                        % (m_init[i + (j - 1) * m_size[0] - 1])).str();
            } else {
                return std::string();
            }
        }
    } else {
        return "cell";
    }
}

std::string MatrixTranslator::getClass(unsigned int i, unsigned int j)
{
    if (m_multiple) {
        if (m_dimension == 0) {
            return m_classes[m_init[i]];
        } else {
            if (m_dimension == 1) {
                return m_classes[m_init[i + (j - 1) * m_size[0] - 1]];
            } else {
                return std::string();
            }
        }
    } else {
        return m_class;
    }
}

std::string MatrixTranslator::getName(unsigned int i, unsigned int j) const
{
    if (m_dimension == 0) {
        return (fmt("%1%_%2%") % m_prefix % i).str();
    } else {
        if (m_dimension == 1) {
            return (fmt("%1%_%2%_%3%") % m_prefix % i % j).str();
        }
    }

    return std::string();
}

void MatrixTranslator::parseXML(const value::Value& value)
{
    const value::Map& root = value::toMapValue(value);

    {
        const value::Tuple& grid = root.getTuple("grid");
        m_dimension = grid.size() - 1;
        int dim = 0;
        for (value::Tuple::const_iterator it = grid.value().begin(); it !=
             grid.value().end(); ++it, ++dim) {
            m_size[dim] = boost::numeric_cast < int >(*it);
        }
    }

    {
        const value::Map& cells = root.getMap("cells");
        const std::string& connectivity = cells.getString("connectivity");
        if (connectivity == "von neumann") {
            m_connectivity = VON_NEUMANN;
        } else if (connectivity == "moore") {
            m_connectivity = MOORE;
        } else {
            m_connectivity = LINEAR;
        }

        if (cells.exist("symmetricport")) {
            m_symmetricport = cells.getBoolean("symmetricport");
        }
        m_prefix = cells.getString("prefix");

        if (cells.exist("library")) {
            m_library = cells.getString("library");
            if (cells.exist("model")) {
                m_model = cells.getString("model");
            }
            m_multiple = false;
        } else if (cells.exist("libraries")) {
            int index = 0;
            const value::Set& libraries = cells.getSet("libraries");
            for (value::Set::const_iterator it = libraries.begin();
                 it != libraries.end(); ++it, ++index) {
                const value::Map* library = value::toMapValue(*it);

                const std::string& lib = library->getString("library");

                if (library->exist("model")) {
                    const std::string& model = library->getString("model");
                    m_libraries[index] = std::make_pair(lib, model);
                } else {
                    m_libraries[index] = std::make_pair(lib, "");
                }
            }
            m_multiple = true;
        }

        if (cells.exist("class")) {
            m_class = cells.getString("class");
            m_multiple = false;
        } else if (cells.exist("classes")) {
            const value::Set& classes = cells.getSet("classes");
            for (value::Set::const_iterator it = classes.begin();
                 it != classes.end(); ++it) {
                m_classes.push_back(value::toString(*it));
            }
            m_multiple = true;
        }

        if (cells.exist("init")) {
            if (m_dimension == 0) {
                m_init = new unsigned int[m_size[0]];
            } else {
                m_init = new unsigned int[m_size[0]*m_size[1]];
            }

            const value::Tuple& init = cells.getTuple("init");
            unsigned int i = 0;

            for (value::Tuple::const_iterator it = init.value().begin(); it !=
                 init.value().end(); ++it) {
                m_init[i] = boost::numeric_cast < unsigned int >(*it);
                ++i;
            }
        }
    }
}

void MatrixTranslator::translate(const value::Value& buffer)
{
    try {
        parseXML(buffer);

        translateDynamics();
        translateConditions();
        translateStructures();
    } catch (const std::exception& e) {
        throw utils::InternalError(fmt(
                _("Matrix translator error: %1%")) % e.what());
    }
}

void MatrixTranslator::translateModel(unsigned int i,
                                      unsigned int j)
{
    const vpz::AtomicModel* atomicModel;

    if (not m_library.empty() or not m_libraries.empty()) {
        std::vector < std::string > conditions;
        conditions.push_back("cond_cell");
        conditions.push_back((fmt("cond_%1%_%2%_%3%")
                              % m_prefix % i % j).str());

        atomicModel = m_exe.createModel(getName(i, j),
                                        std::vector < std::string >(),
                                        std::vector < std::string >(),
                                        getDynamics(i, j),
                                        conditions, "obs_cell");
        m_models[getName(i,j)] = atomicModel;
    } else {
        atomicModel = dynamic_cast < const vpz::AtomicModel*>(
            m_exe.createModelFromClass(getClass(i, j), getName(i, j)));
    }

    if (i != 1)
        m_exe.addInputPort(atomicModel->getName(), "N");

    if (j != 1)
        m_exe.addInputPort(atomicModel->getName(), "W");

    if (i != m_size[0])
        m_exe.addInputPort(atomicModel->getName(), "S");

    if (j != m_size[1])
        m_exe.addInputPort(atomicModel->getName(), "E");

    if (m_connectivity == VON_NEUMANN) {
        if (i != 1 and j != 1) {
            m_exe.addInputPort(atomicModel->getName(), "NW");
        }

        if (i != 1 and j != m_size[1]) {
            m_exe.addInputPort(atomicModel->getName(), "NE");
        }

        if (i != m_size[0] and j != 1) {
            m_exe.addInputPort(atomicModel->getName(), "SW");
        }

        if (i != m_size[0] and j != m_size[1]) {
            m_exe.addInputPort(atomicModel->getName(), "SE");
        }
    }

    if (m_symmetricport) {
        if (i != 1) {
            m_exe.addOutputPort(atomicModel->getName(), "N");
        }

        if (j != 1) {
            m_exe.addOutputPort(atomicModel->getName(), "W");
        }

        if (i != m_size[0]) {
            m_exe.addOutputPort(atomicModel->getName(), "S");
        }

        if (j != m_size[1]) {
            m_exe.addOutputPort(atomicModel->getName(), "E");
        }

        if (m_connectivity == VON_NEUMANN) {
            if (i != 1 and j != 1) {
                m_exe.addOutputPort(atomicModel->getName(), "NW");
            }

            if (i != 1 and j != m_size[1]) {
                m_exe.addOutputPort(atomicModel->getName(), "NE");
            }

            if (i != m_size[0] and j != 1) {
                m_exe.addOutputPort(atomicModel->getName(), "SW");
            }

            if (i != m_size[0] and j != m_size[1]) {
                m_exe.addOutputPort(atomicModel->getName(), "SE");
            }
        }
    } else {
        m_exe.addOutputPort(atomicModel->getName(), "out");
    }
}

void MatrixTranslator::translateStructures()
{
    if (m_dimension == 0) {
        for (unsigned int i = 1; i <= m_size[0]; i++) {
            std::vector < std::string > conditions;

            conditions.push_back("cond_cell");
            conditions.push_back((fmt("cond_%1%_%2%") % m_prefix %
                                  i).str());
            const vpz::AtomicModel* atomicModel = m_exe.createModel(
                getName(i), std::vector < std::string >(),
                std::vector < std::string >(), getDynamics(i), conditions,
                "obs_cell");
            m_models[getName(i)] = atomicModel;

            if (i != 1) {
                m_exe.addInputPort(atomicModel->getName(), "L");
            }

            if (i != m_size[0]) {
                m_exe.addInputPort(atomicModel->getName(), "R");
            }

            if (m_symmetricport) {
                if (i != 1) {
                    m_exe.addOutputPort(atomicModel->getName(), "L");
                }

                if (i != m_size[0]) {
                    m_exe.addOutputPort(atomicModel->getName(), "R");
                }
            } else {
                m_exe.addOutputPort(atomicModel->getName(), "out");
            }
        }
    } else {
        if (m_dimension == 1) {
            for (unsigned int j = 1; j <= m_size[1]; j++) {
                for (unsigned int i = 1; i <= m_size[0]; i++) {
                    if (existModel(i, j)) {
                        translateModel(i, j);
                    }
                }
            }
        }
    }

    if (m_dimension == 0) {
        for (unsigned int i = 1; i <= m_size[0]; i++) {
            if (m_symmetricport) {
                if (i != 1) {
                    m_exe.addConnection(getName(i), "L", getName(i - 1), "R");
                }

                if (i != m_size[0]) {
                    m_exe.addConnection(getName(i), "R", getName(i + 1), "L");
                }
            } else {
                if (i != 1) {
                    m_exe.addConnection(getName(i), "out", getName(i - 1), "R");
                }

                if (i != m_size[0]) {
                    m_exe.addConnection(getName(i), "out", getName(i + 1), "L");
                }
            }
        }
    } else {
        if (m_dimension == 1) {
            if (m_symmetricport) {
                for (unsigned int j = 1; j <= m_size[1]; j++) {
                    for (unsigned int i = 1; i <= m_size[0]; i++) {
                        if (existModel(i, j)) {
                            translateSymmetricConnection2D(i, j);
                        }
                    }
                }
            } else {
                for (unsigned int j = 1; j <= m_size[1]; j++) {
                    for (unsigned int i = 1; i <= m_size[0]; i++) {
                        if (existModel(i, j)) {
                            translateConnection2D(i, j);
                        }
                    }
                }
            }
        }
    }
}

void MatrixTranslator::translateSymmetricConnection2D(unsigned int i,
                                                      unsigned int j)
{
    if (j != 1 and existModel(i, j - 1))
        m_exe.addConnection(getName(i, j), "W", getName(i, j - 1), "E");

    if (i != 1 and existModel(i - 1, j))
        m_exe.addConnection(getName(i, j), "N", getName(i - 1, j), "S");

    if (j != m_size[1] and existModel(i, j + 1))
        m_exe.addConnection(getName(i, j), "E", getName(i, j + 1), "W");

    if (i != m_size[0] and existModel(i + 1, j))
        m_exe.addConnection(getName(i, j), "S", getName(i + 1, j), "N");

    if (m_connectivity == VON_NEUMANN) {
        if (j != 1 and i != 1 and existModel(i - 1, j - 1))
            m_exe.addConnection(getName(i, j), "SW", getName(i - 1, j - 1),
                                "SE");

        if (j != m_size[1] and i != 1 and existModel(i - 1, j + 1))
            m_exe.addConnection(getName(i, j), "SE", getName(i - 1, j + 1),
                                "SW");

        if (j != 1 and i != m_size[0] and existModel(i + 1, j - 1))
            m_exe.addConnection(getName(i, j), "NW", getName(i + 1, j - 1),
                                "NE");

        if (j != m_size[1] and i != m_size[0] and existModel(i + 1, j + 1))
            m_exe.addConnection(getName(i, j), "NE", getName(i + 1, j + 1),
                                "NW");
    }
}

void MatrixTranslator::translateConnection2D(unsigned int i,
                                             unsigned int j)
{
    if (j != 1 and existModel(i, j - 1))
        m_exe.addConnection(getName(i, j), "out", getName(i, j - 1), "E");

    if (i != 1 and existModel(i - 1, j))
        m_exe.addConnection(getName(i, j), "out", getName(i - 1, j), "S");

    if (j != m_size[1] and existModel(i, j + 1))
        m_exe.addConnection(getName(i, j), "out", getName(i, j + 1), "W");

    if (i != m_size[0] and existModel(i + 1, j))
        m_exe.addConnection(getName(i, j), "out", getName(i + 1, j), "N");

    if (m_connectivity == VON_NEUMANN) {
        if (j != 1 and i != 1 and existModel(i - 1, j - 1))
            m_exe.addConnection(getName(i, j), "out", getName(i - 1, j - 1),
                                "SE");

        if (j != m_size[1] and i != 1 and existModel(i - 1, j + 1))
            m_exe.addConnection(getName(i, j), "out", getName(i - 1, j + 1),
                                "SW");

        if (j != 1 and i != m_size[0] and existModel(i + 1, j - 1))
            m_exe.addConnection(getName(i, j), "out", getName(i + 1, j - 1),
                                "NE");

        if (j != m_size[1] and i != m_size[0] and existModel(i + 1, j + 1))
            m_exe.addConnection(getName(i, j), "out", getName(i + 1, j + 1),
                                "NW");
    }
}

void MatrixTranslator::translateDynamics()
{
    if (m_multiple) {
        if (not m_libraries.empty()) {
            libraries_t::const_iterator it = m_libraries.begin();

            while (it != m_libraries.end()) {
                vpz::Dynamic dynamics((fmt("dyn_cell_%1%") %
                                       it->first).str());

                dynamics.setLibrary(it->second.first);
                m_exe.dynamics().add(dynamics);
                ++it;
            }
        }
    } else {
        if (not m_library.empty()) {
            vpz::Dynamic dynamics("cell");

            dynamics.setLibrary(m_library);
            m_exe.dynamics().add(dynamics);
        }
    }
}

void MatrixTranslator::translateCondition1D(unsigned int i)
{
    std::string name = (fmt("cond_%1%_%2%")
                        % m_prefix % i).str();
    vpz::Condition condition(name);
    value::Set* neighbourhood = value::Set::create();

    if (i != 1)
        neighbourhood->add(value::String::create("L"));

    if (i != m_size[0])
        neighbourhood->add(value::String::create("R"));

    condition.addValueToPort("Neighbourhood", neighbourhood);

    condition.addValueToPort("_x", value::Integer::create(i));

    m_exe.conditions().add(condition);
}

void MatrixTranslator::translateCondition2D(unsigned int i,
                                            unsigned int j)
{
    std::string name = (fmt("cond_%1%_%2%_%3%")
                        % m_prefix % i % j).str();
    vpz::Condition condition(name);
    value::Set* neighbourhood = value::Set::create();

    if (i != 1)
        neighbourhood->add(value::String::create("N"));

    if (j != m_size[1])
        neighbourhood->add(value::String::create("E"));

    if (i != m_size[0])
        neighbourhood->add(value::String::create("S"));

    if (j != 1)
        neighbourhood->add(value::String::create("W"));

    if (m_connectivity == VON_NEUMANN) {
        if (i != 1 and j != 1)
            neighbourhood->add(value::String::create("NW"));

        if (i != 1 and j != m_size[1])
            neighbourhood->add(value::String::create("NE"));

        if (i != m_size[0] and j != 1)
            neighbourhood->add(value::String::create("SW"));

        if (i != m_size[0] and j != m_size[1])
            neighbourhood->add(value::String::create("SE"));
    }

    condition.addValueToPort("Neighbourhood", neighbourhood);

    condition.addValueToPort("_x", value::Integer::create(i));
    condition.addValueToPort("_y", value::Integer::create(j));
    m_exe.conditions().add(condition);
}

void MatrixTranslator::translateConditions()
{
    if (m_dimension == 0) {
        for (unsigned int i = 1; i <= m_size[0]; i++) {
            if (!m_init || m_init[i-1] != 0) {
                translateCondition1D(i);
            }
        }
    } else {
        if (m_dimension == 1) {
            for (unsigned int i = 1; i <= m_size[0]; i++) {
                for (unsigned int j = 1; j <= m_size[1]; j++) {
                    if (existModel(i, j)) {
                        translateCondition2D(i, j);
                    }
                }
            }
        }
    }
}

}} // namespace vle translator

