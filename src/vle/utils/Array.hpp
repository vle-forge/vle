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

#ifndef VLE_UTILS_ARRAY_HPP
#define VLE_UTILS_ARRAY_HPP

#include <vector>
#include <vle/utils/Exception.hpp>

namespace vle {
namespace utils {

/**
 * An \e Array defined a two-dimensional template array. Informations are
 * stored into a \e std::vector<T> by default.
 *
 * \tparam T Type of element
 * \tparam Containre Type of container to store two-dimensional array.
 */
template <typename T, class Container = std::vector<T>>
class Array
{
public:
    using container_type = Container;
    using value_type = T;
    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;
    using reverse_iterator = typename container_type::reverse_iterator;
    using const_reverse_iterator =
      typename container_type::const_reverse_iterator;
    using size_type = typename container_type::size_type;

protected:
    container_type m_c;
    size_type m_rows, m_columns;

public:
    Array();

    explicit Array(size_type cols, size_type rows);
    explicit Array(size_type cols, size_type rows, const value_type& value);

    ~Array() = default;

    Array(const Array& q) = default;
    Array(Array&& q) = default;

    Array& operator=(const Array& q) = default;
    Array& operator=(Array&& q) = default;

    Array& operator=(std::initializer_list<value_type> init);

    void resize(size_type cols, size_type rows);
    void resize(size_type cols, size_type rows, const value_type& value);

    template <class InputIterator>
    void assign(InputIterator first, InputIterator last);

    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    iterator end() noexcept;
    const_iterator end() const noexcept;

    reverse_iterator rbegin() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    reverse_iterator rend() noexcept;
    const_reverse_iterator rend() const noexcept;

    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator crend() const noexcept;

    bool empty() const noexcept;
    size_type size() const noexcept;

    size_type rows() const noexcept;
    size_type columns() const noexcept;

    void set(size_type col, size_type row, const value_type& x);
    void set(size_type col, size_type row, value_type&& x);

    template <class... Args>
    void emplace(size_type col, size_type row, Args&&... args);

    const_reference operator()(size_type col, size_type row) const;
    reference operator()(size_type col, size_type row);

