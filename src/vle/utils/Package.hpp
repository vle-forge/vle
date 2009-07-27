/**
 * @file vle/utils/Package.hpp
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


#ifndef VLE_UTILS_PACKAGE_HPP
#define VLE_UTILS_PACKAGE_HPP

#include <vle/utils/Path.hpp>
#include <string>
#include <list>

namespace vle { namespace utils {

    class CMakePackage
    {
    public:
        CMakePackage(const std::string& package);

        static void create(std::string& out, std::string& err);

        static void configure(std::string& out, std::string& err);

        static void build(std::string& out, std::string& err);

        static void clean(std::string& out, std::string& err);

        static void package(std::string& out, std::string& err);

        void setDebugMode() { m_type = Debug; }
        void setReleaseMode() { m_type = Release; }
        void setProcess(unsigned int nb) { m_process = nb; }

        static PathList getInstalledPackages();
        static PathList getInstalledExperiments();
        static PathList getInstalledLibraries();

	static void addFile(const std::string& path,
			    const std::string& name);
	static void addDirectory(const std::string& path,
				 const std::string& name);
	static void removeFile(const std::string& pathFile);
	static void renameFile(const std::string& oldFile,
			       std::string& newName);

    private:
        enum Type { Debug, Release };

        Type m_type;
        unsigned int m_process;

        std::string m_out;
        std::string m_err;
    };

}} // namespace vle utils

#endif
