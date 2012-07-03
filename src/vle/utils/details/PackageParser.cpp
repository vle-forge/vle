/*
 * @file vle/utils/details/PackageParser.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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

#include <vle/utils/details/PackageParser.hpp>
#include <vle/utils/details/Package.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Trace.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/cast.hpp>
#include <fstream>
#include <cassert>

namespace vle { namespace utils {

namespace ba = boost::algorithm;

struct isTrimmedStringEmpty
    : std::unary_function < std::string, bool >
{
    bool operator()(const std::string& str) const
    {
        return ba::trim_copy(str).empty();
    }
};

static bool stringToPackageOperator(const std::string& str,
                                    PackageOperatorType  *type)
{
    if (str == "=") {
        *type = PACKAGE_OPERATOR_EQUAL;
    } else if (str == "<") {
        *type = PACKAGE_OPERATOR_LESS;
    } else if (str == "<=") {
        *type = PACKAGE_OPERATOR_LESS_OR_EQUAL;
    } else if (str == ">") {
        *type = PACKAGE_OPERATOR_GREATER;
    } else if (str == ">=") {
        *type = PACKAGE_OPERATOR_GREATER_OR_EQUAL;
    } else {
        return false;
    }

    return true;
}

static bool isName(const std::string& line)
{
    return ba::starts_with(line, "Package:");
}

static bool isVersion(const std::string& line)
{
    return ba::starts_with(line, "Version:");
}

static bool isDepends(const std::string& line)
{
    return ba::starts_with(line, "Depends:");
}

static bool isBuildDepends(const std::string& line)
{
    return ba::starts_with(line, "Build-Depends:");
}

static bool isConflicts(const std::string& line)
{
    return ba::starts_with(line, "Conflicts:");
}

static bool isMaintainer(const std::string& line)
{
    return ba::starts_with(line, "Maintainer:");
}

static bool isDescription(const std::string& line)
{
    return ba::starts_with(line, "Description:");
}

static bool isEndDescription(const std::string& line)
{
    return line == " .";
}

static bool isTags(const std::string& line)
{
    return ba::starts_with(line, "Tags:");
}

static bool isUrl(const std::string& line)
{
    return ba::starts_with(line, "Url:");
}

static bool isSize(const std::string& line)
{
    return ba::starts_with(line, "Size:");
}

static bool isMD5sum(const std::string& line)
{
    return ba::starts_with(line, "MD5sum:");
}

static std::string getName(const std::string& name)
{
    std::string tmp = ba::trim_copy(name);

    if (tmp.empty()) {
        throw utils::ArgError(fmt(_("RemotePackage: empty name")));
    } else {
        return tmp;
    }
}

static boost::tuple < int32_t, int32_t, int32_t >
getVersion(const std::string& line)
{
    using boost::lexical_cast;

    std::string tmp = ba::trim_copy(line);

    try {
        std::string::size_type major = tmp.find('.');

        if (major != std::string::npos) {
            std::string::size_type minor = tmp.find('.', major + 1);

            if (minor != std::string::npos) {
                int32_t ma, mi, pa;

                ma = lexical_cast < int32_t  >(
                    std::string(tmp, 0, major));
                mi = lexical_cast < int32_t >(
                    std::string(tmp, major + 1, minor - (major + 1)));
                pa = lexical_cast < int32_t >(
                    std::string(tmp, minor + 1));

                return boost::tuple < int32_t, int32_t, int32_t >(ma, mi, pa);
            }
        }
    } catch (const std::exception& /*e*/) {
        throw utils::InternalError(
            fmt(_("Can not convert version `%1%'")) % tmp);
    }

    throw utils::InternalError(fmt(_("Can convert version `%1%'")) % tmp);
}

