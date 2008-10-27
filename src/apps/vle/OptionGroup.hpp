/**
 * @file apps/vle/OptionGroup.hpp
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


#ifndef VLE_OPTIONGROUP_HPP
#define VLE_OPTIONGROUP_HPP

#include <glibmm/optioncontext.h>
#include <vector>

namespace vle { namespace apps {

    /**
     * @brief An abstract class to force checking of attributes members.
     */
    class VLEOptionGroup : public Glib::OptionGroup
    {
    public:
        VLEOptionGroup(const Glib::ustring& name,
                       const Glib::ustring& description,
                       const Glib::ustring& help_description =
                       Glib::ustring()) :
            Glib::OptionGroup(name, description, help_description)
        { }

        virtual ~VLEOptionGroup() { }

        /**
         * @brief A function to throw exception when an error occured in
         * attributes members.
         */
        virtual void check() { }
    };

    /**
     * @brief Manage the command option, manager, simulator and justrun
     * simulator.
     */
    class CommandOptionGroup : public VLEOptionGroup
    {
    public:
        CommandOptionGroup();

        virtual ~CommandOptionGroup() { }

        inline bool manager() const { return mManager; }

        inline bool simulator() const { return mSimulator; }

        inline bool justRun() const { return mJustrun; }

        inline int port() const { return mPort; }

        inline int processor() const { return mProcess; }

        /**
         * @brief Check if only on mode is active.
         *
         * @throw Exception::Internal two or three modes are actives.
         */
        virtual void check();

    private:
        bool    mManager;
        bool    mSimulator;
        bool    mJustrun;
        int     mPort;
        int     mProcess;
    };

    /**
     * @brief Options for manager, ie. run in daemon and all simulation in
     * local vle::Simulator.
     */
    class ManagerOptionGroup : public VLEOptionGroup
    {
    public:
        ManagerOptionGroup();

        virtual ~ManagerOptionGroup() { }

        inline bool allInLocal() const { return mAllinlocal; }

        inline bool savevpz() const { return mSaveVpz; }

        virtual void check();

    private:
        bool    mAllinlocal;
        bool    mSaveVpz;
    };

    /**
     * @brief Global option for all commands, informations, help, version and
     * the verbose mode.
     */
    class GlobalOptionGroup : public VLEOptionGroup
    {
    public:
        GlobalOptionGroup();

        virtual ~GlobalOptionGroup() { }

        inline bool infos() const { return mInfos; }

        inline bool version() const { return mVersion; }

        inline int verbose() const { return mVerbose; }

        /**
         * @brief Check if verbose number mode is correct.
         *
         * @throw Exception::Internal if number is not in range [0 - 3].
         */
        virtual void check();

    private:
        bool    mInfos;
        bool    mVersion;
        int     mVerbose;
    };

}} // namespace vle apps

#endif
