/**
 * @file vle/vpz/Classes.hpp
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


#ifndef VLE_VPZ_CLASSES_HPP
#define VLE_VPZ_CLASSES_HPP

#include <vle/vpz/Base.hpp>
#include <vle/vpz/Class.hpp>
#include <map>


namespace vle { namespace vpz {

    /** 
     * @brief Define a list of class by a dictionnary class name, class value.
     */
    typedef std::map < std::string, Class > ClassList;

    /** 
     * @brief The class Classes is build on the ClassList typedef to construct a
     * list of class.
     */
    class Classes : public Base
    {
    public:
        typedef ClassList::iterator        iterator;
        typedef ClassList::const_iterator  const_iterator;

        Classes() { }

        virtual ~Classes() { }

        /** 
         * @brief Write the classes xml representation into the output stream.
         * @code
         * <classes>
         *  <class name="xxxx">
         *   <structure>
         *    ...
         *   </structure>
         *  </class>
         * </classes>
         * @endcode
         * @param out a output stream where write the classes tags.
         */
        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return CLASSES; }

        /** 
         * @brief Add a new class into the class list.
         * @param name The name of the class.
         * @return A reference to the newly class.
         * @throw utils::SaxParserError if name already exist.
         */
        Class& add(const std::string& name);

        /** 
         * @brief Remove the specified class from the class list.
         * @param name The name of the class.
         */
        void del(const std::string& name);

        /** 
         * @brief Get a const reference to the specified class from the class
         * list.
         * @param name The name of the class.
         * @return A constant reference to the class.
         * @throw utils::SaxParserError if name does not exist.
         */
        const Class& get(const std::string& name) const;

        /** 
         * @brief Get a reference to the specified class from the class list.
         * @param name The name of the class.
         * @return A reference to the class.
         * @throw utils::SaxParserError if name does not exist.
         */
        Class& get(const std::string& name);

        /** 
         * @brief Check if a class already exist in the class list.
         * @param name The name of the class.
         * @return true if the class already exist, false otherwise.
         */
        bool exist(const std::string& name) const
        { return m_lst.find(name) != m_lst.end(); }

        /** 
         * @brief Return a constant iterator to the begin of the ClassList.
         * @return Return a constant iterator to the begin of the ClassList.
         */
        ClassList::const_iterator begin() const
        { return m_lst.begin(); }

        /** 
         * @brief Return a constant reference to the end of the ClassList.
         * @return Return a constant reference to the end of the ClassList.
         */
        ClassList::const_iterator end() const
        { return m_lst.end(); }

        /** 
         * @brief Get a constant reference to the whole class list.
         * @return Get a constant reference to the whole class list.
         */
        const ClassList& list() const
        { return m_lst; }

        /** 
         * @brief Get a reference to the whole class list.
         * @return Get a reference to the whole class list.
         */
        ClassList& list()
        { return m_lst; }

        /** 
         * @brief Remove all class from the class list.
         */
        void clear()
        { m_lst.clear(); }

        ////
        //// functors
        ////

        /** 
         * @brief A functor to get the class value of a ClassList::value_type.
         */
        struct ClassValue
        {
            const Class& operator()(const ClassList::value_type& x) const
            { return x.second; }
        };
        
        /** 
         * @brief A functor to get the class name of a ClassList::value_type.
         */
        struct ClassName
        {
            const std::string& operator()(const ClassList::value_type& x) const
            { return x.first; }
        };

    private:
        ClassList       m_lst;
    };

}} // namespace vle vpz

#endif
