/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2012 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
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

#include <boost/filesystem.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/utils/Exception.hpp>
#include <ostream>
#include <fstream>
#include <archive.h>
#include <archive_entry.h>
#include <fcntl.h>

namespace vle { namespace utils {

static std::string extract_archive_error(const char *filepath,
                                         const char *tarfile,
                                         struct archive *a)
{
    if (a) {
        return std::string((fmt(_("decompress failure: `%1%' in %2% "
                                  "failed: %3%"))
                            % filepath % tarfile
                            % archive_error_string(a)).str());
    } else {
        return std::string((fmt(_("decompress failure: `%1%' in %2%"))
                            % filepath % tarfile).str());
    }
}

static std::string create_archive_error(const char *filepath,
                                        const char *tarfile,
                                        struct archive *a)
{
    if (a) {
        return std::string((fmt(_("compress failure: `%1%' in %2% failed: %3%"))
                            % filepath % tarfile
                            % archive_error_string(a)).str());
    } else {
        return std::string((fmt(_("compress failure: `%1%' in %2%"))
                            % filepath % tarfile).str());
    }
}

static int copy_data(struct archive *ar, struct archive *aw)
{
    int r;
    const void *buff;
    size_t size;
    int64_t offset;

    for (;;) {
        r = archive_read_data_block(ar, &buff, &size, &offset);
        if (r == ARCHIVE_EOF) {
            return ARCHIVE_OK;
        }

        if (r != ARCHIVE_OK) {
            return r;
        }

        r = archive_write_data_block(aw, buff, size, offset);

        if (r != ARCHIVE_OK) {
            return r;
        }
    }

    return ARCHIVE_OK;
}

static void extract_archive(const char *filename, const char *output)
{
    struct archive *a;
    struct archive *ext;
    struct archive_entry *entry;
    int flags;
    int r;

    flags = ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_SECURE_NODOTDOT;

    a = archive_read_new();
    archive_read_support_filter_bzip2(a);
    archive_read_support_format_tar(a);

    ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, flags);
    archive_write_disk_set_standard_lookup(ext);

    r = archive_read_open_file(a, filename, 10240);

    if (r) {
        std::string msg = extract_archive_error(filename, output, a);

	archive_read_close(a);
	archive_read_free(a);

        throw utils::InternalError(msg);
    }

    for (;;) {
        r = archive_read_next_header(a, &entry);
        if (r == ARCHIVE_EOF) {
            break;
        }

        if (r != ARCHIVE_OK) {
            std::string msg = extract_archive_error(filename, output, a);

            archive_read_close(a);
            archive_read_free(a);
            archive_write_close(ext);
            archive_write_free(ext);

            throw utils::InternalError(msg);
        }

        if (r < ARCHIVE_WARN) {
            break;
        }

        r = archive_write_header(ext, entry);

        if (r != ARCHIVE_OK) {
            std::string msg = extract_archive_error(filename, output, a);

            archive_read_close(a);
            archive_read_free(a);
            archive_write_close(ext);
            archive_write_free(ext);

            throw utils::InternalError(msg);
        } else {
            r = copy_data(a, ext);

            if (r != ARCHIVE_OK) {
                std::string msg = extract_archive_error(filename, output, a);

                archive_read_close(a);
                archive_read_free(a);
                archive_write_close(ext);
                archive_write_free(ext);

                throw utils::InternalError(msg);
            }
        }
    }

    archive_read_close(a);
    archive_read_free(a);
    archive_write_close(ext);
    archive_write_free(ext);
}

static void create_archive(const char *filepath, const char *tarfile)
{
    struct archive *a;
    struct archive *disk;
    struct archive_entry *entry;
    int r;

    a = archive_write_new();    /**< build the output tarball file. */
    archive_write_set_compression_bzip2(a);
    archive_write_set_format_ustar(a);
    archive_write_open_file(a, tarfile);

    disk = archive_read_disk_new(); /**< build the input stream. */
    archive_read_disk_set_standard_lookup(disk);

    r = archive_read_disk_open(disk, filepath);
    if (r != ARCHIVE_OK) {
        std::string msg = create_archive_error(filepath, tarfile, disk);
        throw utils::InternalError(msg);
    }

    for (;;) {
        entry = archive_entry_new();
        r = archive_read_next_header2(disk, entry);

        if (r == ARCHIVE_EOF) {
            break;
        }

        if (r != ARCHIVE_OK) {
            std::string msg = create_archive_error(filepath, tarfile, disk);

            archive_read_close(disk);
            archive_read_free(disk);
            archive_write_close(a);
            archive_write_free(a);

            throw utils::InternalError(msg);
        }

        archive_read_disk_descend(disk);
        r = archive_write_header(a, entry);

        if (r == ARCHIVE_FATAL) {
            std::string msg = create_archive_error(filepath, tarfile, disk);

            archive_entry_free(entry);
            archive_read_close(disk);
            archive_read_free(disk);
            archive_write_close(a);
            archive_write_free(a);

            throw utils::InternalError(msg);
        }

        if (r > ARCHIVE_FAILED) {
            char buff[1024];
#ifdef _WIN32
            int fd = ::_open(archive_entry_sourcepath(entry), O_RDONLY);
            int len = ::_read(fd, buff, sizeof(buff));

            while (len > 0) {
                archive_write_data(a, buff, len);
                len = ::_read(fd, buff, sizeof(buff));
            }

            ::_close(fd);
#else
            int fd = ::open(archive_entry_sourcepath(entry), O_RDONLY);
            int len = ::read(fd, buff, sizeof(buff));

            while (len > 0) {
                archive_write_data(a, buff, len);
                len = ::read(fd, buff, sizeof(buff));
            }

            ::close(fd);
#endif

        }

        archive_entry_free(entry);
    }

    archive_read_close(disk);
    archive_read_free(disk);

    archive_write_close(a);
    archive_write_free(a);
}

void Path::compress(const std::string& filepath,
                    const std::string& compressedfilepath)
{
    namespace fs = boost::filesystem;
    namespace fe = boost::system;

    fs::path path(filepath);

    if (fs::exists(path)) {
        create_archive(filepath.c_str(), compressedfilepath.c_str());
    }

}

void Path::decompress(const std::string& compressedfilepath,
                      const std::string& directorypath)
{

    namespace fs = boost::filesystem;
    namespace fe = boost::system;

    fs::path path(directorypath);

    if (fs::exists(path) and fs::is_directory(path)) {
        fe::error_code ec;
        fs::current_path(path, ec);

        if (fs::current_path() == path) {
            extract_archive(compressedfilepath.c_str(),
                            directorypath.c_str());
        }
    }
}

}}
