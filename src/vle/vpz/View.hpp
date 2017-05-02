/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#ifndef VLE_VPZ_VIEW_HPP
#define VLE_VPZ_VIEW_HPP

#include <string>
#include <vle/DllDefines.hpp>
#include <vle/vpz/Base.hpp>

namespace vle {
namespace vpz {

/**
 * @brief A View made a link between a list of Observation and an Output
 * plug-in. This View can be timed by a timestep, finish or completely
 * event and make link with Output by name.
 */
class VLE_API View : public Base
{
public:
    /**
     * @brief Define the type of View.
     */
    enum Type
    {
        NOTHING = 0,
        TIMED = 1 << 1,
        OUTPUT = 1 << 2,
        INTERNAL = 1 << 3,
        EXTERNAL = 1 << 4,
        CONFLUENT = 1 << 5,
        FINISH = 1 << 6
    };

    /**
     * @brief Build a new event view with a specific name.
     * @param name The name of the View.
     */
    View(const std::string& name);

    /**
     * @brief Build a new View with a specific name, type and reference.
     * @param name The name of the View.
     * @param type The type of this View.
     * @param output The name of the Output.
     * @param timestep A timestep, necessary for type TIMED.
     * @throw utils::ArgError it the time step is not greater than 0 and the
     * Type is TIMED.
     */
    View(const std::string& name,
         View::Type type,
         const std::string& output,
         double timestep = 0.0,
         bool enable = true);

    /**
     * @brief Nothing to delete.
     */
    virtual ~View() = default;

    /**
     * @brief Write the XML representation of this class.
     * @code
     * <view name="name" output="outout" type="event" />
     * <view name="name" output="output" type="finish" />
     * @endcode
     * @param out Output stream.
     */
    virtual void write(std::ostream& out) const override;

    /**
     * @brief Get the type of this class.
     * @return VIEW.
     */
    virtual Base::type getType() const override
    {
        return VLE_VPZ_VIEW;
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Get/Set functions.
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Get the name of this View.
     * @return The name.
     */
    inline const std::string& name() const
    {
        return m_name;
    }

    /**
     * @brief Set a new name for this View.
     * @param name The new name for this View.
     q*/
    inline void setName(std::string name)
    {
        m_name = name;
    }

    /**
     * @brief Get the type of this View.
     * @return The type, TIMED, FINISH or EVENT.
     */
    inline const Type& type() const
    {
        return m_type;
    }

    /**
     * @brief Set a new type for this View.
     * @param type The new type, TIMED, FINISH or EVENT.
     */
    inline void setType(Type type)
    {
        m_type = type;
    }

    /**
     * @brief Get a string representation of the current type.
     * @return "timed", "event" or "finish".
     */
    std::string streamtype() const;

    /**
     * @brief Assign a new time step to the timed view.
     * @param time The new time.
     * @throw utils::ArgError if time is not greater thant 0.0.
     */
    void setTimestep(double time);

    /**
     * @brief Get the time step.
     * @return A time step.
     */
    inline double timestep() const
    {
        return m_timestep;
    }

    /**
     * @brief Check if the view is enable.
     * @details Return true if the view is enabled. Default (in
     * constructor) views are enabled. Use the \c disable member function
     * to disable the view.
     *
     * @return true if the View is enable, false otherwise.
     */
    inline bool is_enable() const
    {
        return m_enabled;
    }

    /**
     * @brief Disable the View.
     * @details Disable the View. Use this function in modelling or
     * experimental frames tools. Simulation kernel only use the \c
     * is_enable function at simulation startup.
     */
    inline void disable()
    {
        m_enabled = false;
    }

    /**
     * @brief Enable the View.
     * @details Disable the View. Use this function in modelling or
     * experimental frames tools. Simulation kernel only use the \c
     * is_enable function at simulation startup.
     */
    inline void enable()
    {
        m_enabled = true;
    }

    /**
     * @brief The string representation of the Output.
     * @return The Output.
     */
    inline const std::string& output() const
    {
        return m_output;
    }

    /**
     * @brief The CDATA attached to this View.
     * @return The CDATA.
     */
    inline const std::string& data() const
    {
        return m_data;
    }

    /**
     * @brief Assign a new CDATA.
     * @param data The new CDATA.
     */
    inline void setData(const std::string& data)
    {
        m_data = data;
    }

    /**
     * @brief A operator to compare two Views
     * @param view The View to compare
     * @return True if the views are equals
     */
    bool operator==(const View& view) const;

private:
    double m_timestep;
    std::string m_name;
    std::string m_output;
    std::string m_data;
    bool m_enabled;
    Type m_type;
};

inline View::Type
operator|(View::Type lhs, View::Type rhs) noexcept
{
    return static_cast<View::Type>(static_cast<unsigned>(lhs) |
                                   static_cast<unsigned>(rhs));
}

inline View::Type operator&(View::Type lhs, View::Type rhs) noexcept
{
    return static_cast<View::Type>(static_cast<unsigned>(lhs) &
                                   static_cast<unsigned>(rhs));
}

inline View::Type
operator^(View::Type lhs, View::Type rhs) noexcept
{
    return static_cast<View::Type>(static_cast<unsigned>(lhs) ^
                                   static_cast<unsigned>(rhs));
}

inline View::Type operator~(View::Type flags) noexcept
{
    return static_cast<View::Type>(~static_cast<unsigned>(flags));
}

inline View::Type&
operator|=(View::Type& lhs, View::Type rhs) noexcept
{
    return (lhs = static_cast<View::Type>(static_cast<unsigned>(lhs) |
                                          static_cast<unsigned>(rhs)));
}

inline View::Type&
operator&=(View::Type& lhs, View::Type rhs) noexcept
{
    return (lhs = static_cast<View::Type>(static_cast<unsigned>(lhs) &
                                          static_cast<unsigned>(rhs)));
}

inline View::Type&
operator^=(View::Type& lhs, View::Type rhs) noexcept
{
    return (lhs = static_cast<View::Type>(static_cast<unsigned>(lhs) ^
                                          static_cast<unsigned>(rhs)));
}
}
} // namespace vle vpz

#endif
