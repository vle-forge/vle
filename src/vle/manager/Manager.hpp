/**
 * @file src/vle/manager/Manager.hpp
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




#ifndef VLE_MANAGER_MANAGER_HPP
#define VLE_MANAGER_MANAGER_HPP

#include <string>
#include <vector>
#include <glibmm/thread.h>
#include <glibmm/threadpool.h>
#include <vle/manager/Types.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Host.hpp>
#include <vle/utils/Socket.hpp>
#include <vle/utils/Rand.hpp>
#include <vle/manager/ExperimentGenerator.hpp>
#include <boost/noncopyable.hpp>

namespace vle { namespace manager {


    /** 
     * @brief ManagerRun is the base class for running simulation from a specified
     * vpz file that containts and experimental frames. ManagerRun does not run
     * simulation, see ManagerRunThread and ManagerRunDistant. ManagerRun
     * provides and access to the oov::PluginPtr object of the simulation.
     */
    class ManagerRun : public boost::noncopyable
    {
    public:
        /** 
         * @brief Build a ManagerRun.
         * @param out output to log error.
         * @param writefile write all experimental frames file produced.
         * @param rnd a pseudo random generator build by the user, if rnd is
         * empty then, a new random number generator will be build from the Vpz
         * file.
         */
        ManagerRun(std::ostream& out, bool writefile, RandPtr rnd = RandPtr());

        /** 
         * @brief Get a reference to the oov::PluginPtr list of the simulation
         * sort by filename.
         * @return A reference to the oov::PluginPtr list.
         */
        OutputSimulationMatrix& outputSimulationMatrix();

        /** 
         * @brief Get a constant reference to the oov::PluginPtr list of the
         * simulation sort by filename.
         * @return A reference to the oov::PluginPtr list.
         */
        const OutputSimulationMatrix& outputSimulationMatrix() const;

    protected:
        OutputSimulationMatrix  m_matrix;
        std::ostream&           m_out;
        bool                    m_writefile;
        RandPtr                 m_rand;

        /** 
         * @brief Build the ExperimentGenerator attached to the vpz::Vpz
         * definition in experiments tags.
         * @param file The vpz::Vpz file
         * @return A reference to the newly build combination plan
         */
        ExperimentGenerator* getCombinationPlan(
            const vpz::Vpz& file, std::ostream& out);

        /** 
         * @brief Initialize a random number generator if the user does not
         * call the constructor with a valid RandomNumberGenerator. The seed is
         * provided by the vpz::Vpz file instance in the replicas tags. If the
         * seed is not provided, the std::time(0) or /dev/urandom is used.
         * @param file The vpz::Vpz file where get the seed.
         */
        void initRandomGenerator(const vpz::Vpz& file);
    };



    /** 
     * @brief ManagerRunMono is the class for running experimental frames onto
     * the same host with only one thread.
     */
    class ManagerRunMono : public ManagerRun
    {
    public:
        /** 
         * @brief Build a ManagerRunMono.
         * @param out output to log error.
         * @param writefile write all experimental frames file produced.
         * @param process number of thread to use.
         * @param rnd a pseudo random generator build by the user, if rnd is
         * empty then, a new random number generator will be build from the Vpz
         * file.
         */
        ManagerRunMono(std::ostream& out, bool writefile,
                       RandPtr ptr = RandPtr());

        /** 
         * @brief Read specified vpz files an start simulations on the number of
         * processor specified in constructor.
         * @param filename.
         */
        void operator()(const std::string& filename);

        /** 
         * @brief Read specified vpz files an start simulations on the number of
         * processor specified in constructor.
         * @param filename.
         */
        void start(const std::string& filename);

        /** 
         * @brief Read specified vpz files an start simulations on the number of
         * processor specified in constructor.
         * @param file.
         */
        void operator()(const vpz::Vpz& file);

        /** 
         * @brief Read specified vpz files an start simulations on the number of
         * processor specified in constructor.
         * @param filename.
         */
        void start(const vpz::Vpz& file);

    private:
        unsigned int                            m_process;
        std::string                             m_filename;
        bool                                    m_finish;

        ExperimentGenerator*                    m_exp;
    };



    /** 
     * @brief ManagerRunThread is the class for running experimental frames onto
     * the same host with a specific number of threads.
     */
    class ManagerRunThread : public ManagerRun
    {
    public:
        /** 
         * @brief Build a ManagerRunThread.
         * @param out output to log error.
         * @param writefile write all experimental frames file produced.
         * @param process number of thread to use.
         * @param rnd a pseudo random generator build by the user, if rnd is
         * empty then, a new random number generator will be build from the Vpz
         * file.
         */
        ManagerRunThread(std::ostream& out, bool writefile, int process,
                         RandPtr rnd = RandPtr());

        /** 
         * @brief Read specified vpz files an start simulations on the number of
         * processor specified in constructor.
         * @param filename.
         */
        void operator()(const std::string& filename);

        /** 
         * @brief Read specified vpz files an start simulations on the number of
         * processor specified in constructor.
         * @param filename.
         */
        void start(const std::string& filename);

        /** 
         * @brief Read specified vpz files an start simulations on the number of
         * processor specified in constructor.
         * @param file.
         */
        void operator()(const vpz::Vpz& file);

        /** 
         * @brief Read specified vpz files an start simulations on the number of
         * processor specified in constructor.
         * @param filename.
         */
        void start(const vpz::Vpz& file);

        /** 
         * @brief A pool of threads to run simulation.
         */
        void run();

        /** 
         * @brief A thread to build plan.
         */
        void read();

    private:
        unsigned int                            m_process;
        std::string                             m_filename;
        bool                                    m_finish;

        ExperimentGenerator*                    m_exp;
       
        /** simulation threads */
        Glib::ThreadPool                        m_pool;
        Glib::Mutex                             m_mutex;
        Glib::Cond                              m_prodcond;
        Glib::Cond                              m_conscond;
    };


    
    /** 
     * @brief ManagerRunDistant is the class for running experimental frames
     * onto distant nodes. It parses the user hosts file to get Simulator
     * position and lauch simulation on hosts in immediate mode.
     */
    class ManagerRunDistant : public ManagerRun
    {
    public:
        /** 
         * @brief Build a ManagerRunDistant.
         * @param out output to log error.
         * @param writefile write all experimental frames file produced.
         * @param rnd a pseudo random generator build by the user, if rnd is
         * empty then, a new random number generator will be build from the Vpz
         * file.
         */
        ManagerRunDistant(std::ostream& out, bool writefile,
                          RandPtr rnd = RandPtr());

        /** 
         * @brief Read specified vpz files an start simulations on the number of
         * processor specified in constructor.
         * @param filename.
         */
        void operator()(const std::string& filename);

        /** 
         * @brief Read specified vpz files an start simulations on the number of
         * processor specified in constructor.
         * @param filename.
         */
        void start(const std::string& filename);

        /** 
         * @brief Read specified vpz files an start simulations on the number of
         * processor specified in constructor.
         * @param file.
         */
        void operator()(const vpz::Vpz& file);

        /** 
         * @brief Read specified vpz files an start simulations on the number of
         * processor specified in constructor.
         * @param filename.
         */
        void start(const vpz::Vpz& file);
        
        /** 
         * @brief A thread to send vpz to simulators.
         */
        void send();

        /** 
         * @brief A thread to build plan.
         */
        void read();

    private:
        ExperimentGenerator* get_combination_plan() const;
        void openConnectionWithSimulators();
        void closeConnectionWithSimulators();
        void scheduller();

        gint32 getMaxProcessor(utils::net::Client& cl);
        gint32 getCurrentNumberVpzi(utils::net::Client& cl);
        void sendVpzi(utils::net::Client& cl, const Glib::ustring& filename);
        void getResult(utils::net::Client& cl);

        utils::Hosts                                mHost;
        std::list < utils::net::Client* >           mClients;
        std::string                                 m_filename;
        bool                                        m_finish;

        Glib::Mutex                                 m_mutex;
        Glib::Cond                                  m_prodcond;

        ExperimentGenerator*                        m_exp;

        /** 
         * @brief A thread safe function to acces to the top of the vpz::Vpz
         * reference from the ExperimentGenerator.
         * @return A reference to the vpz::Vpz reference or null if empty.
         */
        vpz::Vpz* getVpz();

        /** 
         * @brief Return the number of vpz::Vpz file remaining in the
         * ExperimentGenerator.
         * @return A number of vpz::Vpz file to simulate;
         */
        unsigned int getVpzNumber();
    };


    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */


    inline ManagerRun::ManagerRun(std::ostream& out, bool writefile,
                                  RandPtr rnd) :
        m_out(out),
        m_writefile(writefile),
        m_rand(rnd)
    {
    }

    inline OutputSimulationMatrix& ManagerRun::outputSimulationMatrix()
    {
        return m_matrix;
    }

    inline const 
    OutputSimulationMatrix& ManagerRun::outputSimulationMatrix() const
    {
        return m_matrix;
    }

    inline ManagerRunMono::ManagerRunMono(std::ostream& out, bool writefile,
                                          RandPtr rnd) :
        ManagerRun(out, writefile, rnd)
    {
    }

    inline void ManagerRunMono::start(const std::string& filename)
    {
        operator()(filename);
    }

    inline void ManagerRunMono::start(const vpz::Vpz& file)
    {
        operator()(file);
    }
    
    inline ManagerRunThread::ManagerRunThread(std::ostream& out, bool writefile,
                                              int process, RandPtr rnd) :
        ManagerRun(out, writefile, rnd),
        m_process(process),
        m_finish(false),
        m_exp(0)
    {
    }

    inline void ManagerRunThread::start(const std::string& filename)
    {
        operator()(filename);
    }

    inline void ManagerRunThread::start(const vpz::Vpz& file)
    {
        operator()(file);
    }

    inline void ManagerRunDistant::start(const std::string& filename)
    {
        operator()(filename);
    }

    inline void ManagerRunDistant::start(const vpz::Vpz& file)
    {
        operator()(file);
    }

}} // namespace vle manager

#endif
