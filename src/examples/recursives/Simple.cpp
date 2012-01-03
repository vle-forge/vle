/*
 * @file examples/recursives/Simple.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
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


#include <vle/devs/Dynamics.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/manager/Run.hpp>
#include <vle/utils/Path.hpp>
#include <iostream>

namespace vle { namespace examples { namespace recursives {

    class SimpleSimulation : public devs::Dynamics
    {
    public:
        SimpleSimulation(const devs::DynamicsInit& mdl,
                         const devs::InitEventList& lst)
            : devs::Dynamics(mdl, lst)
        {}

        virtual ~SimpleSimulation()
        {}

        devs::Time init(const devs::Time& /* time */)
        {
            vpz::Vpz file(utils::Path::path().getExampleFile("counter.vpz"));

            manager::RunQuiet r;    // build a VLE manager
            r.start(file);          // exacute the local file simulation

            // get output of the simulation
            const oov::OutputMatrixViewList& result(r.outputs());

            // Show the results
            std::for_each(result.begin(), result.end(), ShowMatrix());

            // le modèle ne fait rien d'autre.
            return devs::Time::infinity;
        }

        /**
         * @brief A functor to show the matrix result.
         */
        struct ShowMatrix {
            void operator()(
                const oov::OutputMatrixViewList::value_type& x) const
            {
                std::cout << "view: " << x.first << "\n"
                   << x.second.serialize()->writeToString() << "\n";
            }
        };
    };

    class RecursiveSimulation : public devs::Dynamics
    {
    public:
        RecursiveSimulation(const devs::DynamicsInit& mdl,
                         const devs::InitEventList& lst)
            : devs::Dynamics(mdl, lst)
        {}

        virtual ~RecursiveSimulation()
        {}

        virtual devs::Time init(const devs::Time& /* time */)
        {
            if (getModelName() == "10") {    // close the recursive simulation
                return devs::Time::infinity; // creation after
            }                                // 10 recursive models
            return 0.0;
        }

        virtual void internalTransition(const devs::Time& /* time */)
        {
            const std::string& myname(getModelName());
            const int nb = boost::lexical_cast < int >(myname);

            vpz::Vpz file(utils::Path::path().getExampleFile("recursive.vpz"));

            graph::Model::rename(file.project().model().model(),
                                 boost::lexical_cast < std::string >(nb + 1));

            manager::RunQuiet r; // build a VLE manager
            r.start(file);       // execute the local file simulation
        }
    };

    DECLARE_NAMED_DYNAMICS(simple, SimpleSimulation)
    DECLARE_NAMED_DYNAMICS(recursive, RecursiveSimulation)

}}} // namespace vle examples recursives
