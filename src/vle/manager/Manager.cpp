/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
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

#include <vle/manager/Manager.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Spawn.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/vpz/BaseModel.hpp>
#include <vle/vpz/Vpz.hpp>

#include "utils/i18n.hpp"

#include <memory>
#include <sstream>
#include <thread>
#include <utility>
#include <mutex>

#include "utils/ContextPrivate.hpp"
#include "manager/details/wrapper_init.hpp"
#include "manager/details/manager_concepts.hpp"
#include "manager/details/manager_initializations.hpp"
#include "manager/details/thread_specific.hpp"
#include "manager/details/cvle_specific.hpp"

namespace vle {
namespace manager {

//static functions

utils::Path
make_temp(const char* format, const std::string& dir)
{
    //utils::Path tmp = utils::Path::temp_directory_path();
    utils::Path tmp(dir);
    tmp /= utils::Path::unique_path(format);
    return tmp;
}


class Manager::Pimpl
{
public:
    utils::ContextPtr mContext;
    utils::Rand mRand;
    ParallelOptions mParalleloption;      // type of parallelisation
    unsigned int mNbslots;                // nbslots for parallelization
    SimulationOptions mSimulationoption;  // for 1 simulation
    std::chrono::milliseconds mTimeout;   // for 1 simulation
    bool mRemoveMPIfiles;                 // for cvle
    bool mGenerateMPIhost;                // for cvle
    std::string mWorkingDir;             // for cvle (only ?)

    void
    checkSimulationOption()
    {
        if (mSimulationoption == SIMULATION_NO_RETURN) {
            mContext->log(VLE_LOG_WARNING, "[Manager] simulation otpion "
                    "NO_RETURN is not compatible\n");
            mSimulationoption = vle::manager::SIMULATION_SPAWN_PROCESS;
        }
        if (mTimeout != std::chrono::milliseconds::zero()) {
            mSimulationoption |= vle::manager::SIMULATION_SPAWN_PROCESS;
            mContext->log(VLE_LOG_WARNING, "[Manager] using a timeout requires"
                    " to spawn simulation\n");
        }
    }

    void
    checkParallelOption()
    {
        if (mParalleloption == PARALLEL_MONO and mNbslots != 1) {
            mNbslots = 1;
            mContext->log(VLE_LOG_WARNING, "[Manager] nb slots is set to 1"
                    "since no parallelization will be used\n");
        }
        if (mParalleloption != PARALLEL_MONO and mNbslots < 2) {
            mNbslots = 1;
            mParalleloption = PARALLEL_MONO;
            mContext->log(VLE_LOG_WARNING, "[Manager] parallelization is "
                    "disabled since nb slots is 1\n");
        }
    }

public:
    Pimpl(utils::ContextPtr context,
          ParallelOptions paralleloption,      // type of parallelisation
          unsigned int nbslots,                // nbslots for parallelization
          SimulationOptions simulationoption,  // for 1 simulation
          std::chrono::milliseconds timeout,   // for 1 simulation
          bool removeMPIfiles,                 // for cvle
          bool generateMPIhost,                // for cvle
          const std::string& workingDir)       // for cvle
      : mContext(context)
      , mRand()
      , mParalleloption(paralleloption)
      , mNbslots(nbslots)
      , mSimulationoption(simulationoption)
      , mTimeout(timeout)
      , mRemoveMPIfiles(removeMPIfiles)
      , mGenerateMPIhost(generateMPIhost)
      , mWorkingDir(workingDir)
    {
        checkSimulationOption();
        checkParallelOption();
    }


    void
    set_log_priority(unsigned int logLevel)
    {
        mContext->set_log_priority(logLevel);
    }

    void
    set_seed(unsigned int seed)
    {
        mRand.seed(seed);
    }

    utils::Rand&
    random_number_generator()
    {
        return mRand;
    }

    void
    parallel_config(ParallelOptions paralleloption, unsigned int nbslots)
    {
        mParalleloption = paralleloption;
        mNbslots = nbslots;
        checkParallelOption();
    }

