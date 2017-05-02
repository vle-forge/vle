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

#include <boost/algorithm/string/trim.hpp>
#include <cassert>
#include <fstream>
#include <vle/utils/Exception.hpp>
#include <vle/utils/details/Package.hpp>
#include <vle/utils/details/PackageParser.hpp>
#include <vle/utils/i18n.hpp>

namespace vle {
namespace utils {

class DescriptionParser
{
    const std::string& filepath;
    std::istream& in;
    unsigned int line;
    unsigned int oldline;
    unsigned int column;
    unsigned int oldcolumn;
    char last;

    char get()
    {
        last = in.get();

        if (last == '\n') {
            oldline = line++;
            oldcolumn = column;
            column = 0;
        } else {
            oldcolumn = column++;
        }

        return last;
    }

    void unget()
    {
        if (last == '\n') {
            line = oldline;
            column = oldcolumn;
        } else {
            column = oldcolumn;
        }

        in.unget();
    }

    bool read_newline()
    {
        while (in and isblank(get()))
            ;

        if (not in)
            return false;

        unget();

        return get() == '\n';
    }

    bool read_blank()
    {
        while (in and isblank(get()))
            ; /* read all space or tab
               * characters */

        if (not in)
            return false;

        unget();

        return true;
    }

    bool read_space()
    {
        while (in and isspace(get()))
            ;

        if (not in)
            return false;

        unget();

        return true;
    }

    bool read_word(const char* str)
    {
        if (not read_blank())
            return false;

        while (in and *str != '\0') {
            if (get() != *str)
                return false;

            ++str;
        }

        return *str == '\0';
    }

    bool read_identifier(std::string* output)
    {
        if (not read_blank())
            return false;

        char c = get();
        if (!isalnum(c)) /* check if the first character is an alphanum
                            character */
            return false;

        output->append(1, c);

        do {
            c = get();

            if (not(isalnum(c) or c == '_' or c == '.' or c == '-'))
                break;

            output->append(1, c);
        } while (in);

        unget();

        return true;
    }

    bool read_integer(int32_t* value)
    {
        if (not read_blank())
            return false;

        std::string str;
        str.reserve(std::numeric_limits<int32_t>::digits10 + 2);

        do {
            char c = get();

            if (not isdigit(c))
                break;

            str.append(1, c);
        } while (in);

        unget();

        try {
            *value = std::stoi(str);
            return true;
        } catch (const std::exception& /* e */) {
            return false;
        }
    }

    bool read_version(int32_t* major, int32_t* minor, int32_t* patch)
    {
        if (not read_blank())
            return false;

        return read_integer(major) and read_word(".") and
               read_integer(minor) and read_word(".") and
               read_integer(patch) and read_newline();
    }

    bool read_operator(PackageOperatorType* type)
    {
        if (not read_blank())
            return false;

        std::string str;
        str.reserve(3);
        char c;

        c = get();

        if (not(c == '=' or c == '<' or c == '>'))
            return false;

        str.append(1, c);

        if (c == '<' or c == '>') {
            c = get();

            if (c == '=' or c == '=')
                str.append(1, c);
            else
                unget();
        }

        *type = (str == "=")
                  ? PACKAGE_OPERATOR_EQUAL
                  : (str == "<")
                      ? PACKAGE_OPERATOR_LESS
                      : (str == "<=")
                          ? PACKAGE_OPERATOR_LESS_OR_EQUAL
                          : (str == ">")
                              ? PACKAGE_OPERATOR_GREATER
                              : (str == ">=")
                                  ? PACKAGE_OPERATOR_GREATER_OR_EQUAL
                                  : PACKAGE_OPERATOR_EQUAL;

        return true;
    }

    bool read_linkid(PackagesLinkId* pkgs)
    {
        if (not read_blank())
            return false;

        if (read_newline()) /* depends, build-depends and
                             * conflicts can be empty. */
            return true;

        unget();

        do {
            PackageLinkId pkg;
            cleanup(pkg);

            if (not read_identifier(&pkg.name))
                return false;

            if (read_word("(")) {
                if (not read_operator(&pkg.op))
                    return false;

                if (not read_integer(&pkg.major))
                    return false;

                if (not read_word(".")) {
                    pkg.minor = -1;
                    pkg.patch = -1;
                } else {
                    if (not read_integer(&pkg.minor))
                        return false;

                    if (not read_word(".")) {
                        pkg.patch = -1;
                        unget();
                    } else {
                        if (not read_integer(&pkg.patch))
                            return false;
                    }
                }

                if (not read_word(")"))
                    return false;
            } else {
                unget();
            }

            pkgs->push_back(pkg);

            if (read_word(",")) {
                continue;
            } else {
                unget();
                if (read_newline()) {
                    break;
                } else {
                    unget();
                }
            }

        } while (in);

        return true;
    }

