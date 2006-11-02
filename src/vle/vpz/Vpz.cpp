/** 
 * @file vpz/Vpz.cpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 28 fév 2006 12:00:10 +0100
 */

/*
 * Copyright (C) 2006 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <vle/vpz/Vpz.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>
#include <zlib.h>
#include <fstream>

namespace vle { namespace vpz {

using namespace vle::utils;

Vpz::Vpz(const std::string& filename) :
    m_filename(filename)
{
    open(filename);
}

void Vpz::open(const std::string& filename)
{
    m_filename.assign(filename);

    std::string xml = get_gzip_content(filename);
    xmlpp::DomParser dom;
    dom.parse_memory(xml);

    xmlpp::Element* root = xml::get_root_node(dom, "VLE_PROJECT");
    m_project.init(root);
}

void Vpz::expandTranslator()
{
    m_project.expandTranslator();
}

bool Vpz::hasNoVLE() const
{
    return m_project.hasNoVLE();
}

void Vpz::write()
{
    xmlpp::Document doc;
    doc.create_root_node("VLE_PROJECT");
    m_project.write(doc.get_root_node());

    doc.write_to_file_formatted(m_filename);
}

void Vpz::write(const std::string& filename)
{
    m_filename.assign(filename);
    write();
}

void Vpz::clear()
{
    m_filename.clear();
    m_project.clear();
    m_isGzip = false;
}

std::string Vpz::writeToString()
{
    xmlpp::Document doc;
    doc.create_root_node("VLE_PROJECT");
    m_project.write(doc.get_root_node());

    return doc.write_to_string_formatted();
}

std::string Vpz::get_gzip_content(const std::string& filename)
{
    const size_t buffer_size = 4096;
    char* buffer = new char[buffer_size];
    int len;
    std::string result;

    gzFile file = gzopen(filename.c_str(), "rb");
    Assert(utils::FileError, file ,
           boost::format("Cannot read file '%1%'\n") % filename);

    for (;;) {
        len = gzread(file, buffer, buffer_size);
        if (len < 0) {
            delete[] buffer;
            Assert(utils::FileError, len >= 0,
                   boost::format("Read error file '%1%'\n") % filename);
        }
        if (len == 0) break;
        result.append(buffer, len);
    }
    delete[] buffer;
    gzclose(file);
    return result;
}

bool Vpz::is_gzip_file(const std::string& filename)
{
    size_t sizefile, sizeGzip;

    {
        std::ifstream f(filename.c_str());
        Assert(utils::FileError, f.is_open(),
               boost::format("Cannot open file '%1%'\n") % filename);

        f.seekg(0, std::ios_base::end);
        sizefile = f.tellg();
        f.close();
    }
    {
        gzFile file = gzopen(filename.c_str(), "rb");
        Assert(utils::FileError, file ,
               boost::format("Cannot read file '%1%'\n") % filename);
        sizeGzip = gzseek(file, 0, SEEK_END);
        gzclose(file);
    }

    return sizeGzip < sizefile;
}

}} // namespace vle vpz