    void
    simulation_config(SimulationOptions simulationoption,
            std::chrono::milliseconds timeout)
    {
        mSimulationoption = simulationoption;
        mTimeout = timeout;
        checkSimulationOption();
    }

    void
    cvle_config(bool removeMPIfiles, bool generateMPIhost, bool workingDir)
    {
        mRemoveMPIfiles = removeMPIfiles;
        mGenerateMPIhost = generateMPIhost;
        mWorkingDir = workingDir;
    }

    void
    configure(wrapper_init& init)
    {
        bool status = true;
        if (init.exist("log_level", status)) {
            set_log_priority(init.getInt("log_level", status));
        }
        if (init.exist("seed", status)) {
            set_seed(init.getInt("seed", status));
        }
        if (init.exist("parallel_option", status)) {
            std::string tmp;
            tmp.assign(init.getString("parallel_option", status));
            if (tmp == "threads") {
                mParalleloption = PARALLEL_THREADS;
            } else if (tmp == "mpi") {
                mParalleloption = PARALLEL_MPI;
            } else if (tmp == "mono") {
                mParalleloption = PARALLEL_MONO;
            } else {
                mContext->log(VLE_LOG_WARNING, "[Manager] unrecognised "
                        "parallel option : 'mono' is used");
                mParalleloption = PARALLEL_MONO;
                return;
            }
        }
        if (init.exist("nb_slots", status)) {
            mNbslots = init.getInt("nb_slots", status);
        }

        if (init.exist("simulation_spawn", status)) {
            if (init.getBoolean("simulation_spawn", status)) {
                mSimulationoption = SIMULATION_SPAWN_PROCESS;
            } else {
                mSimulationoption = SIMULATION_NONE;
            }
        }

        if (init.exist("simulation_timeout", status)) {
            mTimeout = std::chrono::milliseconds(
                    init.getInt("simulation_timeout", status));
        }

        if (init.exist("rm_MPI_files", status)) {
            mRemoveMPIfiles = init.getBoolean("rm_MPI_files", status);
        }
        if (init.exist("generate_MPI_host", status)) {
            mGenerateMPIhost = init.getBoolean("generate_MPI_host", status);
        }
        if (init.exist("working_dir", status)) {
            mWorkingDir = init.getString("working_dir", status);
        }
        checkSimulationOption();
        checkParallelOption();
    }


    /********************************************************/
    ///run plan without parallelization
    std::unique_ptr<value::Map>
    run_without_parallelization(
            std::unique_ptr<vpz::Vpz> model,
            std::unique_ptr<ManagerObjects> manObj,
            const wrapper_init& init,
            manager::Error& err)
    {
        init_embedded_model(*model, *manObj, init, err);
        if (err.code) {
            return nullptr;
        }
        std::unique_ptr<value::Map> results =
                init_results(manObj->mOutputs, err);
        if (err.code) {
            return nullptr;
        }

        unsigned int inputSize = manObj->inputsSize();
        unsigned int repSize = manObj->replicasSize();

        mContext->log(VLE_LOG_NOTICE, "[Manager] simulation mono nb simus:"
                " %u \n", (repSize*inputSize));

        std::vector<std::thread> gp;
        std::mutex results_mutex;

        thread_worker worker = thread_worker(mContext, *model, init,
                *manObj, manObj->mOutputs, *results, results_mutex,
                mTimeout, mSimulationoption, 0, 1, err);
        worker();
        if (err.code) {
            return nullptr;
        }

        mContext->log(VLE_LOG_NOTICE, "[Manager] end simulation"
                " and aggregation without parallelization\n");
        return results;
    }
    /********************************************************/
    ///run plan with threads
    std::unique_ptr<value::Map>
    run_with_threads(
            std::unique_ptr<vpz::Vpz> model,
            std::unique_ptr<ManagerObjects> manObj,
            const wrapper_init& init,
            manager::Error& err)
    {
        init_embedded_model(*model, *manObj, init, err);
        if (err.code) {
            return nullptr;
        }
        std::unique_ptr<value::Map> results =
                init_results(manObj->mOutputs, err);
        if (err.code) {
            return nullptr;
        }

        unsigned int inputSize = manObj->inputsSize();
        unsigned int repSize = manObj->replicasSize();

        mContext->log(VLE_LOG_NOTICE, "[Manager] simulation threads"
                " (%d threads) nb simus: %u \n", mNbslots,
                (repSize*inputSize));

        std::vector<std::thread> gp;
        std::mutex results_mutex;
        for (uint32_t i = 0; i < mNbslots; ++i) {
            utils::ContextPtr ctx = mContext->clone();
            ctx->set_log_function(std::unique_ptr<utils::Context::LogFunctor>(
                    new thread_log(i)));
            gp.emplace_back(thread_worker(ctx, *model, init,
                    *manObj, manObj->mOutputs, *results, results_mutex,
                    mTimeout, mSimulationoption, i, mNbslots, err));
        }

        for (uint32_t i = 0; i < mNbslots; ++i)
            gp[i].join();

        if (err.code) {
            return nullptr;
        }

        mContext->log(VLE_LOG_NOTICE, "[Manager] end simulation and"
                " aggregation using threads\n");
        return results;
    }

