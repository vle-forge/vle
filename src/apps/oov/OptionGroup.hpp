/** 
 * @file OptionGroup.hpp
 * @brief Some classes to mange the program arguments from command line.
 * @author The vle Development Team
 * @date mer, 07 jun 2006 10:25:04 +0200
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

#ifndef OOV_OPTIONGROUP_HPP
#define OOV_OPTIONGROUP_HPP

#include <glibmm/optioncontext.h>
#include <vector>

namespace vle { namespace apps {

    /** 
     * @brief Manage the command option, manager, simulator and justrun
     * simulator.
     */
    class CommandOptionGroup : public Glib::OptionGroup
    {
    public:
        CommandOptionGroup();

        virtual ~CommandOptionGroup() { }

        inline bool isDaemon() const { return mDaemon; }

        inline int port() const { return mPort; }
        
        inline bool infos() const { return mInfos; }

        inline bool version() const { return mVersion; }

        inline int verbose() const { return mVerbose; }

        /** 
         * @brief Check if only on mode is active.
         *
         * @throw Exception::Internal two or three modes are actives.
         */
        virtual void check();

    private:
        bool    mDaemon;
        int     mPort;
        bool    mInfos;
        bool    mVersion;
        int     mVerbose;
    };

}} // namespace vle apps

#endif
