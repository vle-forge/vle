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

/*
    path.h -- A simple class for manipulating paths on Linux/Windows/Mac OS
    Copyright (c) 2015 Wenzel Jakob <wenzel@inf.ethz.ch>
    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#include <vle/utils/Filesystem.hpp>
#include <fstream>
#include <string>
#include <random>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <cassert>

#if defined(_WIN32)
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#if defined(__unix__)
#include <fts.h>
#endif

#if defined(__linux)
#include <linux/limits.h>
#endif

namespace {

std::vector<std::string>
tokenize(const std::string &string, const std::string &delim)
{
    std::string::size_type lastPos = 0, pos = string.find_first_of(delim,
                                                                   lastPos);
    std::vector<std::string> tokens;

    while (lastPos != std::string::npos) {
        if (pos != lastPos)
            tokens.push_back(string.substr(lastPos, pos - lastPos));
        lastPos = pos;
        if (lastPos == std::string::npos || lastPos + 1 == string.length())
            break;
        pos = string.find_first_of(delim, ++lastPos);
    }

    return tokens;
}

} // anonymous namespace

namespace vle { namespace utils {

FSpath::FSpath()
    : m_type(native_path)
    , m_absolute(false)
{}

FSpath::FSpath(const FSpath &path)
    : m_path(path.m_path)
    , m_type(path.m_type)
    , m_absolute(path.m_absolute)
{}

FSpath::FSpath(FSpath &&path)
    : m_path(std::move(path.m_path))
    , m_type(path.m_type)
    , m_absolute(path.m_absolute)
{}

FSpath::FSpath(const char *string)
{
    if (string)
        set(string);
}

FSpath::FSpath(const std::string &string)
{
    set(string);
}

#if defined(_WIN32)
FSpath::FSpath(const std::wstring &wstring)
{
    set(wstring);
}

FSpath::FSpath(const wchar_t *wstring)
{
    set(wstring);
}
#endif

size_t FSpath::length() const
{
    return m_path.size();
}

bool FSpath::empty() const
{
    return m_path.empty();
}

void FSpath::clear() noexcept
{
    m_path.clear();
}

bool FSpath::is_absolute() const
{
    return m_absolute;
}

bool FSpath::exists() const
{
#if defined(_WIN32)
    return GetFileAttributesW(wstring().c_str()) != INVALID_FILE_ATTRIBUTES;
#else
    struct stat sb;
    return stat(string().c_str(), &sb) == 0;
#endif
}

size_t FSpath::file_size() const
{
#if defined(_WIN32)
    struct _stati64 sb;
    if (_wstati64(wstring().c_str(), &sb) != 0)
        throw std::runtime_error("FSpath::file_size(): cannot stat file \"" + string() + "\"!");
#else
    struct stat sb;
    if (stat(string().c_str(), &sb) != 0)
        throw std::runtime_error("FSpath::file_size(): cannot stat file \"" + string() + "\"!");
#endif
    return (size_t) sb.st_size;
}

bool FSpath::is_directory() const
{
#if defined(_WIN32)
    DWORD result = GetFileAttributesW(wstring().c_str());
    if (result == INVALID_FILE_ATTRIBUTES)
        return false;
    return (result & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
    struct stat sb;
    if (stat(string().c_str(), &sb))
        return false;
    return S_ISDIR(sb.st_mode);
#endif
}

bool FSpath::is_file() const
{
#if defined(_WIN32)
    DWORD attr = GetFileAttributesW(wstring().c_str());
    return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
#else
    struct stat sb;
    if (stat(string().c_str(), &sb))
        return false;
    return S_ISREG(sb.st_mode);
#endif
}

std::string FSpath::extension() const
{
    const std::string &name = filename();
    size_t pos = name.find_last_of(".");
    if (pos == std::string::npos)
        return {};
    return name.substr(pos);
}

std::string FSpath::basename() const
{
    if (empty())
        return {};

    std::string str = filename();
    auto dot = str.rfind('.');

    if (dot == std::string::npos)
        return str;

    return str.substr(0, dot);
}

std::string FSpath::filename() const
{
    if (empty())
        return {};

    const std::string &last = m_path[m_path.size()-1];

    return last;
}

FSpath FSpath::parent_path() const
{
    FSpath result;
    result.m_absolute = m_absolute;

    if (m_path.empty()) {
        if (!m_absolute)
            result.m_path.push_back("..");
    } else {
        size_t until = m_path.size() - 1;
        for (size_t i = 0; i < until; ++i)
            result.m_path.push_back(m_path[i]);
    }
    return result;
}

FSpath FSpath::operator/(const FSpath &other) const
{
    if (other.m_absolute)
        throw std::runtime_error(
            "FSpath::operator/(): expected a relative path!");
    if (m_type != other.m_type)
        throw std::runtime_error(
            "FSpath::operator/(): expected a path of the same type!");

    FSpath result(*this);

    for (size_t i=0; i<other.m_path.size(); ++i)
        result.m_path.push_back(other.m_path[i]);

    return result;
}

FSpath& FSpath::operator/=(const FSpath &other)
{
    if (other.m_absolute)
        throw std::runtime_error(
            "FSpath::operator/(): expected a relative path!");

    std::copy(other.m_path.begin(), other.m_path.end(),
              std::back_inserter(m_path));

    return *this;
}

std::string FSpath::string(path_type type) const
{
    std::ostringstream oss;

    if (m_type == posix_path && m_absolute)
        oss << "/";

    for (size_t i=0; i<m_path.size(); ++i) {
        oss << m_path[i];
        if (i+1 < m_path.size()) {
            if (type == posix_path)
                oss << '/';
            else
                oss << '\\';
        }
    }

    return oss.str();
}

void FSpath::set(const std::string &str, path_type type)
{
    m_type = type;
    if (type == windows_path) {
        m_path = tokenize(str, "/\\");
        m_absolute = str.size() >= 2 && std::isalpha(str[0]) && str[1] == ':';
    } else {
        m_path = tokenize(str, "/");
        m_absolute = !str.empty() && str[0] == '/';
    }
}

FSpath &FSpath::operator=(const FSpath &path)
{
    m_type = path.m_type;
    m_path = path.m_path;
    m_absolute = path.m_absolute;
    return *this;
}

FSpath &FSpath::operator=(FSpath &&path)
{
    if (this != &path) {
        m_type = path.m_type;
        m_path = std::move(path.m_path);
        m_absolute = path.m_absolute;
    }
    return *this;
}

bool FSpath::remove() const
{
    if (is_file()) {
#if !defined(_WIN32)
        return std::remove(string().c_str()) == 0;
#else
        return DeleteFileW(wstring().c_str()) != 0;
#endif
    } else {
#if !defined(_WIN32)
        return rmdir(string().c_str()) == 0;
#else
        return _rmdir(string().c_str()) == 0;
#endif
    }
}

bool FSpath::create_directory() const
{
#if defined(_WIN32)
    return ::CreateDirectoryW(wstring().c_str(), nullptr) != 0;
#else
    return ::mkdir(string().c_str(), S_IRUSR | S_IWUSR | S_IXUSR) == 0;
#endif
}

bool FSpath::create_directories() const
{
    if (is_absolute()) {
        FSpath tmp(*this);

        while (not tmp.empty()) {
            tmp.m_path.pop_back();
            if (tmp.exists())
                break;
        }

        if (not tmp.empty()) {
            auto sz = m_path.size() - tmp.m_path.size();
            while (sz > 0) {
                tmp.m_path.emplace_back(m_path[tmp.m_path.size()]);
                if (not tmp.create_directory())
                    return false;
                sz--;
            }
        }
    } else {
        FSpath tmp;

        for (auto it = cbegin(), end = cend(); it != end; ++it) {
            tmp /= *it;
            if (not tmp.exists())
                if (not tmp.create_directory())
                    return false;
        }
    }

    return true;
}

FSpath FSpath::current_path()
{
#if !defined(_WIN32)
    char temp[PATH_MAX];
    if (::getcwd(temp, PATH_MAX) == NULL)
        throw std::runtime_error("Internal error in getcwd(): " + std::string(strerror(errno)));
    return FSpath(temp);
#else
    std::wstring temp(MAX_PATH, '\0');
    if (!_wgetcwd(&temp[0], MAX_PATH))
        throw std::runtime_error("Internal error in getcwd(): " + std::to_string(GetLastError()));
    return FSpath(temp.c_str());
#endif
}

bool FSpath::current_path(const FSpath& p)
{
#if !defined(_WIN32)
    return ::chdir(p.string().c_str()) == 0;
#else
    return ::_chdir(p.string().c_str()) == 0;
#endif
}

FSpath FSpath::temp_directory_path()
{
    FSpath path;

#if !defined(_WIN32)
    static std::array<const char *, 4> ISO9945_path = { "TMPDIR", "TMP", "TEMP",
                                                        "TEMPDIR" };

    for (auto var : ISO9945_path) {
        const char *result = std::getenv(var);
        path = result;

        if (path.is_directory())
            return path;
    }

    path = "/tmp";
#else
    const DWORD nBufferLength = MAX_PATH + 1;
    char buffer[nBufferLength];

    auto result = GetTempPath(nBufferLength, static_cast<LPSTR>(&buffer[0]));
    if (result > 0) {
        buffer[result] = '\0';
        path = buffer;
    }
#endif

    return path;
}

FSpath FSpath::unique_path(const std::string& model)
{
    std::mt19937 rng;

    rng.seed(std::random_device()());
    std::uniform_int_distribution<
        std::mt19937::result_type> distribution(0, 16);

    for (;;) {
        std::string format(model);
        for (auto& car : format) {
            if (car == '%') {
                auto v = distribution(rng);
                if (v < 10)
                    car = '0' + v;
                else
                    car = 'a' + v - 10;
            }
        }

        FSpath path(format);
        if (not path.exists())
            return FSpath(format);
    }
}

void FSpath::copy_file(const FSpath& from, const FSpath& to)
{
    std::ifstream src(from.string(), std::ios::binary);
    if (not src)
        throw std::runtime_error("Copy file: fail to open source file");

    std::ofstream  dst(to.string(),   std::ios::binary);
    if (not dst)
        throw std::runtime_error("Copy file: fail to open destination file");

    dst << src.rdbuf();

    if (not src.good() or not dst.good())
        throw std::runtime_error("Copy file: error during copy");
}

#if defined(_WIN32)
std::wstring FSpath::wstring(path_type type) const
{
    std::string temp = string(type);
    int size = MultiByteToWideChar(CP_UTF8, 0, &temp[0],
                                   (int)temp.size(), NULL, 0);
    std::wstring result(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), &result[0],
                        size);
    return result;
}

void FSpath::set(const std::wstring &wstring, path_type type)
{
    std::string string;
    if (!wstring.empty()) {
        int size = WideCharToMultiByte(CP_UTF8, 0, &wstring[0],
                                       (int)wstring.size(),
                                       NULL, 0, NULL, NULL);
        string.resize(size, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstring[0], (int)wstring.size(),
                            &string[0], size, NULL, NULL);
    }
    set(string, type);
}

FSpath &FSpath::operator=(const std::wstring &str)
{
    set(str);
    return *this;
}
#endif

bool operator==(const FSpath &p, const FSpath &q)
{
    return p.m_path == q.m_path;
}

bool operator!=(const FSpath &p, const FSpath &q)
{
    return p.m_path != q.m_path;
}

bool operator<(const FSpath &p, const FSpath &q)
{
    const auto max = std::min(p.m_path.size(), q.m_path.size());

    for (auto i = decltype(max) {0}; i != max; ++i)
        if (p.m_path[i] != q.m_path[i])
            return p.m_path[i] < q.m_path[i];

    return p.m_path.size() < q.m_path.size();
}

bool FSpath::is_directory(const FSpath& p)
{
    return p.is_directory();
}

bool FSpath::is_file(const FSpath& p)
{
    return p.is_file();
}

bool FSpath::rename(const FSpath& from, const FSpath& to)
{
    if (not from.exists())
        return false;

    if (to.exists())
        return false;

    return std::rename(from.string().c_str(), to.string().c_str()) == 0;
}

bool FSpath::create_directory(const FSpath& p)
{
    return p.create_directory();
}

bool FSpath::create_directories(const FSpath& p)
{
    return p.create_directories();
}

//
// FSdirectory_entry, FSdirectory_iterator implementation
//

FSdirectory_entry::FSdirectory_entry()
    : m_path()
    , m_is_file(false)
    , m_is_directory(false)
{}

const FSpath& FSdirectory_entry::path() const
{
    return m_path;
}

bool FSdirectory_entry::is_file() const
{
    return m_is_file;
}

bool FSdirectory_entry::is_directory() const
{
    return m_is_directory;
}

struct FSdirectory_iterator::Pimpl
{
    enum Type { REGULAR, DIRECTORY, UNKNOWN };

#if defined(_WIN32)
    HANDLE hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA ffd;
#else
    DIR *m_directory;
#endif
    FSpath m_path;
    FSdirectory_entry m_entry;
    bool m_finish;

public:
    Pimpl(const FSpath& path)
#if defined(_WIN32)
        : hFind(INVALID_HANDLE_VALUE)
        , m_path(path)
        , m_finish(false)
    {
        FSpath spec = m_path / "*.*";

        hFind = FindFirstFile(spec.string().c_str(), &ffd);
        if (hFind == INVALID_HANDLE_VALUE)
            throw std::runtime_error(
                "FindFirstFile: Path does not exist or could not be read.");

        do {
            if (strcmp(ffd.cFileName, ".") != 0 or
                strcmp(ffd.cFileName, "..") != 0) {
                continue;
            } else {
                m_entry.m_path = m_path / ffd.cFileName;
                m_entry.m_is_file = ffd.dwFileAttributes & FILE_ATTRIBUTE_NORMAL;
                m_entry.m_is_directory = ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
                return;
            }
        } while (FindNextFile(hFind, &ffd) != 0);

        m_finish = true;
    }
#else
        : m_directory(nullptr)
        , m_path(path)
        , m_finish(false)
    {
        m_directory = opendir(m_path.string().c_str());
        if (m_directory == nullptr)
            throw std::runtime_error(
                "opendir: Path does not exist or could not be read.");

        next();
    }
#endif

    void next()
    {
        assert(not m_finish);
#if defined(_WIN32)
        while (FindNextFile(hFind, &ffd) != 0) {
            if (strcmp(ffd.cFileName, ".") != 0 or
                strcmp(ffd.cFileName, "..") != 0) {
                continue;
            } else {
                m_entry.m_path = m_path / ffd.cFileName;
                m_entry.m_is_file = ffd.dwFileAttributes & FILE_ATTRIBUTE_NORMAL;
                m_entry.m_is_directory = ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
                return;
            }
        }

        m_finish = true;
#else
        for (;;) {
            struct dirent *entry = readdir(m_directory);
            if (entry == nullptr) {
                m_finish = true;
                return;
            }

            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
                continue;

            m_entry.m_path = m_path / entry->d_name;
            m_entry.m_is_file = entry->d_type == DT_REG;
            m_entry.m_is_directory = entry->d_type == DT_DIR;
            return;
        }
#endif
    }

    ~Pimpl() noexcept
    {
#if defined(_WIN32)
        FindClose(hFind);
        hFind = INVALID_HANDLE_VALUE;
#else
        assert(m_directory);
        closedir(m_directory);
#endif
    }
};;

FSdirectory_iterator::FSdirectory_iterator()
    : m_pimpl()
{}

FSdirectory_iterator::FSdirectory_iterator(const FSpath& p)
    : m_pimpl(std::make_shared<FSdirectory_iterator::Pimpl>(p))
{}

FSdirectory_iterator::~FSdirectory_iterator() = default;

FSdirectory_iterator& FSdirectory_iterator::operator++()
{
    if (not m_pimpl)
        return *this;

    m_pimpl->next();

    if (m_pimpl->m_finish)
        m_pimpl.reset();

    return *this;
}

FSpath FSdirectory_iterator::operator*() const
{
    if (not m_pimpl)
        throw std::runtime_error("null pointer dereference");

    return m_pimpl->m_entry.m_path;
}

FSdirectory_entry* FSdirectory_iterator::operator->() const
{
    if (not m_pimpl)
        throw std::runtime_error("null pointer dereference");

    return &m_pimpl->m_entry;
}

void swap(FSdirectory_iterator& lhs,
        FSdirectory_iterator& rhs)
{
    std::swap(lhs.m_pimpl, rhs.m_pimpl);
}

bool operator==(const FSdirectory_iterator& lhs,
        const FSdirectory_iterator& rhs)
{
    return lhs.m_pimpl == rhs.m_pimpl;
}

bool operator!=(const FSdirectory_iterator& lhs,
        const FSdirectory_iterator& rhs)
{
    return lhs.m_pimpl != rhs.m_pimpl;
}

}} // namespace vle::utils