    /********************************************************/
    /// run Plan with cvle
    std::unique_ptr<value::Map>
    run_with_cvle(
            std::unique_ptr<vpz::Vpz> model,
            std::unique_ptr<ManagerObjects> manObj,
            const wrapper_init& init,
            manager::Error& err)
    {
        init_embedded_model(*model, *manObj, init, err);
        if (err.code) {
            return nullptr;
        }

        unsigned int inputSize = manObj->inputsSize();
        unsigned int repSize = manObj->replicasSize();

        mContext->log(VLE_LOG_NOTICE, "[Manager] simulation with mpi"
                " (%d slots) nb simus: %u \n", mNbslots,
                (repSize*inputSize));

        //save vpz
        utils::Path tempVpzPath = make_temp(
                "vle-rec-%%%%-%%%%-%%%%-%%%%.vpz", mWorkingDir);
        model->write(tempVpzPath.string());

        //write content in in.csv file
        utils::Path tempInCsv = make_temp(
                "vle-rec-%%%%-%%%%-%%%%-%%%%-in.csv", mWorkingDir);
        cvle_write(*manObj, init, tempInCsv.string(), err);
        if (err.code == -1) {
            return nullptr;
        }

        //define output file
        utils::Path  tempOutCsv = make_temp(
                "vle-rec-%%%%-%%%%-%%%%-%%%%-out.csv", mWorkingDir);

        //launch mpirun
        utils::Spawn mspawn(mContext);
        std::string exe = mContext->findProgram("mpirun").string();

        std::vector < std::string > argv;
        argv.push_back("-np");
        argv.push_back(vle::utils::to(mNbslots));
        utils::Path  tempHostFile;
        if (mGenerateMPIhost) {
            tempHostFile = make_temp("vle-rec-%%%%-%%%%-%%%%-%%%%-host.txt",
                    mWorkingDir);
            cvle_write_hostfile(mNbslots, tempHostFile.string());
            argv.push_back("--hostfile");
            argv.push_back(tempHostFile.string());
        }

        argv.push_back("cvle");
        argv.push_back("--block-size");
        argv.push_back(vle::utils::to(
                (int(inputSize*repSize/int(mNbslots-1)+1))));
        if (not mSimulationoption != SIMULATION_SPAWN_PROCESS) {
            argv.push_back("--withoutspawn");
        }
        argv.push_back("--more-output-details");
        ////use mpi_warn_nf_forgk since cvle launches simulation with fork
        //// if withspawn (?)
        //argv.push_back("--mca");
        //argv.push_back("mpi_warn_on_fork");
        //argv.push_back("0");
        ////set more log for mpirun
        //argv.push_back("--mca");
        //argv.push_back("ras_gridengine_verbose");
        //argv.push_back("1");
        //argv.push_back("--mca");
        //argv.push_back("plm_gridengine_verbose");
        //argv.push_back("1");
        //argv.push_back("--mca");
        //argv.push_back("ras_gridengine_show_jobid");
        //argv.push_back("1");
        //argv.push_back("--mca");
        //argv.push_back("plm_gridengine_debug");
        //argv.push_back("1");
        argv.push_back(tempVpzPath.string());
        argv.push_back("-i");
        argv.push_back(tempInCsv.string());
        argv.push_back("-o");
        argv.push_back(tempOutCsv.string());

        if (mContext->get_log_priority() >= VLE_LOG_INFO) {
            std::string messageDbg ="";
            for (const auto& s : argv ) {
                messageDbg += " ";
                messageDbg += s;
            }
            messageDbg += "\n";
            mContext->log(VLE_LOG_INFO, "[Manager] launching in dir %s: %s %s",
                    mWorkingDir.c_str(), exe.c_str(), messageDbg.c_str());
        }

        bool started = mspawn.start(exe, mWorkingDir, argv);
        if (not started) {
            err.code = -1;
            err.message = vle::utils::format(
                    "[Manager] Failed to start `%s'", exe.c_str());
            return nullptr;
        }
        bool is_success = true;
        std::string output, error;
        while (not mspawn.isfinish()) {
            if (mspawn.get(&output, &error)) {
                mContext->log(VLE_LOG_INFO, output);
                output.clear();
                mContext->log(VLE_LOG_INFO, error);
                error.clear();
                std::this_thread::sleep_for(std::chrono::microseconds(200));
            } else {
                break;
            }
        }
        mspawn.wait();
        mspawn.status(&error, &is_success);
        if (! is_success) {
            err.code = -1;
            err.message = "[Manager] ";
            err.message += vle::utils::format("Error launching `%s' : %s ",
                    exe.c_str(), error.c_str());
            return nullptr;
        }
        mContext->log(VLE_LOG_NOTICE,
                "[Manager] simulation with mpi performed\n");
        //read output file
        std::unique_ptr<value::Map> results = cvle_read(tempOutCsv,
                inputSize, repSize, manObj->mOutputs, err);
        if (err.code) {
            return nullptr;
        }
        mContext->log(VLE_LOG_NOTICE,
                "[Manager] results aggregation performed\n");
        if (mRemoveMPIfiles) {
            tempOutCsv.remove();
            tempInCsv.remove();
            tempVpzPath.remove();
            if (mGenerateMPIhost) {
                tempHostFile.remove();
            }
        }
        return results;
    }

