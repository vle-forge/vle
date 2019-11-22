

#ifndef VLE_MANAGER_DETAILS_THREAD_SPECIFIC_HPP_
#define VLE_MANAGER_DETAILS_THREAD_SPECIFIC_HPP_

#include <iomanip>
#include <iostream>
#include <fstream>


namespace vle {
namespace manager {

/**
 * Specific log functor for threads
 *
 */
struct thread_log : vle::utils::Context::LogFunctor
{
    FILE* fp;
    unsigned int th;

    thread_log(unsigned int _th)
      : fp(nullptr)
      , th(_th)
    {}

    ~thread_log() override
    {
        if (fp)
            fclose(fp);
    }

    void write(const vle::utils::Context& /*ctx*/,
               int priority,
               const std::string& str) noexcept override
    {
        if (not fp) {
            std::stringstream s;
            s.imbue(std::locale::classic());
            s << "vle_manager_thread_" << th << ".log";
            fp = fopen(s.str().c_str(), "w");
        }

        if (fp) {
            if (priority == 7)
                fprintf(fp, "debug: %s", str.c_str());
            else if (priority == 4)
                fprintf(fp, "warning: %s", str.c_str());
            else if (priority == 3)
                fprintf(fp, "error: %s", str.c_str());
            else if (priority == 2)
                fprintf(fp, "critical: %s", str.c_str());
            else if (priority == 1)
                fprintf(fp, "alert: %s", str.c_str());
            else if (priority == 0)
                fprintf(fp, "emergency: %s", str.c_str());
            else
                fprintf(fp, "%s", str.c_str());
        }
    }

    void write(const vle::utils::Context& /*ctx*/,
               int priority,
               const char* format,
               va_list args) noexcept override
    {
        if (not fp) {
            std::stringstream s;
            s.imbue(std::locale::classic());
            s << "vle_manager_thread_" << th << ".log";
            fp = fopen(s.str().c_str(), "w");
        }

        if (fp) {
            if (priority == 7)
                fprintf(fp, "debug: ");
            else if (priority == 4)
                fprintf(fp, "warning: ");
            else if (priority == 3)
                fprintf(fp, "error: ");
            else if (priority == 2)
                fprintf(fp, "critical: ");
            else if (priority == 1)
                fprintf(fp, "alert: ");
            else if (priority == 0)
                fprintf(fp, "emergency: ");

            vfprintf(fp, format, args);
        }
    }
};


/**
 * The @c worker is a boost thread functor to execute threaded
 * source code.
 *
 */
struct thread_worker
{
    utils::ContextPtr mContext;
    const vpz::Vpz& mVpz;
    const wrapper_init& mInit;
    const ManagerObjects& mManObjs;
    std::vector<std::unique_ptr<ManOutput>>& mOutputs;//to fill
    value::Map&               mResults;//to fill
    std::mutex&                mMutex;
    std::chrono::milliseconds mTimeout;
    SimulationOptions         mSimulationOption;
    uint32_t                  mIndex;
    uint32_t                  mThreads;
    Error&                    mError;//tofill

    thread_worker(utils::ContextPtr context,
            const vpz::Vpz& vpz,
            const wrapper_init& init,
            const ManagerObjects& manObjs,
            std::vector<std::unique_ptr<ManOutput>>& outputs,//to fill
            value::Map& results,
            std::mutex& results_mutex,
            std::chrono::milliseconds timeout,
            SimulationOptions simulationOption,
            uint32_t index,
            uint32_t threads,
            Error& error):
                mContext(context), mVpz(vpz), mInit(init), mManObjs(manObjs),
                mOutputs(outputs), mResults(results), mMutex(results_mutex),
                mTimeout(timeout), mSimulationOption(simulationOption),
                mIndex(index), mThreads(threads), mError(error)
    {}

    ~thread_worker() = default;

    void operator()()
    {
        unsigned int N = mManObjs.inputsSize();
        unsigned int M = mManObjs.replicasSize();

        std::shared_ptr<value::Value> temp_val;
        for (unsigned int  i =  mIndex; i < M*N; i += mThreads) {
            unsigned int inputIndex = i / M;
            unsigned int replIndex = i % M;
            Simulation sim(mContext, LOG_NONE, //TODO remove
                    mSimulationOption, mTimeout, nullptr);

            std::unique_ptr<vpz::Vpz> vpz_loc(new vpz::Vpz(mVpz));

            for (auto& tmp_input : mManObjs.mInputs) {
                const value::Value& exp = tmp_input->values(mInit);
                switch(exp.getType()) {
                case value::Value::SET:
                    temp_val.reset(
                            exp.toSet().get(inputIndex)->clone().release());
                    break;
                case value::Value::TUPLE:
                    temp_val.reset(new value::Double(
                            exp.toTuple().at(inputIndex)));
                    break;
                default:
                    if (N == 1) {
                        temp_val.reset(exp.clone().release());
                    } else {
                        mError.code = -1;
                        mError.message = "[Manager] error thread";
                        return ;
                    }
                    break;
                }
                vpz_loc->project().experiment().conditions()
                                  .get(tmp_input->cond)
                                  .setValueToPort(tmp_input->port, temp_val);
            }
            for (auto& tmp_repl : mManObjs.mReplicates) {
                const value::Value& exp = tmp_repl->values(mInit);
                switch(exp.getType()) {
                case value::Value::SET:
                    temp_val.reset(
                            exp.toSet().get(replIndex)->clone().release());
                    break;
                case value::Value::TUPLE:
                    temp_val.reset(new value::Double(
                            exp.toTuple().at(replIndex)));
                    break;
                default:
                    if (M == 1) {
                        temp_val.reset(exp.clone().release());
                    } else {
                        mError.code = -1;
                        mError.message = "[Manager] error thread";
                        return ;
                    }
                }
                vpz_loc->project().experiment().conditions()
                                  .get(tmp_repl->cond)
                                  .setValueToPort(tmp_repl->port, temp_val);
            }
            Error error_loc;
            auto simresult = sim.run(std::move(vpz_loc), &error_loc);

            //////////////////
            mMutex.lock();
            if (mError.code) {
                mMutex.unlock();
                return;
            }
            if (error_loc.code) {
                mError.code = error_loc.code;
                mError.message = "[Manager error] simulation input="+
                        std::to_string(inputIndex)+
                        ", replicate="+std::to_string(replIndex)+" : "+
                        error_loc.message;
                mMutex.unlock();
                return ;
            }
            try {
                std::unique_ptr<value::Value> aggr_value;
                for (unsigned int out=0; out<mOutputs.size(); out++) {
                    ManOutput& mout = *mOutputs[out];
                    aggr_value = mout.insertReplicate(*simresult,
                            inputIndex, N, M);
                    if (aggr_value) {
                        mResults.set(mout.id, std::move(aggr_value));
                    }
                }
            } catch ( const std::exception& e) {
                mError.code = -1;
                mError.message = "[Manager error] aggregation input="+
                        std::to_string(inputIndex)+
                        ", replicate="+std::to_string(replIndex)+" : "+
                        e.what();
                mMutex.unlock();
                return ;
            }
            mMutex.unlock();
        }
    }
};

}
} // namespace vle manager

#endif