struct GetDepend
    : public std::unary_function < std::string, void >
{
    PackagesLinkId *dep;

    GetDepend(PackagesLinkId *dep) : dep(dep) {}

    void operator()(std::string depend)
    {
        ba::trim(depend);

        std::string::size_type para = depend.find("(");
        std::string name(depend, 0, para);
        std::string opera;
        int32_t major = -1;
        int32_t minor = -1;
        int32_t patch = -1;
        ba::trim(name);

        if (para != std::string::npos) {
            std::string version(depend, para, std::string::npos);

            if (version.size() > 2) {
                version.assign(version, 1, version.size() - 2);

                std::string::size_type op = version.find(" ");
                if (op != std::string::npos) {
                    opera.assign(version, 0, op);
                    ba::trim(opera);

                    std::string vers(version, op + 1, std::string::npos);
                    ba::trim(vers);

                    std::vector < std::string > vec;

                    ba::split(vec, vers, ba::is_any_of("."),
                              ba::token_compress_on);

                    if (vec.size() >= 1) {
                        using boost::lexical_cast;

                        major = lexical_cast < int32_t >(vec[0].c_str());
                        if (vec.size() >= 2) {
                            minor = lexical_cast < int32_t >(vec[1].c_str());
                            if (vec.size() >= 3) {
                                patch = lexical_cast < int32_t >(vec[2].c_str());
                            }
                        }
                    }
                }
            }
        }

        PackageLinkId tmp;

        tmp.name = name;

        if (stringToPackageOperator(opera, &tmp.op)) {
            tmp.major = major;
            tmp.minor = minor;
            tmp.patch = patch;

            dep->push_back(tmp);
        }
    }
};

static void getDepends(const std::string& line,
                       PackagesLinkId *depends)
{
    std::vector < std::string > strings(20);

    ba::split(strings,
              line,
              ba::is_any_of(","),
              ba::token_compress_on);

    depends->clear();
    depends->reserve(strings.size());

    std::for_each(strings.begin(), strings.end(),
                  GetDepend(depends));
}

static std::string getMaintainer(const std::string& maintainer)
{
    std::string tmp = ba::trim_copy(maintainer);

    if (tmp.empty()) {
        throw utils::ArgError(fmt(_("RemotePackage: empty maintainer")));
    }

    return tmp;
}

static std::string getDescription(const std::string& description)
{
    std::string tmp = ba::trim_copy(description);

    if (tmp.empty()) {
        throw utils::ArgError(fmt(_("RemotePackage: empty description")));
    }

    return tmp;
}

static std::string appendDesciption(const std::string& description)
{
    return ba::trim_copy(description);
}


static void getTags(const std::string& line,
                    std::vector < std::string > *tags)
{
    std::vector < std::string > strings;

    ba::split(strings,
              line,
              ba::is_any_of(","),
              ba::token_compress_on);

    tags->reserve(strings.size());
    tags->clear();

    std::remove_copy_if(strings.begin(),
                        strings.end(),
                        std::back_inserter(*tags),
                        std::not1(isTrimmedStringEmpty()));
}

static std::string getUrl(const std::string& url)
{
    std::string tmp = ba::trim_copy(url);

    if (tmp.empty()) {
        throw utils::ArgError(fmt(_("RemotePackage: empty url")));
    }

    return tmp;
}

static uint64_t getSize(const std::string& size)
{
    std::string tmp = ba::trim_copy(size);

    try {
        return boost::lexical_cast < uint64_t >(tmp);
    } catch (const std::exception& /*e*/) {
        throw utils::InternalError(
            fmt(_("Can not convert size `%1%'")) % size);
    }
}

static std::string getMD5sum(const std::string& md5sum)
{
    std::string tmp = ba::trim_copy(md5sum);

    if (tmp.empty()) {
        throw utils::ArgError(fmt(_("RemotePackage: empty md5sum")));
    }

    return tmp;
}