    /*********************************************/
    //init manager structures from wrapper init
    std::unique_ptr<ManagerObjects>
    init_from_plan(wrapper_init& init, manager::Error& err)
    {

        std::unique_ptr<ManagerObjects> manObj(new ManagerObjects());
        std::string in_cond;
        std::string in_port;
        std::string out_id;
        configure(init);
        bool status;
        try {
            std::string conf = "";
            for (init.begin(); not init.isEnded(); init.next()) {
                const value::Value& val = init.current(conf, status);
                if (parseInput(conf, in_cond, in_port, "define_")) {
                    manObj->mDefine.emplace_back(
                            new ManDefine(in_cond, in_port, val));
                } else if (parseInput(conf, in_cond, in_port, "propagate_")) {
                    manObj->mPropagate.emplace_back(
                            new ManPropagate(in_cond, in_port));
                } else if (parseInput(conf, in_cond, in_port)) {
                    manObj->mInputs.emplace_back(
                            new ManInput(in_cond, in_port, val, mRand));
                    //TODO handle composite
                } else if (parseInput(conf, in_cond, in_port, "replicate_")){
                    manObj->mReplicates.emplace_back(
                            new ManReplicate(in_cond, in_port, val, mRand));
                    //TODO handle composite
                } else if (parseOutput(conf, out_id)){
                    manObj->mOutputs.emplace_back(
                            new ManOutput(out_id, val));
                }
            }
            std::sort(manObj->mDefine.begin(), manObj->mDefine.end(),
                    ManDefineSorter());
            std::sort(manObj->mPropagate.begin(), manObj->mPropagate.end(),
                    ManPropagateSorter());
            std::sort(manObj->mInputs.begin(), manObj->mInputs.end(),
                    ManInputSorter());
            std::sort(manObj->mOutputs.begin(), manObj->mOutputs.end(),
                    ManOutputSorter());
        } catch (const std::exception& e){
            err.code = -1;
            err.message = "[Manager] ";
            err.message += e.what();
            return nullptr;
        }
        return manObj;
    }