    bool read_maintainer(std::string* maintainer)
    {
        if (not read_blank())
            return false;

        do {
            char c = get();

            if (c == '\n' or c == '\r')
                break;

            maintainer->append(1, c);
        } while (in);

        boost::trim(*maintainer);

        return not maintainer->empty();
    }

    bool read_description(std::string* description)
    {
        if (not read_blank())
            return false;

        do {
            char c = get();

            if (c == '\n' or c == '\r') {
                if (in) {
                    char c2 = get();
                    if (in and c2 == ' ') {
                        char c3 = get();
                        if (in and c3 == '.') {
                            break;
                        } else {
                            description->append(1, c3);
                        }
                    } else {
                        description->append(1, c2);
                    }
                } else {
                    description->append(1, c);
                }
            }

            description->append(1, c);
        } while (in);

        boost::trim(*description);

        if (not read_newline())
            return false;

        return true;
    }

    bool read_tags(Tags* tags)
    {
        if (not read_blank())
            return false;

        do {
            std::string str;
            str.reserve(32);

            do {
                char c = get();

                if (c == ',' or c == '\n') {
                    if (not str.empty()) {
                        boost::trim(str);
                        if (not str.empty()) {
                            tags->push_back(str);
                        }
                    }

                    if (c == '\n')
                        return true;

                    break;
                }
                str.append(1, c);
            } while (in);
        } while (in);

        return false;
    }

    bool read_size(uint64_t* value)
    {
        if (not read_blank())
            return false;

        std::string str;
        str.reserve(std::numeric_limits<uint64_t>::digits10 + 1);

        do {
            char c = get();

            if (not isdigit(c))
                break;

            str.append(1, c);
        } while (in);

        try {
            *value = std::stol(str);
            return true;
        } catch (const std::exception& /* e */) {
            return false;
        }
    }

    int read_package(PackageId* pkg)
    {
        if (not read_space()) {
            if (in.eof()) {
                return 1;
            }
        }

        if (not(read_word("Package:") and read_identifier(&pkg->name) and
                read_newline()))
            return -1;

        if (not(read_word("Version:") and
                read_version(&pkg->major, &pkg->minor, &pkg->patch)))
            return -2;

        if (not(read_word("Depends:") and read_linkid(&pkg->depends)))
            return -3;

        if (not(read_word("Build-Depends:") and
                read_linkid(&pkg->builddepends)))
            return -4;

        if (not(read_word("Conflicts:") and read_linkid(&pkg->conflicts)))
            return -5;

        if (not(read_word("Maintainer:") and
                read_maintainer(&pkg->maintainer)))
            return -6;

        if (not(read_word("Description:") and
                read_description(&pkg->description)))
            return -7;

        if (not(read_word("Tags:") and read_tags(&pkg->tags)))
            return -8;

        if (not(read_word("Url:") and read_maintainer(&pkg->url)))
            return -9;

        if (not(read_word("Size:") and read_size(&pkg->size)))
            return -10;

        if (not(read_word("MD5sum:") and read_maintainer(&pkg->md5sum)))
            return -11;

        return 0;
    }

public:
    DescriptionParser(std::istream& in, const std::string& filepath)
      : filepath(filepath)
      , in(in)
      , line(0)
      , oldline(0)
      , column(0)
      , oldcolumn(0)
    {
    }

    ~DescriptionParser()
    {
    }

    void read_packages(const std::string& distribution, Packages* pkgs)
    {
        do {
            PackageId p;
            cleanup(p);
            p.distribution = distribution;

            int result = read_package(&p);

            switch (result) {
            case 0:
                pkgs->push_back(p);
                break;
            case 1:
                return;
            default:
                throw FileError(
                  (boost::format(_("Remote: syntax error in file `%1%'"
                                   " l. %2% c. %3%: %4%")) %
                   filepath % line % column % result)
                    .str());
            }
        } while (in);
    }
};

void
PackageParser::extract(const std::string& filepath,
                       const std::string& distribution)
{
    std::ifstream ifs(filepath.c_str());

    if (not ifs)
        throw FileError(
          (boost::format(_("Remote: fail to parse package file '%1%'")) %
           filepath)
            .str());

    DescriptionParser parser(ifs, filepath);
    parser.read_packages(distribution, &m_packages);
}
}
} // namespace vle utils
