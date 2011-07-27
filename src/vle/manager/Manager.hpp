/*
 * @file vle/manager/Manager.hpp
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


#ifndef VLE_MANAGER_CLASS_MANAGER_HPP
#define VLE_MANAGER_CLASS_MANAGER_HPP

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
#include <vle/manager/DllDefines.hpp>
#include <boost/noncopyable.hpp>

namespace vle { namespace manager {

    /**
     * @brief ManagerRun is the base class for running simulation from a
     * specified vpz file that containts and experimental frames. ManagerRun
     * does not run simulation, see ManagerRunThread and ManagerRunDistant.
     * ManagerRun provides and access to the oov::PluginPtr object of the
     * simulation.
     */
    class VLE_MANAGER_EXPORT ManagerRun : public boost::noncopyable
    {
    public:
        /**
         * @brief Build a ManagerRun.
         *
         * @param out output to log error.
         * @param writefile write all experimental frames file produced.
         * @param storecomb stores simulated combinations in order to access to
         * the function getInputFromCombination.
         * @param commonseed if true, the same seed is used for the simulation
         * of different combinations of one replica
         * @param rnd a pseudo random generator build by the user, if rnd is
         * empty then, a new random number generator will be build from the Vpz
         * file.
         */
        ManagerRun(std::ostream & out, bool writefile, bool storecomb,
                   bool commonseed, RandPtr rnd = RandPtr())
            : m_out(out), m_writefile(writefile), m_storecomb(storecomb),
            m_commonseed(commonseed), m_rand(rnd), m_exp(0)
        {
        }

        /**
         * @brief Build a ManagerRun with default parameters.
         *
         * @param out output to log error.
         * @param writefile write all experimental frames file produced.
         * @param rnd a pseudo random generator build by the user, if rnd is
         * empty then, a new random number generator will be build from the Vpz
         * file.
         */
        ManagerRun(std::ostream & out, bool writefile, RandPtr rnd =
                   RandPtr())
            : m_out(out), m_writefile(writefile), m_storecomb(false),
            m_commonseed(true), m_rand(rnd), m_exp(0)
        {
        }

        virtual ~ManagerRun();

        /**
         * @brief Get a reference to the oov::PluginPtr list of the simulation
         * sort by filename.
         *
         * @return A reference to the oov::PluginPtr list.
         */
        inline OutputSimulationMatrix& outputSimulationMatrix()
        {
            return m_matrix;
        }

        /**
         * @brief Get a constant reference to the oov::PluginPtr list of the
         * simulation sort by filename.
         *
         * @return A reference to the oov::PluginPtr list.
         */
        inline const OutputSimulationMatrix& outputSimulationMatrix() const
        {
            return m_matrix;
        }

        /**
         * @brief Get the input value used for simulating a given combination
         *
         * @param comb number of combination
         * @param condition name of a condition
         * @param port name of a port from the condition
         * @return the value, on port port_name of condition condition_name,
         *  used for simulation of combination comb
         */
        const value::Value& getInputFromCombination(
            unsigned int comb,
            const std::string& condition,
            const std::string& port) const
        {
            if (not m_storecomb) {
                throw utils::ArgError(
                    _("combination storage is not activated"));
            }
            return m_exp->getInputFromCombination(comb, condition, port);
        }

        /**
         * @brief Get the Diff object that stores the differences between
         * the default combination conditions and all the simulated
         * combinations
         *
         * @return the Diff object
         */
        const ExperimentGenerator::Diffs getCombinations() const
        {
            return m_exp->getCombinations();
        }

    protected:
        OutputSimulationMatrix m_matrix;
        std::ostream& m_out;
        bool m_writefile;
        bool m_storecomb;
        bool m_commonseed;
        RandPtr m_rand;
        ExperimentGenerator* m_exp;

        /**
         * @brief Build the ExperimentGenerator attached to the vpz::Vpz
         * definition in experiments tags.
         *
         * @param file The vpz::Vpz file
         * @return A reference to the newly build combination plan
         */
        ExperimentGenerator* getCombinationPlan(const vpz::Vpz& file,
                                                std::ostream&   out);

        /**
         * @brief Initialize a random number generator if the user does not
         * call the constructor with a valid RandomNumberGenerator. The seed is
         * provided by the vpz::Vpz file instance in the replicas tags. If the
         * seed is not provided, the std::time(0) or /dev/urandom is used.
         *
         * @param file The vpz::Vpz file where get the seed.
         */
        void initRandomGenerator(const vpz::Vpz& file);
    };

    /**
     * @brief ManagerRunMono is the class for running experimental frames onto
     * the same host with only one thread.
     */
    class VLE_MANAGER_EXPORT ManagerRunMono : public ManagerRun
    {
    public:
        /**
         * @brief Build a ManagerRunMono.
         * @param out output to log error.
         * @param writefile write all experimental frames file produced.
         * @param storecomb stores simulated combinations in order to access to
         * the function getInputFromCombination.
         * @param commonseed if true, the same seed is used for the simulation
         * of different combinations of one replica
         * @param rnd a pseudo random generator build by the user, if rnd is
         * empty then, a new random number generator will be build from the Vpz
         * file.
         */
        ManagerRunMono(std::ostream & out, bool writefile,
                       bool storecomb, bool commonseed,
                       RandPtr rnd = RandPtr())
            : ManagerRun(out, writefile, storecomb, commonseed, rnd)
        {
        }

        /**
         * @brief Build a ManagerRunMono with default parameters.
         *
         * @param out output to log error.
         * @param writefile write all experimental frames file produced.
         * @param rnd a pseudo random generator build by the user, if rnd is
         * empty then, a new random number generator will be build from the Vpz
         * file.
         */
        ManagerRunMono(std::ostream & out, bool writefile,
                       RandPtr rnd = RandPtr())
            : ManagerRun(out, writefile, false, true, rnd)
        {
        }

        virtual ~ManagerRunMono()
        {
        }

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
        inline void start(const std::string& filename)
        {
            operator()(filename);
        }

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
        inline void start(const vpz::Vpz& file)
        {
            operator()(file);
        }

    private:
        std::string m_filename;
    };

    /**
     * @brief ManagerRunThread is the class for running experimental frames onto
     * the same host with a specific number of threads.
     */
    class VLE_MANAGER_EXPORT ManagerRunThread : public ManagerRun
    {
    public:
        /**
         * @brief Build a ManagerRunThread.
         * @param out output to log error.
         * @param writefile write all experimental frames file produced.
         * @param storecomb stores simulated combinations in order to access to
         * the function getInputFromCombination.
         * @param commonseed if true, the same seed is used for the simulation
         * of different combinations of one replica
         * @param process number of thread to use.
         * @param rnd a pseudo random generator build by the user, if rnd is
         * empty then, a new random number generator will be build from the Vpz
         * file.
         */
        ManagerRunThread(std::ostream & out, bool writefile, int process,
                         bool storecomb, bool commonseed,
                         RandPtr rnd = RandPtr())
            : ManagerRun(out, writefile, storecomb, commonseed, rnd),
            m_process(process), m_finish(false)
        {
        }

        /**
         * @brief Build a ManagerRunThread with default parameters.
         *
         * @param out output to log error.
         * @param writefile write all experimental frames file produced.
         * @param process number of thread to use.
         * @param rnd a pseudo random generator build by the user, if rnd is
         * empty then, a new random number generator will be build from the Vpz
         * file.
         */
        ManagerRunThread(std::ostream & out, bool writefile, int process,
                         RandPtr rnd = RandPtr())
            : ManagerRun(out, writefile, false, true, rnd), m_process(process),
            m_finish(false)
        {
        }

        virtual ~ManagerRunThread()
        {
        }

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
        inline void start(const std::string& filename)
        {
            operator()(filename);
        }

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
        inline void start(const vpz::Vpz& file)
        {
            operator()(file);
        }

        /**
         * @brief A pool of threads to run simulation.
         */
        void run();

        /**
         * @brief A thread to build plan.
         */
        void read();

    private:
        unsigned int m_process;
        std::string m_filename;
        bool m_finish;

        /** simulation threads */
        Glib::ThreadPool m_pool;
        Glib::Mutex m_mutex;
        Glib::Cond m_prodcond;
        Glib::Cond m_conscond;
    };

    /**
     * @brief ManagerRunDistant is the class for running experimental frames
     * onto distant nodes. It parses the user hosts file to get Simulator
     * position and lauch simulation on hosts in immediate mode.
     */
    class VLE_MANAGER_EXPORT ManagerRunDistant : public ManagerRun
    {
    public:
        /**
         * @brief Build a ManagerRunDistant.
         * @param out output to log error.
         * @param writefile write all experimental frames file produced.
         * @param storecomb stores simulated combinations in order to access to
         * the function getInputFromCombination.
         * @param commonseed if true, the same seed is used for the simulation
         * of different combinations of one replica
         * @param rnd a pseudo random generator build by the user, if rnd is
         * empty then, a new random number generator will be build from the Vpz
         * file.
         */
        ManagerRunDistant(std::ostream & out, bool writefile,
                          bool storecomb, bool commonseed,
                          RandPtr rnd = RandPtr());

        /**
         * @brief Build a ManagerRunDistant with default parameters.
         * @param out output to log error.
         * @param writefile write all experimental frames file produced.
         * @param rnd a pseudo random generator build by the user, if rnd is
         * empty then, a new random number generator will be build from the Vpz
         * file.
         */
        ManagerRunDistant(std::ostream & out, bool writefile,
                          RandPtr rnd = RandPtr());

        virtual ~ManagerRunDistant()
        {
        }

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
        inline void start(const std::string& filename)
        {
            operator()(filename);
        }

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
        inline void start(const vpz::Vpz& file)
        {
            operator()(file);
        }

        /**
         * @brief A thread to send vpz to simulators.
         */
        void send();

        /**
         * @brief A thread to build plan.
         */
        void read();

    private:
        void openConnectionWithSimulators();

        void closeConnectionWithSimulators();

        void scheduller();

        gint32 getMaxProcessor(utils::net::Client& cl);

        gint32 getCurrentNumberVpzi(utils::net::Client& cl);

        void sendVpzi(utils::net::Client& cl, const Glib::ustring& filename);

        void getResult(utils::net::Client& cl);

        utils::Hosts mHost;
        std::list < utils::net::Client* > mClients;
        std::string m_filename;
        bool m_finish;

        Glib::Mutex m_mutex;
        Glib::Cond m_prodcond;

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

}}  // namespace vle manager

#endif