    /********************************************************/
    void
    check(const ManagerObjects& manObj, manager::Error& err)
    {
        //check Define
        for (auto& vleDef : manObj.mDefine) {
            //check if exist in Input
            bool found = false;
            for (auto& vleIn : manObj.mInputs) {
                if (vleDef->getName() == vleIn->getName()) {
                    if (vleDef->to_add) {
                        found = true;
                    } else {
                        err.code = -1;
                        err.message = utils::format(
                                "[Manager]: error in define_X '%s': it cannot "
                                "be removed and declared as input at the same time",
                                vleDef->getName().c_str());
                        return ;
                    }
                }
            }
            //check if exist in Propagate
            for (auto& vleProp : manObj.mPropagate) {
                if (vleDef->getName() == vleProp->getName()) {
                    if (vleDef->to_add) {
                        found = true;
                    } else {
                        err.code = -1;
                        err.message = utils::format(
                                "[Manager]: error in define_X '%s': it cannot "
                                "be removed and declared as propagate at the same "
                                "time", vleDef->getName().c_str());
                        return ;
                    }
                }
            }
            //check if initialized
            if (vleDef->to_add and not found) {
                err.code = -1;
                err.message = utils::format(
                        "[Manager]: error in define_X '%s': it cannot "
                        "be added without initialization",
                        vleDef->getName().c_str());
                return ;
            }
        }

        //check Inputs
        unsigned int inputSize = 0;
        for (auto&  vleIn : manObj.mInputs) {
            //check input size which has to be consistent
            if (inputSize == 0 and vleIn->nbValues > 1) {
                inputSize = vleIn->nbValues;
            } else {
                if (vleIn->nbValues > 1 and inputSize > 0
                        and inputSize != vleIn->nbValues) {
                    err.code = -1;
                    err.message = utils::format(
                            "[Manager]: error in input values: wrong number"
                            " of values 1st input has %u values,  input %s has %u "
                            "values", inputSize, vleIn->getName().c_str(),
                            vleIn->nbValues);
                    return ;
                }
            }
            //check if already exist in replicate or propagate
            for (auto& vleRepl : manObj.mReplicates) {
                if (vleRepl->getName() == vleIn->getName()) {
                    err.code = -1;
                    err.message = utils::format(
                            "[Manager]: error input '%s' is also a replicate",
                            vleIn->getName().c_str());
                    return ;
                }
            }
            for (auto& vleProp : manObj.mPropagate) {
                if (vleProp->getName() == vleIn->getName()) {
                    err.code = -1;
                    err.message = utils::format(
                            "[Manager]: error input '%s' is also a propagate",
                            vleIn->getName().c_str());
                    return ;
                }
            }
        }
        //check Replicates
        unsigned int replSize = 0;
        for (auto& vleRepl : manObj.mReplicates) {
            //check repl size which has to be consistent
            if (replSize == 0 and vleRepl->nbValues > 1) {
                replSize = vleRepl->nbValues;
            } else {
                if (vleRepl->nbValues > 1 and replSize > 0
                        and replSize != vleRepl->nbValues) {
                    err.code = -1;
                    err.message = utils::format(
                            "[Manager]: error in replicate values: wrong number"
                            " of values 1st replicate has %u values, replicate %s "
                            "has %u values", replSize, vleRepl->getName().c_str(),
                            vleRepl->nbValues);
                    return ;
                }
            }
            //check if already exist in replicate or propagate
            for (auto& vleProp : manObj.mPropagate) {
                if (vleProp->getName() == vleRepl->getName()) {
                    err.code = -1;
                    err.message = utils::format(
                            "[Manager]: error replicate '%s' is also a "
                            "propagate", vleRepl->getName().c_str());
                    return ;
                }
            }
        }
    }

