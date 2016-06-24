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

#ifndef VLE_UTILS_FILESYSTEM_HPP
#define VLE_UTILS_FILESYSTEM_HPP

#include <vle/DllDefines.hpp>
#include <memory>
#include <string>
#include <vector>

namespace vle { namespace utils {

class VLE_API FSpath
{
public:
    using container_type = std::vector<std::string>;
    using value_type = container_type::value_type;
    using iterator = container_type::iterator;
    using const_iterator = container_type::const_iterator;
    using reverse_iterator = container_type::reverse_iterator;
    using const_reverse_iterator = container_type::const_reverse_iterator;

    inline iterator begin() noexcept { return m_path.begin(); }
    inline const_iterator begin() const noexcept { return m_path.cbegin(); }
    inline const_iterator cbegin() const noexcept { return m_path.cbegin(); }
    inline reverse_iterator rbegin() noexcept { return m_path.rbegin(); }
    inline const_reverse_iterator crbegin() const noexcept { return m_path.crbegin(); }
    inline iterator end() noexcept { return m_path.end(); }
    inline const_iterator end() const noexcept { return m_path.cend(); }
    inline const_iterator cend() const noexcept { return m_path.cend(); }
    inline reverse_iterator rend() noexcept { return m_path.rend(); }
    inline const_reverse_iterator crend() const noexcept { return m_path.crend(); }

    enum path_type {
        windows_path = 0,
        posix_path = 1,
#if defined(_WIN32)
        native_path = windows_path
#else
        native_path = posix_path
#endif
    };

    FSpath();
    ~FSpath() = default;

    FSpath(const char *string);
    FSpath(const std::string &string);
#if defined(_WIN32)
    FSpath(const std::wstring &wstring);
    FSpath(const wchar_t *wstring);
    FSpath &operator=(const std::wstring& str);
#endif
    FSpath(const FSpath &path);
    FSpath(FSpath &&path);
    FSpath &operator=(const FSpath &path);
    FSpath &operator=(FSpath &&path);

    size_t length() const;

    bool empty() const;

    void clear() noexcept;

    bool is_absolute() const;

    bool exists() const;

    size_t file_size() const;

    bool is_directory() const;

    bool is_file() const;

    std::string extension() const;

    /**
     * Return the basename of the current path.
     * @code
     * std::string s = "/tmp/test";
     * std::string file = basename(s); // dir == test
     *
     * std::string x = "/tmp/test.toto";
     * std::string file = basename(s); // dir == test
     * @endcode
     * @return The basename.
     */
    std::string basename() const;

    /**
     * Return the filename of the current path.
     * @code
     * std::string s = "/tmp/test";
     * std::string file = filename(s); // dir == test
     *
     * std::string x = "/tmp/test.toto";
     * std::string file = filename(s); // dir == test.toto
     * @endcode
     *
     * @return The filename in the path.
     */
    std::string filename() const;

    FSpath parent_path() const;

    FSpath operator/(const FSpath &other) const;

    FSpath& operator/=(const FSpath &other);

    std::string string(path_type type = native_path) const;

#if defined(_WIN32)
    std::wstring wstring(path_type type = native_path) const;

    void set(const std::wstring &wstring, path_type type = native_path);
#endif

    void set(const std::string &str, path_type type = native_path);

    bool remove() const;

    bool create_directory() const;

    bool create_directories() const;

    static bool is_directory(const FSpath& p);

    static bool is_file(const FSpath& p);

    static FSpath current_path();

    static bool current_path(const FSpath& p);

    /**
     * temp_directory_path get a directory path.
     *
     * \return A directory path suitable for temporary files under the
     * conventions of the operating system. The specifics of how this path
     * is determined are implementation defined. An error shall be
     * reported if !exists(p) || !is_directory(p), where p is the path to
     * be returned.  ISO/IEC 9945: The path supplied by the first
     * environment variable found in the list TMPDIR, TMP, TEMP,
     * TEMPDIR. If none of these are found, "/tmp".
     * Windows: The path reported by the Windows GetTempPath API function.
     */
    static FSpath temp_directory_path();

    /**
     * The unique_path function generates a path name suitable for
     * creating temporary files, including directories. The name is based
     * on a model that uses the percent sign character to specify
     * replacement by a random hexadecimal digit.
     *
     * \example
     * FSpath p = FSpath::unique_path("vle-%%%%-%%%%-%%%%-%%%%.data");
     * \endexample
     *
     * \return A path identical to model, except that each occurrence of
     * a percent sign character is replaced by a random hexadecimal digit
     * character in the range 0-9, a-f.
     */
    static FSpath unique_path(const std::string& model);

    static void copy_file(const FSpath& from, const FSpath& to);

    static bool rename(const FSpath& from, const FSpath& to);

    static bool create_directory(const FSpath& p);

    static bool create_directories(const FSpath& p);

    friend bool operator==(const FSpath &p, const FSpath &q);
    friend bool operator!=(const FSpath &p, const FSpath &q);
    friend bool operator<(const FSpath &p, const FSpath &q);

private:
    std::vector<std::string> m_path;
    path_type m_type;
    bool m_absolute;
};

/**
 * Helper class to update and restore current path automatically.
 *
 */
class FScurrent_path_restore
{
    FSpath old;

public:
    FScurrent_path_restore(const FSpath& p)
        : old(FSpath::current_path())
    {
        if (not FSpath::current_path(p))
            old.clear();
    }

    ~FScurrent_path_restore() noexcept
    {
        if (not old.empty())
            FSpath::current_path(old);
    }
};

class FSdirectory_iterator;
class VLE_API FSdirectory_entry
{
    FSpath m_path;
    bool m_is_file;
    bool m_is_directory;

public:
    FSdirectory_entry();
    ~FSdirectory_entry() = default;
    FSdirectory_entry(const FSdirectory_entry& rhs) = default;

    const FSpath& path() const;
    bool is_file() const;
    bool is_directory() const;

    friend FSdirectory_iterator;
};

class VLE_API FSdirectory_iterator
{
    struct Pimpl;
    std::shared_ptr<Pimpl> m_pimpl;

public:
    FSdirectory_iterator();
    FSdirectory_iterator(const FSpath& p);
    ~FSdirectory_iterator();

    FSdirectory_iterator& operator++();
    FSpath operator*() const;
    FSdirectory_entry* operator->() const;

    friend void swap(FSdirectory_iterator& lhs,
                     FSdirectory_iterator& rhs);
    friend bool operator==(const FSdirectory_iterator& lhs,
                           const FSdirectory_iterator& rhs);
    friend bool operator!=(const FSdirectory_iterator& lhs,
                           const FSdirectory_iterator& rhs);
};

}} // namespace vle utils

#endif
