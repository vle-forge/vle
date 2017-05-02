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

#ifndef VLE_VPZ_VIEWS_HPP
#define VLE_VPZ_VIEWS_HPP

#include <vle/DllDefines.hpp>
#include <vle/vpz/Base.hpp>
#include <vle/vpz/Observables.hpp>
#include <vle/vpz/Outputs.hpp>
#include <vle/vpz/View.hpp>

namespace vle {
namespace vpz {

/**
 * @brief A list of View.
 */
typedef std::map<std::string, View> ViewList;

/**
 * @brief Views is a container based on ViewList to build a list of View
 * using the View's name as key. The Views class store Outputs and
 * Observables objects too.
 */
class VLE_API Views : public Base
{
public:
    typedef ViewList::iterator iterator;
    typedef ViewList::const_iterator const_iterator;
    typedef ViewList::size_type size_type;
    typedef ViewList::value_type value_type;

    /**
     * @brief Build an empty Views.
     */
    Views()
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~Views()
    {
    }

    /**
     * @brief Write into the stream the XML representation of these class.
     * @code
     * <views>
     *  [...] <!-- outputs -->
     *  [...] <!-- observables -->
     *  [...] <!-- view -->
     * </views>
     * @endcode
     * @param out The output stream.
     */
    virtual void write(std::ostream& out) const override;