    /********************************************************/
    void
    configEmbedded(vpz::Vpz& model, wrapper_init& init, manager::Error& err,
            unsigned int input_index, unsigned int replicate_index)
    {
        std::unique_ptr<ManagerObjects> manObj = init_from_plan(init, err);
        if (err.code) return;
        check(*manObj, err);
        if (err.code) return;
        //update conditions
        post_define(model, manObj->mDefine, err);
        if (err.code) return;
        post_propagates(model, manObj->mPropagate, init, err);
        if (err.code) return;
        config_views(model, manObj->mOutputs, err);
        if (err.code) return;
        //check indices
        if (input_index >= manObj->inputsSize()) {
            err.code = -1;
            err.message = "[Manager] error in 'getEmbedded' config : ";
            err.message += " 'input_index' too big";
            return;
        }
        if (replicate_index >= manObj->replicasSize()) {
            err.code = -1;
            err.message = "[Manager] error in 'getEmbedded' config : ";
            err.message += " 'replicate_index' too big";
            return;
        }
        try {
            vpz::Conditions& conds = model.project().experiment().conditions();
            //post replica
            for (unsigned int i=0; i < manObj->mReplicates.size(); i++) {
                ManReplicate& tmp_repl = *manObj->mReplicates[i];
                vpz::Condition& condRep = conds.get(tmp_repl.cond);
                condRep.clearValueOfPort(tmp_repl.port);
                const value::Value& exp = tmp_repl.values(init);
                switch(exp.getType()) {
                case value::Value::SET:
                    condRep.addValueToPort(tmp_repl.port,
                            exp.toSet().get(replicate_index)->clone());
                    break;
                case value::Value::TUPLE:
                    condRep.addValueToPort(tmp_repl.port, value::Double::create(
                            exp.toTuple().at(replicate_index)));
                    break;
                default:
                    //error already detected
                    break;
                }
            }
            //post input
            for (unsigned int i=0; i < manObj->mInputs.size(); i++) {
                ManInput& tmp_input = *manObj->mInputs[i];
                vpz::Condition& condIn = conds.get(tmp_input.cond);
                condIn.clearValueOfPort(tmp_input.port);
                const value::Value& exp = tmp_input.values(init);
                switch (exp.getType()) {
                case value::Value::SET: {
                    condIn.addValueToPort(tmp_input.port,
                            exp.toSet().get(input_index)->clone());
                    break;
                } case value::Value::TUPLE: {
                    condIn.addValueToPort(tmp_input.port, value::Double::create(
                            exp.toTuple().at(input_index)));
                    break;
                } default: {
                    //error already detected
                    break;
                }}
            }
        } catch (const std::exception& e){
            err.code = -1;
            err.message = "[Manager] error in 'getEmbedded' config : ";
            err.message += e.what();
        }
    }


