/*
    path.h -- A simple class for manipulating paths on Linux/Windows/Mac OS
    Copyright (c) 2015 Wenzel Jakob <wenzel@inf.ethz.ch>
    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.

    Filesystem.hpp -- A modified path.h with little changes
    Copyright (c) 2016-2018 INRA
    gauthier.quesnel@inra.fr
    - Remove make_absolute() functionality useless for VLE.
    - Add temp_directory_path() and unique_path() for tempory path.
    - Add DirectoryEntry and DirectoryIterator to pass trough repertory.
*/

#ifdef _WIN32
#include <tchar.h>

#define NOMINMAX
#include <Windows.h>

#include <ctime>
#include <vle/utils/details/UtilsWin.hpp>
#endif

#include <array>
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Filesystem.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/i18n.hpp>

#ifndef _WIN32
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef __unix__
#include <fts.h>
#endif

#ifdef __linux
#include <linux/limits.h>
#endif

namespace vle {
namespace utils {

Path::Path() = default;

Path::Path(const Path& /*path*/) = default;

Path::Path(Path&& path)
  : m_path(std::move(path.m_path))
  , m_type(path.m_type)
  , m_absolute(path.m_absolute)
{}

Path::Path(const char* string)
{
    if (string)
        set(string);
}

Path::Path(const std::string& string)
{
    set(string);
}

#if defined(_WIN32)
Path::Path(const std::wstring& wstring)
{
    set(wstring);
}

Path::Path(const wchar_t* wstring)
{
    if (wstring)
        set(wstring);
}
#endif

size_t
Path::length() const
{
    return m_path.size();
}

bool
Path::empty() const
{
    return m_path.empty();
}

void
Path::clear() noexcept
{
    m_path.clear();
}

bool
Path::is_absolute() const
{
    return m_absolute;
}

bool
Path::exists() const
{
#if defined(_WIN32)
    return ::GetFileAttributesW(wstring().c_str()) != INVALID_FILE_ATTRIBUTES;
#else
    struct stat sb;
    return stat(string().c_str(), &sb) == 0;
#endif
}

size_t
Path::file_size() const
{
#if defined(_WIN32)
    struct _stati64 sb;
    if (_wstati64(wstring().c_str(), &sb) != 0)
        throw FileError(_("Path::file_size(): cannot stat file %s"),
                        string().c_str());
#else
    struct stat sb;
    if (stat(string().c_str(), &sb) != 0)
        throw FileError(_("Path::file_size(): cannot stat file %s"),
                        string().c_str());
#endif
    return (size_t)sb.st_size;
}

bool
Path::is_directory() const
{
#if defined(_WIN32)
    DWORD result = ::GetFileAttributesW(wstring().c_str());
    return ((result != INVALID_FILE_ATTRIBUTES) &&
            (result & FILE_ATTRIBUTE_DIRECTORY) != 0);
#else
    struct stat sb;
    if (stat(string().c_str(), &sb))
        return false;
    return S_ISDIR(sb.st_mode);
#endif
}

bool
Path::is_file() const
{
#if defined(_WIN32)
    DWORD result = ::GetFileAttributesW(wstring().c_str());
    return ((result != INVALID_FILE_ATTRIBUTES) &&
            (result & FILE_ATTRIBUTE_DIRECTORY) == 0);
#else
    struct stat sb;
    if (stat(string().c_str(), &sb))
        return false;
    return S_ISREG(sb.st_mode);
#endif
}

std::string
Path::extension() const
{
    const std::string& name = filename();
    size_t pos = name.find_last_of(".");
    if (pos == std::string::npos)
        return {};
    return name.substr(pos);
}

std::string
Path::basename() const
{
    if (empty())
        return {};

    std::string str = filename();
    auto dot = str.rfind('.');

    if (dot == std::string::npos)
        return str;

    return str.substr(0, dot);
}

std::string
Path::filename() const
{
    if (empty())
        return {};

    const std::string& last = m_path[m_path.size() - 1];

    return last;
}

Path
Path::parent_path() const
{
    Path result;
    result.m_absolute = m_absolute;

    if (m_path.empty()) {
        if (!m_absolute)
            result.m_path.emplace_back("..");
    } else {
        size_t until = m_path.size() - 1;
        for (size_t i = 0; i < until; ++i)
            result.m_path.push_back(m_path[i]);
    }
    return result;
}

Path
Path::operator/(const Path& other) const
{
    if (other.m_absolute)
        throw FileError(_("Path::operator/(): expected a relative path"));
    if (m_type != other.m_type)
        throw FileError(
          _("Path::operator/(): expected a path of the same type"));

    Path result(*this);

    for (const auto& i : other.m_path)
        result.m_path.push_back(i);

    return result;
}

Path&
Path::operator/=(const Path& other)
{
    if (other.m_absolute)
        throw FileError(_("Path::operator/(): expected a relative path"));

    for (const auto& elem : other.m_path)
        m_path.push_back(elem);

    return *this;
}

std::string
Path::string(path_type type) const
{
    std::size_t size =
      std::accumulate(m_path.cbegin(),
                      m_path.cend(),
                      static_cast<std::size_t>(0),
                      [](std::size_t size, const std::string& str) {
                          return size + str.size();
                      });

    std::string ret;
    ret.reserve(size + m_path.size());

    if (m_type == posix_path and m_absolute)
        ret += "/";

    for (std::size_t i = 0, e = m_path.size(); i != e; ++i) {
        ret += m_path[i].c_str();

        if ((i + 1) < m_path.size()) {
            if (type == posix_path)
                ret += '/';
            else
                ret += '\\';
        }
    }

    return ret;
}

void
Path::set(const std::string& str, path_type type)
{
    m_type = type;
    if (type == windows_path) {
        m_path.clear();
        tokenize(str, m_path, "/\\", true);
        m_absolute = str.size() >= 2 && std::isalpha(str[0]) && str[1] == ':';
    } else {
        m_path.clear();
        tokenize(str, m_path, "/", true);
        m_absolute = !str.empty() && str[0] == '/';
    }
}

Path&
Path::operator=(const Path& path) = default;

Path&
Path::operator=(Path&& path)
{
    if (this != &path) {
        m_type = path.m_type;
        m_path = std::move(path.m_path);
        m_absolute = path.m_absolute;
    }
    return *this;
}

bool
Path::remove() const
{
    if (is_file()) {
#if !defined(_WIN32)
        return std::remove(string().c_str()) == 0;
#else
        return ::DeleteFileW(wstring().c_str()) != 0;
#endif
    } else {
#if !defined(_WIN32)
        return rmdir(string().c_str()) == 0;
#else
        return ::RemoveDirectoryW(wstring().c_str());
#endif
    }
}

bool
Path::create_directory() const
{
#if defined(_WIN32)
    if (::CreateDirectoryW(wstring().c_str(), nullptr) == 0) {
#else
    if (::mkdir(string().c_str(), S_IRUSR | S_IWUSR | S_IXUSR)) {
#endif
        return false;
    }

    return true;
}

bool
Path::create_directories() const
{
    if (is_absolute()) {
        Path tmp(*this);

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
        Path tmp;

        for (const auto& it : *this) {
            tmp /= it;
            if (not tmp.exists())
                if (not tmp.create_directory())
                    return false;
        }
    }

    return true;
}

Path
Path::current_path()
{
#if !defined(_WIN32)
    char temp[PATH_MAX];
    if (::getcwd(temp, PATH_MAX) == nullptr)
        throw FileError(_("Internal error in getcwd(): %s"), strerror(errno));
    return Path(temp);
#else
    std::wstring buffer(MAX_PATH, '\0');
    DWORD ret =
      ::GetCurrentDirectoryW(static_cast<DWORD>(buffer.size()), &buffer[0]);

    while (ret > buffer.size()) {
        buffer.resize(ret, '\0');
        ret = ::GetCurrentDirectoryW(ret, &buffer[0]);
    }

    if (ret == 0)
        throw FileError(_("Internal error in getcwd(): %lu"), GetLastError());

    buffer.resize(ret);
    return Path(buffer);
#endif
}

bool
Path::current_path(const Path& p)
{
#if !defined(_WIN32)
    return ::chdir(p.string().c_str()) == 0;
#else
    return ::SetCurrentDirectoryW(p.wstring().c_str());
#endif
}

Path
Path::temp_directory_path()
{
#if !defined(_WIN32)
    static std::array<const char*, 4> ISO9945_path{
        { "TMPDIR", "TMP", "TEMP", "TEMPDIR" }
    };

    for (auto var : ISO9945_path) {
        const char* result = std::getenv(var);
        if (not result)
            continue;

        Path path = result;
        if (path.is_directory())
            return path;
    }

    return { "/tmp" };
#else
    std::wstring buffer(MAX_PATH, '\0');
    auto ret = ::GetTempPathW(static_cast<DWORD>(buffer.size()), &buffer[0]);

    while (ret > buffer.size()) {
        buffer.resize(ret, '\0');
        ret = ::GetTempPathW(static_cast<DWORD>(buffer.size()), &buffer[0]);
    }

    if (ret == 0)
        return Path(L"C:\\Windows\\Temp");

    buffer.resize(ret);
    return Path(buffer);
#endif
}

Path
Path::unique_path(const std::string& model)
{
    std::mt19937 rng;
#if defined _WIN32
    rng.seed(static_cast<unsigned int>(std::time(0)));
#else
    rng.seed(std::random_device{}());
#endif
    std::uniform_int_distribution<std::mt19937::result_type> distribution(0,
                                                                          16);

    for (;;) {
        std::string format(model);
        for (auto& car : format) {
            if (car == '%') {
                auto v = distribution(rng);
                if (v < 10)
                    car = static_cast<char>(v + '0');
                else
                    car = static_cast<char>(v + 'a' - 10);
            }
        }

        Path path(format);
        if (not path.exists())
            return Path(format);
    }
}

void
Path::copy_file(const Path& from, const Path& to)
{
    std::ifstream src(from.string(), std::ios::binary);
    if (not src)
        throw FileError(_("Copy file: fail to open source file"));

    std::ofstream dst(to.string(), std::ios::binary);
    if (not dst)
        throw FileError(_("Copy file: fail to open destination file"));

    dst << src.rdbuf();

    if (not src.good() or not dst.good())
        throw FileError(_("Copy file: error during copy"));
}

#if defined(_WIN32)
std::wstring
Path::wstring(path_type type) const
{
    return from_utf8_to_wide(string(type));
}

void
Path::set(const std::wstring& wstring, path_type type)
{
    set(from_wide_to_utf8(wstring), type);
}

#endif

bool
operator==(const Path& p, const Path& q)
{
    return p.m_path == q.m_path;
}

bool
operator!=(const Path& p, const Path& q)
{
    return p.m_path != q.m_path;
}

bool
operator<(const Path& p, const Path& q)
{
    const auto max = std::min(p.m_path.size(), q.m_path.size());

    for (auto i = decltype(max){ 0 }; i != max; ++i)
        if (p.m_path[i] != q.m_path[i])
            return p.m_path[i] < q.m_path[i];

    return p.m_path.size() < q.m_path.size();
}

bool
Path::is_directory(const Path& p)
{
    return p.is_directory();
}

bool
Path::is_file(const Path& p)
{
    return p.is_file();
}

bool
Path::rename(const Path& from, const Path& to)
{
    if (not from.exists())
        return false;

    if (to.exists())
        return false;

    return std::rename(from.string().c_str(), to.string().c_str()) == 0;
}

bool
Path::create_directory(const Path& p)
{
    return p.create_directory();
}

bool
Path::create_directories(const Path& p)
{
    return p.create_directories();
}

UnlinkPath::UnlinkPath(const Path& path_to_delete)
  : m_unlink_path(path_to_delete)
{}

UnlinkPath::~UnlinkPath() noexcept
{
    try {
        if (m_unlink_path.exists())
            m_unlink_path.remove();
    } catch (...) {
    }
}

Path
UnlinkPath::path() const
{
    return m_unlink_path;
}

std::string
UnlinkPath::string() const
{
    return m_unlink_path.string();
}

//
// DirectoryEntry, DirectoryIterator implementation
//

DirectoryEntry::DirectoryEntry()
  : m_path()
{}

const Path&
DirectoryEntry::path() const
{
    return m_path;
}

bool
DirectoryEntry::is_file() const
{
    return m_is_file;
}

bool
DirectoryEntry::is_directory() const
{
    return m_is_directory;
}

struct DirectoryIterator::Pimpl
{
    enum Type
    {
        REGULAR,
        DIRECTORY,
        UNKNOWN
    };

#if defined(_WIN32)
    HANDLE hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW ffd;
#else
    DIR* m_directory;
#endif
    Path m_path;
    DirectoryEntry m_entry;
    bool m_finish;

public:
    Pimpl(Path path)
#if defined(_WIN32)
      : hFind(INVALID_HANDLE_VALUE)
      , m_path(path)
      , m_finish(false)
    {
        Path spec = m_path / "*.*";

        hFind = FindFirstFileW(spec.wstring().c_str(), &ffd);
        if (hFind == INVALID_HANDLE_VALUE)
            throw FileError(
              _("DirectoryEntry: Path does not exist or could not be read."));

        do {
            std::string filename(from_tchar(ffd.cFileName));

            if (filename != "." and filename != "..") {
                m_entry.m_path = m_path / filename;
                m_entry.m_is_directory =
                  ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
                m_entry.m_is_file = not m_entry.m_is_directory;
                return;
            }
        } while (FindNextFileW(hFind, &ffd) != 0);

        m_finish = true;
    }
#else
      : m_directory(nullptr)
      , m_path(std::move(path))
      , m_finish(false)
    {
        m_directory = opendir(m_path.string().c_str());
        if (m_directory == nullptr)
            throw FileError(
              _("DirectoryEntry: Path does not exist or could not be read."));

        next();
    }
#endif

    void next()
    {
        assert(not m_finish);
#if defined(_WIN32)
        while (FindNextFileW(hFind, &ffd) != 0) {
            std::string filename(from_tchar(ffd.cFileName));

            if (filename != "." and filename != "..") {
                m_entry.m_path = m_path / filename;
                m_entry.m_is_directory =
                  ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
                m_entry.m_is_file = not m_entry.m_is_directory;
                return;
            }
        }

        m_finish = true;
#else
        for (;;) {
            struct dirent* result = readdir(m_directory);
            if (result == nullptr) {
                m_finish = true;
                return;
            }

            if (!strcmp(result->d_name, ".") || !strcmp(result->d_name, ".."))
                continue;

            m_entry.m_path = m_path / result->d_name;
            m_entry.m_is_file = result->d_type == DT_REG;
            m_entry.m_is_directory = result->d_type == DT_DIR;
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
};
;

DirectoryIterator::DirectoryIterator()
  : m_pimpl()
{}

DirectoryIterator::DirectoryIterator(const Path& p)
  : m_pimpl(std::make_shared<DirectoryIterator::Pimpl>(p))
{
    if (m_pimpl->m_finish)
        m_pimpl.reset();
}

DirectoryIterator::~DirectoryIterator() = default;

DirectoryIterator&
DirectoryIterator::operator++()
{
    if (not m_pimpl)
        return *this;

    m_pimpl->next();

    if (m_pimpl->m_finish)
        m_pimpl.reset();

    return *this;
}

Path DirectoryIterator::operator*() const
{
    if (not m_pimpl)
        throw FileError(_("DirectoryIterator: null pointer dereference"));

    return m_pimpl->m_entry.m_path;
}

DirectoryEntry* DirectoryIterator::operator->() const
{
    if (not m_pimpl)
        throw FileError(_("DirectoryIterator: null pointer dereference"));

    return &m_pimpl->m_entry;
}

void
swap(DirectoryIterator& lhs, DirectoryIterator& rhs)
{
    std::swap(lhs.m_pimpl, rhs.m_pimpl);
}

bool
operator==(const DirectoryIterator& lhs, const DirectoryIterator& rhs)
{
    return lhs.m_pimpl == rhs.m_pimpl;
}

bool
operator!=(const DirectoryIterator& lhs, const DirectoryIterator& rhs)
{
    return lhs.m_pimpl != rhs.m_pimpl;
}
}
} // namespace vle::utils
