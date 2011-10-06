/*
 * @file vle/utils/Trace.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
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


#include <vle/utils/Trace.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/DateTime.hpp>
#include <vle/utils/Debug.hpp>
#include <boost/thread.hpp>


namespace vle { namespace utils {

/*
 * The utils::Trace::Pimpl source.
 */

class Trace::Pimpl
{
public:
    Pimpl()
        : mFilename(getDefaultLogFilename()),
        mFile(new std::ofstream(getDefaultLogFilename().c_str())),
        mLevel(utils::Trace::ALWAYS), mWarnings(0)
    {
        if (not mFile->is_open()) {
            mFilename.clear();
            delete mFile;
            mFile = 0;
        } else {
            (*mFile) << _("Start log at ") << utils::DateTime::currentDate()
                << "\n\n" << std::flush;
        }
    }

    ~Pimpl()
    {
        delete mFile;
    }

    void setLogFile(const std::string& filename)
    {
        std::ofstream* tmp = new std::ofstream(filename.c_str());

        if (not tmp->is_open()) {
            delete tmp;
        } else {
            if (mFile) {
                mFile->close();
                delete mFile;
            }
            mFilename.assign(filename);
            mFile = tmp;
            (*mFile) << _("Start log at ") <<
                utils::DateTime::currentDate() << "\n\n" << std::flush;
        }
    }

    std::string getLogFile()
    {
        return mFilename;
    }

    void send(const std::string& str, Level level)
    {
        if (mFile) {
            (*mFile) << "---" << str << std::endl;
            if (level != ALWAYS) {
                mWarnings++;
            }
        }
    }

    Level getLevel()
    {
        return mLevel;
    }

    void setLevel(Level level)
    {
        mLevel = (level < 0 or level > utils::Trace::DEVS) ?
            utils::Trace::DEVS : level;
    }

    bool isInLevel(Level level)
    {
        return Trace::ALWAYS <= level and level <= mLevel;
    }

    bool haveWarning()
    {
        return mWarnings > 0;
    }

    size_t warnings()
    {
        return mWarnings;
    }

    static boost::mutex mMutex; /**< Store the instance of the mutex to
                                  implemend a thread-safe single design
                                  pattern. */

    static Pimpl* mTrace; /**< The singleton static variable, to be sure to
                            avoid all multithread problem, call the
                            Trace::init() function before using macro or
                            utils::Trace API. */

private:
    std::string     mFilename; /**< The current filename of the singleton. */

    std::ofstream*  mFile; /**< A reference to the current ostream. */

    Level           mLevel; /**< The current level of the singleton. */

    size_t          mWarnings; /**< Number of warning since the singleton
                                 exists. */
};

boost::mutex Trace::Pimpl::mMutex;
Trace::Pimpl* Trace::Pimpl::mTrace = 0;

/*
 * the utils::Trace source.
 */

void Trace::init()
{
    if (not Trace::Pimpl::mTrace) {
        boost::mutex::scoped_lock lock(Trace::Pimpl::mMutex);

        if (not Trace::Pimpl::mTrace) {
            Trace::Pimpl::mTrace = new Trace::Pimpl();
        }
    }
}

void Trace::kill()
{
    if (Trace::Pimpl::mTrace) {
        boost::mutex::scoped_lock lock(Pimpl::mMutex);

        if (Trace::Pimpl::mTrace) {
            delete Trace::Pimpl::mTrace;
            Trace::Pimpl::mTrace = 0;
        }
    }
}

std::string Trace::getLogFile()
{
    if (not Trace::Pimpl::mTrace) {
        Trace::init();
    }

    boost::mutex::scoped_lock lock(Pimpl::mMutex);

    return Pimpl::mTrace->getLogFile();
}

void Trace::setLogFile(const std::string& filename)
{
    if (not Trace::Pimpl::mTrace) {
        Trace::init();
    }

    boost::mutex::scoped_lock lock(Pimpl::mMutex);

    Pimpl::mTrace->setLogFile(filename);
}

void Trace::send(const std::string& str, Level level)
{
    if (not Trace::Pimpl::mTrace) {
        Trace::init();
    }

    boost::mutex::scoped_lock lock(Pimpl::mMutex);

    Pimpl::mTrace->send(str, level);
}

void Trace::send(const boost::format& str, Level level)
{
    if (not Trace::Pimpl::mTrace) {
        Trace::init();
    }

    boost::mutex::scoped_lock lock(Pimpl::mMutex);

    Pimpl::mTrace->send(str.str(), level);
}

std::string Trace::getDefaultLogFilename()
{
    return getLogFilename("vle.log");
}

std::string Trace::getLogFilename(const std::string& filename)
{
    return Path::buildFilename(utils::Path::path().getHomeDir(), filename);
}

Trace::Level Trace::getLevel()
{
    if (not Trace::Pimpl::mTrace) {
        Trace::init();
    }

    boost::mutex::scoped_lock lock(Pimpl::mMutex);

    return Pimpl::mTrace->getLevel();
}

void Trace::setLevel(Trace::Level level)
{
    if (not Trace::Pimpl::mTrace) {
        Trace::init();
    }

    boost::mutex::scoped_lock lock(Pimpl::mMutex);

    Pimpl::mTrace->setLevel(level);
}

bool Trace::isInLevel(Trace::Level level)
{
    if (not Trace::Pimpl::mTrace) {
        Trace::init();
    }

    boost::mutex::scoped_lock lock(Pimpl::mMutex);

    return Pimpl::mTrace->isInLevel(level);
}

bool Trace::haveWarning()
{
    if (not Trace::Pimpl::mTrace) {
        Trace::init();
    }

    boost::mutex::scoped_lock lock(Pimpl::mMutex);

    return Pimpl::mTrace->haveWarning();
}

size_t Trace::warnings()
{
    if (not Trace::Pimpl::mTrace) {
        Trace::init();
    }

    boost::mutex::scoped_lock lock(Pimpl::mMutex);

    return Pimpl::mTrace->warnings();
}

}} // namespace vle utils