    /********************************************************/
    // run wrapper
    std::unique_ptr<value::Map>
    runPlan(std::unique_ptr<vpz::Vpz> model, wrapper_init& init,
            manager::Error& err)
    {
        std::unique_ptr<ManagerObjects> manObj = init_from_plan(init, err);
        if (err.code) {
            return nullptr;
        }
        check(*manObj, err);
        if (err.code) {
            return nullptr;
        }
        //launch simulations
        switch(mParalleloption) {
        case PARALLEL_MONO: {
            return run_without_parallelization(std::move(model),
                    std::move(manObj), init, err);
            break;
        } case PARALLEL_THREADS: {
            return run_with_threads(std::move(model),
                    std::move(manObj), init, err);
            break;
        } case PARALLEL_MPI: {
            return run_with_cvle(std::move(model),
                    std::move(manObj), init, err);
            break;
        }}
        return nullptr;
    }



};

Manager::Manager(utils::ContextPtr context,
        ParallelOptions paralleloption,      // type of parallelisation
        unsigned int nbslots,                // nbslots for parallelization
        SimulationOptions simulationoption,  // for 1 simulation
        std::chrono::milliseconds timeout,   // for 1 simulation
        bool removeMPIfiles,                 // for cvle
        bool generateMPIhost,                // for cvle
        const std::string& workingDir)       // for cvle
  : mPimpl(std::make_unique<Manager::Pimpl>(
          context, paralleloption, nbslots, simulationoption,
          timeout, removeMPIfiles, generateMPIhost, workingDir))
{}

Manager::Manager(utils::ContextPtr context)
  : mPimpl(std::make_unique<Manager::Pimpl>(context,
          PARALLEL_MONO,                                //paralleloption
          1,                                            //nbslots
          SIMULATION_SPAWN_PROCESS,                     //simulationoption,
          std::chrono::milliseconds(0),                 //timeout,
          true,                                         //removeMPIfiles,
          false,                                        //generateMPIhost,
          utils::Path::temp_directory_path().string())) //workingDir
{}

Manager::~Manager() = default;

std::unique_ptr<value::Matrix>
Manager::run(std::unique_ptr<vpz::Vpz> /*exp*/,
             uint32_t /*thread*/,
             uint32_t /*rank*/,
             uint32_t /*world*/,
             Error*/*error*/)
{
    std::unique_ptr<value::Matrix> result;

    //TODO not available anymore

    return result;
}

utils::Rand&
Manager::random_number_generator()
{
    return mPimpl->random_number_generator();
}

//specific runPlan signatures
std::unique_ptr<value::Map>
Manager::runPlan(std::shared_ptr<vle::value::Map> init
        , manager::Error& err)
{
    wrapper_init init_rec(init.get());
    std::unique_ptr<vpz::Vpz> model = build_embedded_model(
            init_rec, mPimpl->mContext, err);
    return mPimpl->runPlan(std::move(model), init_rec, err);
}

std::unique_ptr<value::Map>
Manager::runPlan(const vle::value::Map& init
        , manager::Error& err)
{
    wrapper_init init_rec(&init);
    std::unique_ptr<vpz::Vpz> model = build_embedded_model(
            init_rec, mPimpl->mContext, err);
    return mPimpl->runPlan(std::move(model), init_rec, err);
}

std::unique_ptr<value::Map>
Manager::runPlan(const vd::InitEventList& events
        , manager::Error& err)
{
    wrapper_init init_rec(&events);
    std::unique_ptr<vpz::Vpz> model = build_embedded_model(
            init_rec, mPimpl->mContext, err);
    return mPimpl->runPlan(std::move(model), init_rec, err);
}

std::unique_ptr<value::Map>
Manager::runPlan(std::unique_ptr<vpz::Vpz> model
        , std::shared_ptr<vle::value::Map> init
        , manager::Error& err)
{
    wrapper_init init_rec(init.get());
    return mPimpl->runPlan(std::move(model), init_rec, err);
}

std::unique_ptr<value::Map>
Manager::runPlan(std::unique_ptr<vpz::Vpz> model
        , const vle::value::Map& init
        , manager::Error& err)
{
    wrapper_init init_rec(&init);
    return mPimpl->runPlan(std::move(model), init_rec, err);
}

std::unique_ptr<value::Map>
Manager::runPlan(std::unique_ptr<vpz::Vpz> model
        , const vd::InitEventList& events
        , manager::Error& err)
{
    wrapper_init init_rec(&events);
    return mPimpl->runPlan(std::move(model), init_rec, err);
}

std::unique_ptr<vpz::Vpz>
Manager::getEmbedded(std::shared_ptr<vle::value::Map> init, Error& err,
        unsigned int input_index, unsigned int replicate_index)
{
    wrapper_init init_rec(init.get());
    std::unique_ptr<vpz::Vpz> model =
            build_embedded_model(init_rec, mPimpl->mContext, err);
    mPimpl->configEmbedded(*model, init_rec, err,
                           input_index, replicate_index);
    if (err.code) {
        return nullptr;
    } else {
        return model;
    }

}

std::unique_ptr<vpz::Vpz>
Manager::getEmbedded(const vle::value::Map& init, Error& err,
        unsigned int input_index, unsigned int replicate_index)
{
    wrapper_init init_rec(&init);
    std::unique_ptr<vpz::Vpz> model =
            build_embedded_model(init_rec, mPimpl->mContext, err);
    mPimpl->configEmbedded(*model, init_rec, err,
            input_index, replicate_index);
    if (err.code) {
        return nullptr;
    } else {
        return model;
    }
}

std::unique_ptr<vpz::Vpz>
Manager::getEmbedded(const devs::InitEventList& events, Error& err,
        unsigned int input_index, unsigned int replicate_index)
{
    wrapper_init init_rec(&events);
    std::unique_ptr<vpz::Vpz> model =
            build_embedded_model(init_rec, mPimpl->mContext, err);
    mPimpl->configEmbedded(*model, init_rec, err,
            input_index, replicate_index);
    if (err.code) {
        return nullptr;
    } else {
        return model;
    }
}

std::unique_ptr<vpz::Vpz>
Manager::getEmbedded(std::unique_ptr<vpz::Vpz> exp,
        std::shared_ptr<vle::value::Map> init, Error& err,
        unsigned int input_index, unsigned int replicate_index)
{
    wrapper_init init_rec(init.get());
    mPimpl->configEmbedded(*exp, init_rec, err,
            input_index, replicate_index);
    if (err.code) {
        return nullptr;
    } else {
        return std::move(exp);
    }
}

std::unique_ptr<vpz::Vpz>
Manager::getEmbedded(std::unique_ptr<vpz::Vpz> exp, const vle::value::Map& init,
        Error& err, unsigned int input_index, unsigned int replicate_index)
{
    wrapper_init init_rec(&init);
    mPimpl->configEmbedded(*exp, init_rec, err,
            input_index, replicate_index);
    if (err.code) {
        return nullptr;
    } else {
        return std::move(exp);
    }
}

std::unique_ptr<vpz::Vpz>
Manager::getEmbedded(std::unique_ptr<vpz::Vpz> exp,
        const devs::InitEventList& events, Error& err,
        unsigned int input_index, unsigned int replicate_index)
{
    wrapper_init init_rec(&events);
    mPimpl->configEmbedded(*exp, init_rec, err,
            input_index, replicate_index);
    if (err.code) {
        return nullptr;
    } else {
        return std::move(exp);
    }
}

//Manager utils
bool
Manager::parseInput(const std::string& conf,
        std::string& cond, std::string& port,
        const std::string& prefix)
{
    std::string varname;
    cond.clear();
    port.clear();
    if (prefix.size() > 0) {
        if (conf.compare(0, prefix.size(), prefix) != 0) {
            return false;
        }
        varname.assign(conf.substr(prefix.size(), conf.size()));
    } else {
        varname.assign(conf);
    }
    std::vector <std::string> tokens;
    utils::tokenize(varname, tokens, ".", false);
    if (tokens.size() != 2) {
        return false;
    }
    cond.assign(tokens[0]);
    port.assign(tokens[1]);
    return not cond.empty() and not port.empty();
}
bool
Manager::parseOutput(const std::string& conf, std::string& idout)
{
    idout.clear();
    std::string prefix = "output_";
    if (conf.compare(0, prefix.size(), prefix) != 0) {
        return false;
    }
    idout.assign(conf.substr(prefix.size(), conf.size()));
    return not idout.empty();
}


}
} // namespace vle manager
