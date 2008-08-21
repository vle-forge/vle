/**
 * @file vle/manager/JustRun.hpp
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


#ifndef VLE_MANAGER_JUSTRUN_HPP
#define VLE_MANAGER_JUSTRUN_HPP

#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/manager/Types.hpp>
#include <glibmm/thread.h>
#include <glibmm/threadpool.h>
#include <queue>
#include <list>

namespace vle { namespace manager {

    /** 
     * @brief JustRun is the base class for running simulation from specified
     * vpz files. Errors are log to specified output std::ostream. JustRun
     * provided access to the oov::PluginPtr object.
     */
    class JustRun
    {
    public:
        /** 
         * @brief Build a JustRun object with a specified output std::ostream.
         * @param out output to log error.
         */
        JustRun(std::ostream& out);

        /** 
         * @brief Get a reference to the oov::PluginPtr list of the simulation
         * sort by filename.
         * @return A reference to the oov::PluginPtr list.
         */
        OutputSimulationList& outputSimulationList();

        /** 
         * @brief Get a constant reference to the oov::PluginPtr list of the
         * simulation sort by filename.
         * @return A reference to the oov::PluginPtr list.
         */
        const OutputSimulationList& outputSimulationList() const;

        /** 
         * @brief Get a constant reference to the dictionary to find vpz file
         * name and index in the output simulation list.
         * @return A constant reference to the vpz file names.
         */
        const OutputSimulationNames& outputSimulationNames() const;

        /** 
         * @brief Get a reference to the dictionary to find vpz file name and
         * index in the output simulation list.
         * @return A reference to the vpz file names.
         */
        OutputSimulationNames& outputSimulationNames();

    protected:
        OutputSimulationList    m_lst;
        OutputSimulationNames   m_names;
        std::ostream&           m_out;
    };



    /** 
     * @brief JustRunMono is the class for running simulation from specified vpz
     * files. All simulation are run on the same process.
     */
    class JustRunMono : public JustRun
    {
    public:
        /** 
         * @brief Build a JustRunMono object with a specified output
         * std::ostream.
         * @param out output to log error.
         */
        JustRunMono(std::ostream& out);

        /** 
         * @brief Read specified vpz files and start simulations on the same
         * process.
         */
        void operator()(const CmdArgs& args);

    private:
        bool        m_output;
    };


    /** 
     * @brief JustRunThread is the class for running simulation from specified
     * vpz files. All simulation are run on the specified number of processor.
     */
    class JustRunThread : public JustRun
    {
    public:
        /** 
         * @brief Build a JustRunMono object with a specified output
         * std::ostream.
         * @param out output to log error.
         * @param process number of thread.
         */
        JustRunThread(std::ostream& out, int process);

        /** 
         * @brief Read specified vpz files an start simulations on the number of
         * processor specified in constructor.
         */
        void operator()(const CmdArgs& args);

    private:
        CmdArgs                                 m_args;

        /** @brief the protected structure to thread */
        std::list < vpz::Vpz* >                 m_vpzs;

        /** @brief conserve the output of each simulation. */
        std::map < std::string, std::string >   m_outputs;

        unsigned int                            m_process;
        bool                                    m_finish;

        Glib::ThreadPool            m_pool; /* simulation threads */
        Glib::Mutex                 m_mutex;
        Glib::Cond                  m_prodcond;
        Glib::Cond                  m_conscond;

        /** 
         * @brief A thread to convert filename to vpz files.
         */
        void read();

        /** 
         * @brief A pool of threads to run simulation.
         */
        void run();
    };

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    inline JustRun::JustRun(std::ostream& out) :
        m_out(out)
    {
    }

    inline OutputSimulationList& JustRun::outputSimulationList()
    {
        return m_lst;
    }

    inline const OutputSimulationList& JustRun::outputSimulationList() const
    {
        return m_lst;
    }

    inline const OutputSimulationNames& JustRun::outputSimulationNames() const
    {
        return m_names;
    }

    inline OutputSimulationNames& JustRun::outputSimulationNames()
    {
        return m_names;
    }

    inline JustRunMono::JustRunMono(std::ostream& out) :
        JustRun(out),
        m_output(true)
    {
    }
    
    inline JustRunThread::JustRunThread(std::ostream& out, int process) :
        JustRun(out),
        m_process(process),
        m_finish(false)
    {
    }

}} // namespace vle manager

#endif
