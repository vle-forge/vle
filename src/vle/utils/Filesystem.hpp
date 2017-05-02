/*
    path.h -- A simple class for manipulating paths on Linux/Windows/Mac OS
    Copyright (c) 2015 Wenzel Jakob <wenzel@inf.ethz.ch>
    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.

    Filesystem.hpp -- A modified path.h with little changes
    Copyright (c) 2016-2017 INRA
    gauthier.quesnel@inra.fr
    - Remove make_absolute() functionality useless for VLE.
    - Add temp_directory_path() and unique_path() for tempory path.
    - Add DirectoryEntry and DirectoryIterator to pass trough repertory.
*/

#ifndef VLE_UTILS_FILESYSTEM_HPP
#define VLE_UTILS_FILESYSTEM_HPP

#include <memory>
#include <string>
#include <vector>
#include <vle/DllDefines.hpp>

namespace vle {
namespace utils {

class VLE_API Path
{
public:
    using container_type = std::vector<std::string>;
    using value_type = container_type::value_type;
    using iterator = container_type::iterator;
    using const_iterator = container_type::const_iterator;
    using reverse_iterator = container_type::reverse_iterator;
    using const_reverse_iterator = container_type::const_reverse_iterator;

    inline iterator begin() noexcept
    {
        return m_path.begin();
    }
    inline const_iterator begin() const noexcept
    {
        return m_path.cbegin();
    }
    inline const_iterator cbegin() const noexcept
    {
        return m_path.cbegin();
    }
    inline reverse_iterator rbegin() noexcept
    {
        return m_path.rbegin();
    }
    inline const_reverse_iterator crbegin() const noexcept
    {
        return m_path.crbegin();
    }
    inline iterator end() noexcept
    {
        return m_path.end();
    }
    inline const_iterator end() const noexcept
    {
        return m_path.cend();
    }
    inline const_iterator cend() const noexcept
    {
        return m_path.cend();
    }
    inline reverse_iterator rend() noexcept
    {
        return m_path.rend();
    }
    inline const_reverse_iterator crend() const noexcept
    {
        return m_path.crend();
    }

    enum path_type
    {
        windows_path = 0,
        posix_path = 1,
#if defined(_WIN32)
        native_path = windows_path
#else
        native_path = posix_path
#endif
    };

    Path();
    ~Path() = default;

    Path(const char* string);
    Path(const std::string& string);
#if defined(_WIN32)
    Path(const std::wstring& wstring);
    Path(const wchar_t* wstring);
    Path& operator=(const std::wstring& str);
#endif
    Path(const Path& path);
    Path(Path&& path);
    Path& operator=(const Path& path);
    Path& operator=(Path&& path);

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

    Path parent_path() const;

    Path operator/(const Path& other) const;

    Path& operator/=(const Path& other);

    std::string string(path_type type = native_path) const;

#if defined(_WIN32)
    std::wstring wstring(path_type type = native_path) const;

    void set(const std::wstring& wstring, path_type type = native_path);
#endif

    void set(const std::string& str, path_type type = native_path);

    bool remove() const;

    bool create_directory() const;

    bool create_directories() const;

    static bool is_directory(const Path& p);

    static bool is_file(const Path& p);

    static Path current_path();

    static bool current_path(const Path& p);

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
    static Path temp_directory_path();

    /**
     * The unique_path function generates a path name suitable for
     * creating temporary files, including directories. The name is based
     * on a model that uses the percent sign character to specify
     * replacement by a random hexadecimal digit.
     *
     * \example
     * Path p = Path::unique_path("vle-%%%%-%%%%-%%%%-%%%%.data");
     * \endexample
     *
     * \return A path identical to model, except that each occurrence of
     * a percent sign character is replaced by a random hexadecimal digit
     * character in the range 0-9, a-f.
     */
    static Path unique_path(const std::string& model);

    static void copy_file(const Path& from, const Path& to);

    static bool rename(const Path& from, const Path& to);

    static bool create_directory(const Path& p);

    static bool create_directories(const Path& p);

    friend bool operator==(const Path& p, const Path& q);
    friend bool operator!=(const Path& p, const Path& q);
    friend bool operator<(const Path& p, const Path& q);

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
    Path old;

public:
    FScurrent_path_restore(const Path& p)
      : old(Path::current_path())
    {
        if (not Path::current_path(p))
            old.clear();
    }

    ~FScurrent_path_restore() noexcept
    {
        if (not old.empty())
            Path::current_path(old);
    }
};

class DirectoryIterator;
class VLE_API DirectoryEntry
{
    Path m_path;
    bool m_is_file;
    bool m_is_directory;

public:
    DirectoryEntry();
    ~DirectoryEntry() = default;
    DirectoryEntry(const DirectoryEntry& rhs) = default;

    const Path& path() const;
    bool is_file() const;
    bool is_directory() const;

    friend DirectoryIterator;
};

class VLE_API DirectoryIterator
{
    struct Pimpl;
    std::shared_ptr<Pimpl> m_pimpl;

public:
    DirectoryIterator();
    DirectoryIterator(const Path& p);
    ~DirectoryIterator();

    DirectoryIterator& operator++();
    Path operator*() const;
    DirectoryEntry* operator->() const;

    friend void swap(DirectoryIterator& lhs, DirectoryIterator& rhs);
    friend bool operator==(const DirectoryIterator& lhs,
                           const DirectoryIterator& rhs);
    friend bool operator!=(const DirectoryIterator& lhs,
                           const DirectoryIterator& rhs);
};
}
} // namespace vle utils

#endif