    /**
     * @brief Get the type of this class.
     * @return VIEWS.
     */
    virtual Base::type getType() const override
    {
        return VLE_VPZ_VIEWS;
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage outputs objects
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Add a local Output stream.
     * @param name The name of this output.
     * @param location The location of this output (on local filesystem).
     * @param plugin The name of the plugin.
     * @param package The package of the plugin.
     * @return A reference to the newly output.
     * @throw utils::ArgError if name already exists.
     */
    Output& addStreamOutput(const std::string& name,
                            const std::string& location,
                            const std::string& plugin,
                            const std::string& package)
    {
        return m_outputs.addStream(name, location, plugin, package);
    }

    /**
     * @brief Add a local Output stream.
     * @param name The name of this output.
     * @param location The location of this output (on local filesystem).
     * @param plugin The name of the plugin.
     * @return A reference to the newly output.
     * @throw utils::ArgError if name already exists.
     */
    Output& addStreamOutput(const std::string& name,
                            const std::string& location,
                            const std::string& plugin)
    {
        return m_outputs.addStream(name, location, plugin);
    }

    /**
     * @brief Delete the specified Output.
     * @param name The name of the Output to delete.
     */
    void delOutput(const std::string& name);

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage observables objects
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Add an Observables.
     * @param obs The Observables to add.
     * @return A reference to the newly Observable.
     * @throw utils::ArgError if name already exists.
     */
    Observable& addObservable(const Observable& obs);

    /**
     * @brief Add an Observables.
     * @param name The name of the Observables to add.
     * @return A reference to the newly Observable.
     * @throw utils::ArgError if name already exists.
     */
    Observable& addObservable(const std::string& name);

    /**
     * @brief Delete the specified Observables.
     * @param name The name of the Output to delete.
     */
    void delObservable(const std::string& name);

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage views objects
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Add the list of View, Outputs and Observables into the curent
     * Views.
     * @param views The set of objects to add.
     * @throw utils::ArgError if a object have the same name.
     */
    void add(const Views& views);

    /**
     * @brief Add a new view.
     * @param view The new view to add.
     * @return A reference to the newly View.
     * @throw utils::ArgError if a view already exist.
     */
    View& add(const View& view);

    /**
     * @brief Delete all objects from the Views: View, Outputs and
     * Observables.
     */
    void clear();

    /**
     * @brief Add a new event View.
     * @param name The name of the View.
     * @param type Type of the view.
     * @param output The output of the View.
     * @param enable If false, the view is disabled at simulation
     * @return A reference to the newly View.
     * @throw utils::ArgError if name already exist.
     */
    View& addEventView(const std::string& name,
                       View::Type type,
                       const std::string& output,
                       bool enable = true);

    /**
     * @brief Add a new timed View.
     * @param name The name of the View.
     * @param timestep The timestep of observation.
     * @param output The output of the View.
     * @param enable If false, the view is disabled at simulation
     * @return A reference to the newly View.
     * @throw utils::ArgError if name already exist.
     */
    View& addTimedView(const std::string& name,
                       double timestep,
                       const std::string& output,
                       bool enable = true);

    /**
     * @brief Delete the specified View.
     * @param name The name of the View to delete.
     */
    void del(const std::string& name);

    /**
     * @brief Get a constant reference to the specified View.
     * @param name The name of the View.
     * @return A constant reference to the specified View.
     * @throw utils::ArgError if name does not exist.
     */
    const View& get(const std::string& name) const;

    /**
     * @brief Get a reference to the specified View.
     * @param name The name of the View.
     * @return A reference to the specified View.
     * @throw utils::ArgError if name does not exist.
     */
    View& get(const std::string& name);

    /**
     * @brief Check if the specified View exists.
     * @param name The name of the View to test.
     * @return True if the View exists, false otherwise.
     */
    bool exist(const std::string& name) const
    {
        return m_list.find(name) != m_list.end();
    }

    /**
     * @brief For each vpz::View that vpz::Output name equal oldname are
     * remove and a copy with a new name if push.
     * @param oldname The old name of the vpz::Output.
     * @param newname The new name of the vpz::Output.
     */
    void renameOutput(const std::string& oldname, const std::string& newname);

    /**
     * @brief Rename a specified vpz::View oldname with the newname
     * parameter.
     * @param oldname The old name of the vpz::View
     * @param newname The new name of the vpz::View
     */
    void renameView(const std::string& oldname, const std::string& newname);

    /**
     * @brief Copy and create a vpz::Output
     * @param outputname the name of the original vpz::Output
     * @param copyname the name of the copy vpz::Output
     */
    void copyOutput(const std::string& outputname,
                    const std::string& copyname);

    /**
     * @brief Copy a specified vpz::View.
     * @param viewname the name of the current vpz::View
     * @param copyname the name of the copy
     */
    void copyView(const std::string& viewname, const std::string& copyname);

    /**
     * @brief Check if a View of the ViewList use the specific output.
     * @param name The name of the Output to check.
     * @return True if a View use an output with the, otherwise, return
     * false.
     */
    bool isUsedOutput(const std::string& name) const;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Get/Set functions.
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Get a constant reference to the Outputs.
     * @return Get a constant reference to the Outputs.
     */
    const Outputs& outputs() const
    {
        return m_outputs;
    }

    /**
     * @brief Get a reference to the Outputs.
     * @return Get a reference to the Outputs.
     */
    Outputs& outputs()
    {
        return m_outputs;
    }

    /**
     * @brief Get a constant reference to the Observables.
     * @return Get a constant reference to the Observables.
     */
    const Observables& observables() const
    {
        return m_observables;
    }

    /**
     * @brief Get a reference to the Observables.
     * @return Get a reference to the Observables.
     */
    Observables& observables()
    {
        return m_observables;
    }

    /**
     * @brief Get a constant reference to the ViewList.
     * @return Get a constant reference to the ViewList.
     */
    const ViewList& viewlist() const
    {
        return m_list;
    }

    /**
     * @brief Get a reference to the ViewList.
     * @return Get a reference to the ViewList.
     */
    ViewList& viewlist()
    {
        return m_list;
    }

    /**
     * @brief Get a iterator the begin of the ViewList.
     * @return Get a iterator the begin of the ViewList.
     */
    iterator begin()
    {
        return m_list.begin();
    }

    /**
     * @brief Get a iterator the end of the ViewList.
     * @return Get a iterator the end of the ViewList.
     */
    iterator end()
    {
        return m_list.end();
    }

    /**
     * @brief Get a constant iterator the begin of the ViewList.
     * @return Get a constant iterator the begin of the ViewList.
     */
    const_iterator begin() const
    {
        return m_list.begin();
    }

    /**
     * @brief Get a constant iterator the end of the ViewList.
     * @return Get a constant iterator the end of the ViewList.
     */
    const_iterator end() const
    {
        return m_list.end();
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Functors
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief A functor to add a ViewList::value_type of a Views. To use
     * with std::for_each.
     */
    struct AddViews
    {
        /**
         * @brief Constructor.
         * @param views The Views that store the View.
         */
        AddViews(Views& views)
          : m_views(views)
        {
        }

        /**
         * @brief Add View to the Views.
         * @param pair the View to add.
         */
        void operator()(const ViewList::value_type& pair)
        {
            m_views.add(pair.second);
        }

        Views& m_views; //!< the output parameter.
    };

    /**
     * @brief A functor to test if a ViewList::value_type use the specific
     * output. To use with std::find_if.
     */
    struct UseOutput
    {
        /**
         * @brief Constructor.
         * @param output The name of the output to find.
         */
        UseOutput(const std::string& output)
          : m_output(output)
        {
        }

        /**
         * @brief Check if the View use Output m_output.
         * @param pair the View to check.
         * @return True if View use Output m_output.
         */
        bool operator()(const ViewList::value_type& pair) const
        {
            return pair.second.output() == m_output;
        }

        const std::string& m_output; //!< the output name to find.
    };

private:
    ViewList m_list;
    Outputs m_outputs;
    Observables m_observables;
};
}
} // namespace vle vpz

#endif
