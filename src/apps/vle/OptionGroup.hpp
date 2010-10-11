/*
 * @file apps/vle/OptionGroup.hpp
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


#ifndef VLE_OPTIONGROUP_HPP
#define VLE_OPTIONGROUP_HPP

#include <glibmm/optioncontext.h>
#include <vector>

namespace vle { namespace apps {

    /**
     * @brief An abstract class to force checking of attributes members.
     */
    class OptionGroup : public Glib::OptionGroup
    {
    public:
        OptionGroup();

        virtual ~OptionGroup() {}

        bool manager() const { return mManager; }
        bool simulator() const { return mSimulator; }
        bool justRun() const { return mJustrun; }
        int port() const { return mPort; }
        int processor() const { return mProcess; }
        bool allInLocal() const { return mAllinlocal; }
        bool savevpz() const { return mSaveVpz; }
        bool infos() const { return mInfos; }
        bool version() const { return mVersion; }
        bool list() const { return mList; }
        int verbose() const { return mVerbose; }
        const Glib::ustring& currentPackage() const { return mCurrentPackage; }
        bool package() const { return mPackage; }
        bool remote() const { return mRemote; }
        bool config() const { return mConfig; }

        /**
         * @brief Check the validity of the selected option from the command
         * line interface.
         * @throw vle::utils::InternalError.
         */
        void check();

    private:
        bool          mManager;
        bool          mSimulator;
        bool          mJustrun;
        int           mPort;
        int           mProcess;
        bool          mAllinlocal;
        bool          mSaveVpz;
        bool          mInfos;
        bool          mVersion;
        bool          mList;
        int           mVerbose;
        Glib::ustring mCurrentPackage;
        bool          mPackage;
        bool          mRemote;
        bool          mConfig;
    };

}} // namespace vle apps

#endif