    void swap(Array& c) noexcept(noexcept(swap(m_c, c.m_c)));

private:
    void m_check_index(size_type col, size_type) const;
};

template <typename T, class Container>
Array<T, Container>::Array()
  : m_c()
  , m_rows(0)
  , m_columns(0)
{
}

template <typename T, class Container>
Array<T, Container>::Array(size_type columns, size_type rows)
  : m_c(rows * columns)
  , m_rows(rows)
  , m_columns(columns)
{
}

template <typename T, class Container>
Array<T, Container>::Array(size_type columns,
                           size_type rows,
                           const value_type& value)
  : m_c(rows * columns, value)
  , m_rows(rows)
  , m_columns(columns)
{
}

template <typename T, class Container>
Array<T, Container>&
Array<T, Container>::operator=(std::initializer_list<value_type> init)
{
    assign(init.begin(), init.end());
    return *this;
}

template <typename T, class Container>
void
Array<T, Container>::resize(size_type cols, size_type rows)
{
    container_type new_c(rows * cols);

    size_type rmin = std::min(rows, m_rows);
    size_type cmin = std::min(cols, m_columns);

    for (size_type r = 0; r != rmin; ++r)
        for (size_type c = 0; c != cmin; ++c)
            new_c[r * cols + c] = m_c[r * m_columns + c];

    m_columns = cols;
    m_rows = rows;
    std::swap(new_c, m_c);
}

template <typename T, class Container>
void
Array<T, Container>::resize(size_type cols,
                            size_type rows,
                            const value_type& value)
{
    m_c.resize(rows * cols);
    m_rows = rows;
    m_columns = cols;

    std::fill(std::begin(m_c), std::end(m_c), value);
}

template <typename T, class Container>
template <class InputIterator>
void
Array<T, Container>::assign(InputIterator first, InputIterator last)
{
    for (size_type r = 0; r != m_rows; ++r) {
        for (size_type c = 0; c != m_columns; ++c) {
            set(c, r, *first++);
            if (first == last)
                return;
        }
    }
}

template <typename T, class Container>
typename Array<T, Container>::iterator
Array<T, Container>::begin() noexcept
{
    return m_c.begin();
}

template <typename T, class Container>
typename Array<T, Container>::const_iterator
Array<T, Container>::begin() const noexcept
{
    return m_c.begin();
}

template <typename T, class Container>
typename Array<T, Container>::iterator
Array<T, Container>::end() noexcept
{
    return m_c.end();
}

template <typename T, class Container>
typename Array<T, Container>::const_iterator
Array<T, Container>::end() const noexcept
{
    return m_c.end();
}

template <typename T, class Container>
typename Array<T, Container>::reverse_iterator
Array<T, Container>::rbegin() noexcept
{
    return m_c.rbegin();
}

template <typename T, class Container>
typename Array<T, Container>::const_reverse_iterator
Array<T, Container>::rbegin() const noexcept
{
    return m_c.rbegin();
}

template <typename T, class Container>
typename Array<T, Container>::reverse_iterator
Array<T, Container>::rend() noexcept
{
    return m_c.rend();
}

template <typename T, class Container>
typename Array<T, Container>::const_reverse_iterator
Array<T, Container>::rend() const noexcept
{
    return m_c.rend();
}

template <typename T, class Container>
typename Array<T, Container>::const_iterator
Array<T, Container>::cbegin() const noexcept
{
    return m_c.cbegin();
}

template <typename T, class Container>
typename Array<T, Container>::const_iterator
Array<T, Container>::cend() const noexcept
{
    return m_c.cend();
}

template <typename T, class Container>
typename Array<T, Container>::const_reverse_iterator
Array<T, Container>::crbegin() const noexcept
{
    return m_c.crbegin();
}

template <typename T, class Container>
typename Array<T, Container>::const_reverse_iterator
Array<T, Container>::crend() const noexcept
{
    return m_c.crend();
}

template <typename T, class Container>
bool
Array<T, Container>::empty() const noexcept
{
    return m_c.empty();
}

template <typename T, class Container>
typename Array<T, Container>::size_type
Array<T, Container>::size() const noexcept
{
    return m_c.size();
}

template <typename T, class Container>
typename Array<T, Container>::size_type
Array<T, Container>::rows() const noexcept
{
    return m_rows;
}

template <typename T, class Container>
typename Array<T, Container>::size_type
Array<T, Container>::columns() const noexcept
{
    return m_columns;
}

template <typename T, class Container>
void
Array<T, Container>::set(size_type column, size_type row, const value_type& x)
{
    m_check_index(column, row);
    m_c[row * m_columns + column] = x;
}

template <typename T, class Container>
void
Array<T, Container>::set(size_type column, size_type row, value_type&& x)
{
    m_check_index(column, row);
    m_c.emplace(std::begin(m_c) + (row * m_columns + column), std::move(x));
}

template <typename T, class Container>
template <class... Args>
void
Array<T, Container>::emplace(size_type column, size_type row, Args&&... args)
{
    m_check_index(column, row);
    m_c.emplace(std::begin(m_c) + (row * m_columns + column),
                std::forward<Args>(args)...);
}

template <typename T, class Container>
typename Array<T, Container>::const_reference
Array<T, Container>::operator()(size_type column, size_type row) const
{
    m_check_index(column, row);
    return m_c[row * m_columns + column];
}

template <typename T, class Container>
typename Array<T, Container>::reference
Array<T, Container>::operator()(size_type column, size_type row)
{
    m_check_index(column, row);
    return m_c[row * m_columns + column];
}

template <typename T, class Container>
void
Array<T, Container>::swap(Array& c) noexcept(noexcept(swap(m_c, c.m_c)))
{
    std::swap(m_c, c.m_c);
    std::swap(m_columns, c.m_columns);
    std::swap(m_rows, c.m_rows);
}

template <typename T, class Container>
void
Array<T, Container>::m_check_index(size_type column, size_type row) const
{
    if (column >= m_columns or row >= m_rows)
        throw vle::utils::ArgError("Array: bad access");
}
}
} // namespace vle value

#endif
