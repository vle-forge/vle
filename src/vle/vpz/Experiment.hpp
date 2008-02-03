/**
 * @file vle/vpz/Experiment.hpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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


#ifndef VLE_VPZ_EXPERIMENT_HPP
#define VLE_VPZ_EXPERIMENT_HPP

#include <vle/vpz/Base.hpp>
#include <vle/vpz/Replicas.hpp>
#include <vle/vpz/Conditions.hpp>
#include <vle/vpz/Views.hpp>

namespace vle { namespace vpz {

    class Experiment : public Base
    {
    public:
        Experiment() :
            m_duration(0),
            m_seed(1)
        { }

        virtual ~Experiment() { }

        /** 
         * @brief Write Experiment information under specified root node
         * including Replicas, ExperimentalCondition and Views.
         * @code
         * <experiment name="exp1" duration="0.33" seed="65431">
         *   [...]
         * </experiment>
         * @endcode
         *
         */
        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return EXPERIMENT; }

        /** 
         * @brief Initialise Experiment information from XML including,
         * Replicas, ExperimentalCondition and Views.
         * @code
         * <experiment name="exp1" duration="0.33" seed="65431">
         *   [...]
         * </experiment>
         * @endcode
         * 
         * @param elt a xml element on the tag EXPERIMENT.
         *
         * @throw Exception::Parse if elt is NULL or name not equal to
         * EXPERIMENT tag.
         */
        void initFromExperiment(xmlpp::Element* elt);

        /** 
         * @brief Just delete the complete list of information from
         * vpz::Experiment.
         */
        void clear();

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
          * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        const Replicas& replicas() const
        { return m_replicas; }

        Replicas& replicas()
        { return m_replicas; }

        const Conditions& conditions() const
        { return m_conditions; }

        Conditions& conditions()
        { return m_conditions; }

        const Views& views() const
        { return m_views; }

        Views& views()
        { return m_views; }

        void addConditions(const Conditions& c);

        void addViews(const Views& m);

        /** 
         * @brief Remove all no permanent value of the Experiement ie.: all
         * observable and condition that are no permanent. This function is use
         * to clean not usefull data for the devs::ModelFactory.
         */
        void cleanNoPermanent();

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
          * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /** 
         * @brief Set the Experiment name.
         * 
         * @param name the Experiment name.
         *
         * @throw Exception::Internal if name is empty.
         */
        void setName(const std::string& name);

        /** 
         * @brief Get the Experiment name.
         * 
         * @return the Experiment name.
         */
        const std::string& name() const
        { return m_name; }

        /** 
         * @brief Set the duration of the Experiment file.
         * 
         * @param duration The duration to set Experiment file.
         *
         * @throw Exception::Internal if duration is <= 0.
         */
        void setDuration(double duration);

        /** 
         * @brief Get the duration of the Experiment file.
         * 
         * @return The duration.
         */
        double duration() const
        { return m_duration; }

        /** 
         * @brief Set the seed of the Experiment file.
         * 
         * @param seed The new seed to initialise the random generator.
         */
        void setSeed(guint32 seed)
        { m_seed = seed; }

        /** 
         * @brief Get the seed of the Experiment file.
         * 
         * @return The seed to initialise to random generator.
         */
        guint32 seed() const
        { return m_seed; }

        /** 
         * @brief Set the experimental design combination.
         * 
         * @param name The new name of experimental design combination.
         */
        void setCombination(const std::string& name);

        /** 
         * @brief Set the experimental design combination.
         * 
         * @return the current name of experimental design combination.
         */
        const std::string& combination() const
        { return m_combination; }

    private:
        std::string         m_name;
        double              m_duration;
        guint32             m_seed;
        std::string         m_combination;
        Replicas            m_replicas;
        Conditions          m_conditions;
        Views               m_views;
    };

}} // namespace vle vpz

#endif
