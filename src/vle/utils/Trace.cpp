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


#include <iostream>
#include <fstream>
#include <locale>
#include <vle/utils/Trace.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/DateTime.hpp>
#include <vle/utils/Filesystem.hpp>
#include <mutex>
#include <cstdio>
#include <cstdarg>


namespace vle { namespace utils {

/*
 * The utils::Trace::Pimpl source.
 */

struct Trace::Pimpl
{
    void cleanup__()
    {
        if (mType == TRACE_STREAM_FILE) {
            delete mStream;
            mStream = nullptr;
            mFilename.clear();
        }
    }

public:
    Pimpl()
        : mFilename(), mStream(&std::cerr), mWarnings(0),
          mLevel(TRACE_LEVEL_ALWAYS), mType(TRACE_STREAM_STANDARD_ERROR)
    {
    }

    ~Pimpl()
    {
        if (mType == TRACE_STREAM_FILE) {
            delete mStream;
        }
    }

    void setLogFile(const std::string& filename)
    {
        auto  tmp = new std::ofstream(filename.c_str());

        if (not tmp->is_open()) {
            delete tmp;
        } else {
            cleanup__();

            mFilename.assign(filename);
            mStream = tmp;
            (*mStream) << _("Start log at ") <<
                utils::DateTime::currentDate() << "\n\n" << std::flush;
            mType = TRACE_STREAM_FILE;
        }
    }

    void setStandardOutput()
    {
        cleanup__();

        mStream = &std::cout;
        mType = TRACE_STREAM_STANDARD_OUTPUT;
    }

    void setStandardError()
    {
        cleanup__();

        mStream = &std::cerr;
        mType = TRACE_STREAM_STANDARD_ERROR;
    }

    std::string getLogFile() const
    {
        return mFilename;
    }

    void send(const std::string& str, TraceLevelOptions level)
    {
        if (mStream) {
            (*mStream) << "---" << str << std::endl;
            if (level != utils::TRACE_LEVEL_ALWAYS) {
                mWarnings++;
            }
        }
    }

    void send(TraceLevelOptions level, const char *format, ...)
    {
        if (mStream) {
            va_list ap;
            int size = 0;

            va_start(ap, format);
            size = vsnprintf(nullptr, size, format, ap);
            va_end(ap);

            if (size < 0)
                return;

            size++;
            std::string ret;
            ret.reserve(size);

            va_start(ap, format);
            size = vsnprintf(&ret[0], size, format, ap);
            if (size < 0)
                return;
            va_end(ap);

            (*mStream) << "---" << ret << std::endl;
            if (level != utils::TRACE_LEVEL_ALWAYS)
                mWarnings++;
        }
    }

    TraceStreamType getType() const
    {
        return mType;
    }

    TraceLevelOptions getLevel() const
    {
        return mLevel;
    }

    void setLevel(TraceLevelOptions level)
    {
        mLevel = (level < 0 or level > utils::TRACE_LEVEL_DEVS) ?
            utils::TRACE_LEVEL_DEVS :
            level;
    }

    bool isInLevel(TraceLevelOptions level) const
    {
        return TRACE_LEVEL_ALWAYS <= level and level <= mLevel;
    }

    bool haveWarning() const
    {
        return mWarnings > 0;
    }

    size_t warnings() const
    {
        return mWarnings;
    }

    std::mutex& getMutex()
    {
        return mMutex;
    }

private:
    std::string mFilename;      /**< The current filename of the singleton. */

    std::ostream* mStream;      /**< A reference to the current ostream. */

    size_t mWarnings;           /**< Number of warning since the singleton
                                   exists. */

    TraceLevelOptions mLevel;   /**< The current level of the
                                 * singleton. */

    TraceStreamType mType;      /**< The current stream. */

    std::mutex mMutex;        /**< Store the instance of the mutex
                                 to implement a thread-safe single
                                 design pattern. */
};

Trace::Trace()
    : ppImpl(new Trace::Pimpl())
{
}

Trace::~Trace() noexcept = default;

std::string Trace::getLogFile()
{
    std::lock_guard<std::mutex> lock(instance().ppImpl->getMutex());

    return instance().ppImpl->getLogFile();
}

void Trace::setLogFile(const std::string& filename)
{
    std::lock_guard<std::mutex> lock(instance().ppImpl->getMutex());

    instance().ppImpl->setLogFile(filename);
}

void Trace::setStandardOutput()
{
    std::lock_guard<std::mutex> lock(instance().ppImpl->getMutex());

    instance().ppImpl->setStandardOutput();
}

void Trace::setStandardError()
{
    std::lock_guard<std::mutex> lock(instance().ppImpl->getMutex());

    instance().ppImpl->setStandardError();
}

void Trace::send(const std::string& str, TraceLevelOptions level)
{
    std::lock_guard<std::mutex> lock(instance().ppImpl->getMutex());

    instance().ppImpl->send(str, level);
}

void Trace::send(TraceLevelOptions level, const char *format, ...)
{
    va_list ap;
    int size = 0;

    va_start(ap, format);
    size = vsnprintf(nullptr, size, format, ap);
    va_end(ap);

    if (size < 0)
        return;

    size++;
    std::string ret(size, '\0');

    va_start(ap, format);
    size = vsnprintf(&ret[0], size, format, ap);
    if (size < 0)
        return;
    va_end(ap);

    std::lock_guard<std::mutex> lock(instance().ppImpl->getMutex());

    instance().ppImpl->send(ret, level);
}

std::string Trace::getDefaultLogFilename()
{
    return getLogFilename("vle.log");
}

std::string Trace::getLogFilename(const std::string& filename)
{
    FSpath p(utils::Path::path().getHomeDir());
    p /= filename;

    return p.string();
}

TraceStreamType Trace::getType()
{
    std::lock_guard<std::mutex> lock(instance().ppImpl->getMutex());

    return instance().ppImpl->getType();
}

TraceLevelOptions Trace::getLevel()
{
    std::lock_guard<std::mutex> lock(instance().ppImpl->getMutex());

    return instance().ppImpl->getLevel();
}

void Trace::setLevel(TraceLevelOptions level)
{
    std::lock_guard<std::mutex> lock(instance().ppImpl->getMutex());

    instance().ppImpl->setLevel(level);
}

bool Trace::isInLevel(TraceLevelOptions level)
{
    std::lock_guard<std::mutex> lock(instance().ppImpl->getMutex());

    return instance().ppImpl->isInLevel(level);
}

bool Trace::haveWarning()
{
    std::lock_guard<std::mutex> lock(instance().ppImpl->getMutex());

    return instance().ppImpl->haveWarning();
}

size_t Trace::warnings()
{
    std::lock_guard<std::mutex> lock(instance().ppImpl->getMutex());

    return instance().ppImpl->warnings();
}

TraceLevelOptions Trace::cast(int level) throw()
{
    switch (level) {
    case 0:
        return TRACE_LEVEL_ALWAYS;
    case 1:
        return TRACE_LEVEL_MODEL;
    case 2:
        return TRACE_LEVEL_EXTENSION;
    case 3:
        return TRACE_LEVEL_DEVS;
    default:
        return TRACE_LEVEL_ALWAYS;
    }
}

}} // namespace vle utils