static bool fillPackage(std::istream& in,
                        PackageId *pkg)
{
    std::string line;

    try {
        if (std::getline(in, line)) {
            if (isName(line)) {
                pkg->name = getName(std::string(line, 8));
            }
        }

        if (std::getline(in, line)) {
            if (isVersion(line)) {
                boost::tuple < int32_t, int32_t, int32_t > r;
                r = getVersion(std::string(line, 8));

                pkg->major = r.get < 0 >();
                pkg->minor = r.get < 1 >();
                pkg->patch = r.get < 2 >();
            }
        }

        if (std::getline(in, line)) {
            if (isDepends(line)) {
                getDepends(std::string(line, 8), &pkg->depends);
            }
        }

        if (std::getline(in, line)) {
            if (isBuildDepends(line)) {
                getDepends(std::string(line, 8), &pkg->builddepends);
            }
        }

        if (std::getline(in, line)) {
            if (isConflicts(line)) {
                getDepends(std::string(line, 8), &pkg->conflicts);
            }
        }

        if (std::getline(in, line)) {
            if (isMaintainer(line)) {
                pkg->maintainer = getMaintainer(std::string(line, 11));
            }
        }

        if (std::getline(in, line)) {
            if (isDescription(line)) {
                pkg->description = getDescription(std::string(line, 12));

                while (std::getline(in, line) &&  !isEndDescription(line)) {
                    pkg->description += appendDesciption(line);
                }
            }
        }

        if (std::getline(in, line)) {
            if (isTags(line)) {
                getTags(std::string(line, 5), &pkg->tags);
            }
        }

        if (std::getline(in, line)) {
            if (isUrl(line)) {
                pkg->url = getUrl(std::string(line, 4));
            }
        }

        if (std::getline(in, line)) {
            if (isSize(line)) {
                pkg->size = getSize(std::string(line, 5));
            }
        }

        if (std::getline(in, line)) {
            if (isMD5sum(line)) {
                pkg->md5sum = getMD5sum(std::string(line, 7));
            }
        }
    } catch (const std::ios_base::failure &e) {
        TraceAlways(vle::fmt("Remote package (i/o) failure: %1%") % e.what());
        return false;
    } catch (const std::exception &e) {
        TraceAlways(vle::fmt("Remote package error: %1%") % e.what());
        return false;
    }

    return in;
}

static bool fillPackagesSet(std::istream& in,
                            const std::string& distribution,
                            PackagesIdSet *pkgs)
{
    std::string line;
    PackageId p;

    PackageIdClear()(p);
    p.distribution = distribution;

    try {
        while (fillPackage(in, &p)) {
            pkgs->insert(p);

            PackageIdClear()(p);
            p.distribution = distribution;
        }
    } catch (const std::ios_base::failure &e) {
        TraceAlways(
            vle::fmt("Remote package (i/o) failure: %1%") % e.what());
    } catch (const std::exception &e) {
        TraceAlways(
            vle::fmt("Remote package error: %1%") % e.what());
    }

    return not pkgs->empty();
}

bool LoadPackages(std::istream&       stream,
                  const std::string&  distribution,
                  PackagesIdSet      *pkgs) throw()
{
    assert(pkgs);

    if (stream) {
        return fillPackagesSet(stream, distribution, pkgs);
    }

    return false;
}

bool LoadPackages(const std::string&  filename,
                  const std::string&  distribution,
                  PackagesIdSet      *pkgs) throw()
{
    assert(pkgs);

    std::ifstream ifs(filename.c_str());

    return LoadPackages(ifs, distribution, pkgs);
}

bool LoadPackage(std::istream&       stream,
                 const std::string&  distribution,
                 PackageId          *pkg) throw()
{
    assert(pkg);

    if (stream) {
        PackageIdClear()(*pkg);
        pkg->distribution = distribution;

        return fillPackage(stream, pkg);
    }

    return false;
}

bool LoadPackage(const std::string&  filename,
                 const std::string&  distribution,
                 PackageId          *pkg) throw()
{
    assert(pkg);

    std::ifstream ifs(filename.c_str());

    return LoadPackage(ifs, distribution, pkg);
}

}} // namespace vle utils
