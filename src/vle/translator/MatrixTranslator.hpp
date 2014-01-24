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


#ifndef VLE_TRANSLATOR_MATRIXTRANSLATOR_HPP
#define VLE_TRANSLATOR_MATRIXTRANSLATOR_HPP

#include <vle/DllDefines.hpp>
#include <vle/devs/Executive.hpp>
#include <string>
#include <vector>

namespace vle { namespace translator {

    /**
     * @brief A translator to build Vector, Matrix 2D or 3D of Atomic model with
     * specific libraries.
     *
     * @code
     * <map>
     *  <key name="grid">
     *   <tuple>11 7</tuple>
     *  </key>
     *  <key name="cells">
     *   <map>
     *    <key name="connectivity"><string>neuman|moore</string></key>
     *    <key name="symmetricport"><boolean>0|1</boolean></key>
     *    <key name="prefix"><string>cell</string></key>
     *    <key name="library"><string>libcellule</string>/key>
     *    <key name="model"><string>model</string>/key>
     *    <!-- or -->
     *    <key name="libraries">
     *     <set>
     *      <map>
     *       <key name="library"><string>lib1</string></key>
     *      </map>
     *      <map>
     *       <key name="library"><string>lib2</string></key>
     *       <key name="model"><string>model</string></key>
     *      </map>
     *     </set>
     *    </key>
     *    <!-- or -->
     *    <key name="class"><string>Class</string></key>
     *    <!-- or -->
     *    <key name="classes">
     *     <set>
     *      <string>Class1</string>
     *      <string>Class2</string>
     *     </set>
     *    </key>
     *    <key name="init">
     *     <tuple>
     *    0 0 0 1 1 2 2 2 1 1 0
     *    0 0 0 0 1 1 2 1 1 1 0
     *    0 0 0 0 0 1 1 1 1 0 0
     *    0 0 0 0 0 0 1 1 0 0 0
     *    0 0 0 0 0 0 0 0 0 0 0
     *    0 0 0 0 0 0 0 0 0 0 0
     *    0 0 0 0 0 0 0 0 0 0 0
     *     </tuple>
     *    </key>
     *   </map>
     *  </key>
     * </map>
     *
     * <?xml version="1.0" ?>
     * <celldevs>
     *  <grid>
     *   <dim axe="0" number="11" />
     *   <dim axe="1" number="7" />
     *  </grid>
     *  <cells connectivity="neuman|moore" library="libcellule"
     *         symmetricport="true|false" >
     *   <libraries>
     *    <library index="1" name="lib1" />
     *    <library index="2" name="lib2" />
     *   </libraries>
     *   <prefix>cell</prefix>
     *   <init>
     *    0 0 0 1 1 1 1 1 1 1 0
     *    0 0 0 0 1 1 1 1 1 1 0
     *    0 0 0 0 0 1 1 1 1 0 0
     *    0 0 0 0 0 0 1 1 0 0 0
     *    0 0 0 0 0 0 0 0 0 0 0
     *    0 0 0 0 0 0 0 0 0 0 0
     *    0 0 0 0 0 0 0 0 0 0 0
     *   </init>
     *  </cells>
     * </celldevs>
     * Von neumann = 8
     * Mmoore = 4
     * @endcode
     */
    class VLE_API MatrixTranslator
    {
    public:
        MatrixTranslator(devs::Executive& exe)
            : m_exe(exe), m_dimension(0), m_init(0), m_symmetricport(false)
        {}

        virtual ~MatrixTranslator();

	std::string getName(unsigned int i, unsigned int j = 0) const;
	const vpz::AtomicModel* getModel(const std::string& name) const;
	unsigned int getSize(unsigned int i) const;
        void translate(const value::Value& buffer);

    private:
        typedef enum { VON_NEUMANN, MOORE, LINEAR } connectivity_t;
        typedef std::map < unsigned int, std::pair < std::string,
            std::string > > libraries_t;
        typedef std::vector < std::string > classes_t;

        devs::Executive& m_exe;

        unsigned int m_dimension;
        std::map < unsigned int, unsigned int > m_size;
        connectivity_t m_connectivity;
        bool m_multiple;

        // library + model
        std::string m_library;
        std::string m_model;
        libraries_t m_libraries;
        // classes
        std::string m_class;
        classes_t m_classes;

        std::string m_prefix;
        unsigned int* m_init;
        std::map < std::string , const vpz::AtomicModel* > m_models;
        bool m_symmetricport;

        bool existModel(unsigned int i, unsigned int j = 0);
        std::string getDynamics(unsigned int i, unsigned int j = 0);

        std::string getClass(unsigned int i, unsigned int j = 0);

        void parseXML(const value::Value& value);
        void translateModel(unsigned int i,
                            unsigned int j);
        void translateSymmetricConnection2D(unsigned int i,
                                            unsigned int j);
        void translateConnection2D(unsigned int i,
                                   unsigned int j);
        void translateStructures();
        void translateDynamics();
        void translateCondition2D(unsigned int i,
                                  unsigned int j);
        void translateCondition1D(unsigned int i);
        void translateConditions();
    };

}} // namespace vle translator

#endif
