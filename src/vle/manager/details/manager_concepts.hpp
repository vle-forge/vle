#ifndef VLE_MANAGER_DETAILS_MANAGER_CONCEPTS_HPP_
#define VLE_MANAGER_DETAILS_MANAGER_CONCEPTS_HPP_

#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/String.hpp>
#include <vle/utils/Package.hpp>

#include "manager/details/accu_multi.hpp"

namespace vle {
namespace manager {

//only for run(Map) functions
enum INTEGRATION_TYPE {LAST, MAX, SUM, MSE, ALL};

struct ManDefine
{
    /*
     * @brief ManDefine a cond.port to add or remove.
     * @param cond, id of the cond
     * @param port, id of the port
     * @param val, true (add) or false (remove)
     */
    ManDefine(const std::string& _cond, const std::string& _port,
            const vle::value::Value& val);

    std::string getName() const;

    std::string cond;
    std::string port;
    bool to_add;
};

struct ManDefineSorter
{
    bool operator() (const std::unique_ptr<ManDefine>& i,
            const std::unique_ptr<ManDefine>& j)
    {
        return (i->getName()<j->getName());
    }
};


struct ManPropagate
{
    /*
     * @brief ManPropagate specifies the value to set for all simulations
     *        to a port condition of the embedded simulator
     * @param cond, id of the cond
     * @param port, id of the port
     */
    ManPropagate(const std::string& _cond, const std::string& _port);

    virtual ~ManPropagate();

    /**
     * @brief Gets the value for this propagation input
     * @param init, the initialization map
     *            (either devs:InitEventList or value::Map)
     */

    const vle::value::Value& value(const wrapper_init& init);

    std::string getName() const;

    std::string cond;
    std::string port;

};

struct ManPropagateSorter
{
    bool operator() (const std::unique_ptr<ManPropagate>& i,
            const std::unique_ptr<ManPropagate>& j)
    {
        return (i->getName()<j->getName());
    }
};

struct ManInput
{
    /*
     * @brief ManInput identifies an input of the experiment plan
     * @param cond, id of the cond
     * @param port, id of the port
     * @param val, the value given on port conf_name, is either
     *  a value::Set or value::Tuple that identifies an experiment plan
     * @param rn, a random number generator
     */
    ManInput(const std::string& _cond, const std::string& _port,
            const vle::value::Value& val, utils::Rand& rn);

    virtual ~ManInput();

    /**
     * @brief Gets the experiment plan from initialization map
     * @param init, the initialization map
     */
    const vle::value::Value& values(const wrapper_init& init) const;

    std::string getName() const;

    std::string cond;
    std::string port;
    unsigned int nbValues;
    //only for distribution configuration
    std::unique_ptr<value::Tuple> mvalues;
};

struct ManInputSorter
{
    bool operator() (const std::unique_ptr<ManInput>& i,
            const std::unique_ptr<ManInput>& j)
    {
        return (i->getName()<j->getName());
    }
};

struct ManReplicate
{
    /*
     * @brief ManReplicate identifies the replicate input
     * @param cond, id of the cond
     * @param port, id of the port
     * @param val, the value given on port conf_name, is either
     * a value::Set or value::Tuple that identifies an experiment plan
     * @param rn, a random number generator
     */
    ManReplicate(const std::string& _cond, const std::string& _port,
            const vle::value::Value& val, utils::Rand& rn);


    virtual ~ManReplicate();

    /**
     * @brief Gets the experiment plan from initialization map
     * @param init, the initialization map
     */
    const vle::value::Value& values(const wrapper_init& init) const;

    std::string getName() const;

    std::string cond;
    std::string port;
    unsigned int nbValues;
    //only for distribution configuration
    std::unique_ptr<value::Tuple> mvalues;
};


class ManOutput;
/**
 * Delegate output functionnalities
 */
class DelegateOut
{
public:
    DelegateOut(ManOutput& vleout, bool managedouble):
        vleOut(vleout), manageDouble(managedouble)
    {
    }

    virtual ~DelegateOut() {}


    virtual std::unique_ptr<vle::value::Value>
    insertReplicate(vle::value::Matrix& outMat, unsigned int currInput) = 0;

    /**
     * Temporal integration, shared with other delegates
     * @param [in] the vle output
     * @param [in] the output matrix
     * @return the temporal integration of the output
     */
    static std::unique_ptr<value::Value> integrateReplicate(ManOutput& vleout,
            vle::value::Matrix& outMat);

    static AccuMulti& getAccu(std::map<int, std::unique_ptr<AccuMulti>>& accus,
            unsigned int index, const ManOutput& vleout);



    static AccuMono& getAccu(std::map<int, std::unique_ptr<AccuMono>>& accu,
            unsigned int index, const ManOutput& vleout);


    ManOutput& vleOut;
    bool manageDouble;
};

/**
 * Delegate output for standard (no all integration nor all aggregation_input)
 */
class DelOutStd : public DelegateOut
{
public:
    DelOutStd(ManOutput& vleout);

    std::unique_ptr<vle::value::Value> insertReplicate(
            vle::value::Matrix& outMat, unsigned int currInput) override;

    //for replicate aggregation for current input index
    std::map<int, std::unique_ptr<AccuMono>> mreplicateAccu;
    std::unique_ptr<AccuMono> minputAccu;
};

/**
 * Delegate output for both integration and aggregation_input to 'all'
 */
class DelOutIntAggrALL : public DelegateOut
{
public:
    DelOutIntAggrALL(ManOutput& vleout, bool managedouble);

    std::unique_ptr<vle::value::Value> insertReplicate(
            vle::value::Matrix& outMat, unsigned int currInput) override;

    //for replicate aggregation for current input index
    std::map<int, std::unique_ptr<AccuMulti>> mreplicateAccu;
    std::unique_ptr<value::Value> minputAccu;
    unsigned int nbInputsFilled;
};

/**
 * Delegate output for integration set to 'all'
 */
class DelOutIntALL : public DelegateOut
{
public:
    DelOutIntALL(ManOutput& vleout);

    std::unique_ptr<vle::value::Value> insertReplicate(
            vle::value::Matrix& outMat, unsigned int currInput) override;

    //for replicate aggregation for current input index
    std::map<int, std::unique_ptr<AccuMulti>> mreplicateAccu;
    std::unique_ptr<AccuMulti> minputAccu;
};

/**
 * Delegate output for aggregation_input set to 'all'
 */
class DelOutAggrALL : public DelegateOut
{
public:
    DelOutAggrALL(ManOutput& vleout, bool managedouble);

    std::unique_ptr<vle::value::Value> insertReplicate(
            vle::value::Matrix& outMat, unsigned int currInput) override;

    //for replicate aggregation for current input index
    std::map<int, std::unique_ptr<AccuMono>> mreplicateAccu;
    std::unique_ptr<value::Value> minputAccu;
    unsigned int nbInputsFilled;
};

/**
 * Default interface for ManOutput
 */
class ManOutput
{
public:
    ManOutput();

    ManOutput(const std::string& _id, const vle::value::Value& val);

    virtual ~ManOutput();

    std::string getId() const;

    /**
     * @brief extract atom path and port from absolutePort
     * @param[out] atomPath, the model path into the hierarchy
     * @param[out] port, observable port of the output
     * @return true if extraction is ok
     *
     * @example
     *  if absolutePort is 'Coupled:Atomic.Port' then
     *  atomPath = 'Atomic'
     *  port = 'Port'
     */
    bool extractAtomPathAndPort(std::string& atomPath, std::string& port);

    /**
     * @brief insert a replicate from a map of views
     * @param result, one simulation result (map of views)
     * @param currInput, current input index
     * @param nbInputs, nb inputs of the experiment plan (for allocation)
     * @param nbReplicates, nb replicates of the experiment plan
     * @return the input aggregated value if all inputs and all replicates
     * have aggregated
     */
    std::unique_ptr<value::Value> insertReplicate(value::Map& result,
            unsigned int currInput, unsigned int nbInputs,
            unsigned int nbReplicates);

    /**
     * @brief insert a replicate from a view
     * @param result, one view (matrix) from one simulation result
     * @param currInput, current input index
     * @param nbIn, nb inputs of the experiment plan (for allocation)
     * @param nbRepl, nb replicates of the experiment plan
     * @return the input aggregated value if all inputs and all replicates
     * have aggregated
     */
    std::unique_ptr<value::Value>
    insertReplicate(vle::value::Matrix& outMat, unsigned int currInput,
            unsigned int nbIn, unsigned int nbRepl);

    bool parsePath(const std::string& path);

    std::string id;
    std::string view;
    std::string absolutePort;//of the form Coupled:Atomic.Port
    int colIndex;
    bool shared;
    INTEGRATION_TYPE integrationType;
    AccuStat replicateAggregationType;
    AccuStat inputAggregationType;
    unsigned int nbInputs;
    unsigned int nbReplicates;
    std::unique_ptr<DelegateOut> delegate;
    //optional for integration == MSE only
    std::unique_ptr<vle::value::Tuple> mse_times;
    std::unique_ptr<vle::value::Tuple> mse_observations;
    //optionnal for aggregate_replicate = "quantile"
    double replicateAggregationQuantile;
};

struct ManOutputSorter
{
    bool operator() (const std::unique_ptr<ManOutput>& i,
            const std::unique_ptr<ManOutput>& j)
    {
        return (i->getId()<j->getId());
    }
};

struct ManagerObjects
{
    //cond.port
    std::vector<std::unique_ptr<ManDefine>> mDefine;
    std::vector<std::unique_ptr<ManPropagate>> mPropagate;
    std::vector<std::unique_ptr<ManInput>> mInputs;
    std::vector<std::unique_ptr<ManReplicate>> mReplicates;
    //view/path_to_atomic.port
    std::vector<std::unique_ptr<ManOutput>> mOutputs;

    unsigned int inputsSize() const
    {
        if (mInputs.size() > 0) {
            return mInputs[0]->nbValues;
        }
        return 1;
    }

    unsigned int replicasSize() const
    {
        if (mReplicates.size() == 0) {
            return 1;
        }
        return mReplicates[0]->nbValues;
    }

    unsigned int nbInputs() const
    {
        return mInputs.size();
    }

    unsigned int nbReplicas() const
    {
        return mReplicates.size();
    }
};

//static functions

/**
 * @brief Build a tuple value from a distribution
 *
 * @param[in]     distr, vle::value::Map specifying distribution and params
 * @param[in/out] rn, the random number generator
 *
 * @return the double avalues generated form distribution
 */
std::unique_ptr<value::Tuple>
valuesFromDistrib(const value::Map& distrib, utils::Rand& rn)
{

    if (distrib.exist("distribution")
            and distrib.get("distribution")->isString()) {
        if (distrib.getString("distribution") == "uniform") {
            if (distrib.exist("nb") and distrib.exist("min")
                    and distrib.exist("max")
                    and distrib.get("nb")->isInteger()
                    and distrib.get("min")->isDouble()
                    and distrib.get("max")->isDouble()) {
                std::unique_ptr<value::Tuple> res(new value::Tuple(
                        distrib.getInt("nb")));
                double min = distrib.getDouble("min");
                double max = distrib.getDouble("max");
                std::vector<double>::iterator itb = res->value().begin();
                std::vector<double>::iterator ite = res->value().end();
                for (; itb != ite; itb++) {
                    *itb = rn.getDouble(min, max);
                }
                return std::move(res);
            }
        }
    }
    return nullptr;
}


/***********************
 * Implementation
 ***********************/


/////////// ManDefine

ManDefine::ManDefine(const std::string& _cond, const std::string& _port,
        const vle::value::Value& val):
  cond(_cond), port(_port), to_add(true)
{
    if (val.isBoolean()) {
        to_add = val.toBoolean().value();
    } else {
        throw utils::ArgError(utils::format(
                "[Manager] : the define_X has wrong form: '%s.%s'",
                cond.c_str(),  port.c_str()));
    }
}

std::string
ManDefine::getName() const
{
    std::string ret = cond;
    ret.append(".");
    ret.append(port);
    return ret;
}

////////// ManPropagate

ManPropagate::ManPropagate(const std::string& _cond, const std::string& _port):
                cond(_cond), port(_port)
{
    if (cond.empty() or port.empty()) {
        throw utils::ArgError(utils::format(
                "[Manager] : the propagate input has wrong form: '%s.%s'",
                cond.c_str(),  port.c_str()));
    }
}

ManPropagate::~ManPropagate() {}

const vle::value::Value&
ManPropagate::value(const wrapper_init& init)
{
    std::string key("propagate_");
    key.append(getName());
    bool status = true;
    return init.get(key, status);
}

std::string
ManPropagate::getName() const
{
    std::string ret = cond;
    ret.append(".");
    ret.append(port);
    return ret;
}

////////// ManInput

ManInput::ManInput(const std::string& _cond, const std::string& _port,
        const vle::value::Value& val, utils::Rand& rn):
                        cond(_cond), port(_port), nbValues(0)
{
    if (cond.empty() or port.empty()) {
        throw utils::ArgError(utils::format(
                "[Manager] : the input has wrong form: '%s.%s'",
                cond.c_str(),  port.c_str()));
    }
    switch (val.getType()) {
    case value::Value::TUPLE:
        nbValues = val.toTuple().size();
        break;
    case value::Value::SET:
        nbValues = val.toSet().size();
        break;
    case value::Value::MAP: {
        mvalues = valuesFromDistrib(val.toMap(), rn);
        if (mvalues) {
            nbValues = mvalues->size();
        } else {
            nbValues = 1;
        }
        break;
    } default:
        nbValues = 1;
        break;
    }
}

ManInput::~ManInput() {}

const vle::value::Value&
ManInput::values(const wrapper_init& init) const
{
    if (mvalues){
        return *mvalues;
    } else {
        std::string key("input_");
        key.append(getName());
        bool status = true;
        return init.get(key, status);
    }
}

std::string
ManInput::getName() const
{
    std::string ret = cond;
    ret.append(".");
    ret.append(port);
    return ret;
}

////////// ManReplicate


ManReplicate::ManReplicate(const std::string& _cond, const std::string& _port,
        const vle::value::Value& val, utils::Rand& rn):
                        cond(_cond), port(_port), nbValues(0)
{
    bool err = false;
    if (cond.empty() or port.empty()) {
        throw utils::ArgError(utils::format(
                "[Manager] : the replicate has wrong form: '%s.%s'",
                cond.c_str(),  port.c_str()));
    }
    switch (val.getType()) {
    case value::Value::TUPLE:
        nbValues = val.toTuple().size();
        break;
    case value::Value::SET:
        nbValues = val.toSet().size();
        break;
    case value::Value::MAP: {
        mvalues = valuesFromDistrib(val.toMap(), rn);
        if (mvalues) {
            nbValues = mvalues->size();
        } else {
            err = true;
        }
        break;
    } default: {
        err = true;
        break;
    }}
    if (err) {
        throw utils::ArgError(utils::format(
                "[Manager] : error in configuration of "
                "'replicate_%s.%s', expect a value::Set or Tuple",
                cond.c_str(),  port.c_str()));
    }
}


ManReplicate::~ManReplicate() {}

const vle::value::Value&
ManReplicate::values(const wrapper_init& init) const
{
    if (mvalues){
        return *mvalues;
    } else {
        std::string key("replicate_");
        key.append(getName());
        bool status = true;
        return init.get(key, status);
    }
}

std::string
ManReplicate::getName() const
{
    std::string ret = cond;
    ret.append(".");
    ret.append(port);
    return ret;
}


////////// DelegateOut

std::unique_ptr<value::Value>
DelegateOut::integrateReplicate(ManOutput& vleout, vle::value::Matrix& outMat)
{
    switch(vleout.integrationType) {
    case MAX: {
        double max = -9999;
        for (unsigned int i=1; i < outMat.rows(); i++) {
            double v = outMat.getDouble(vleout.colIndex, i);
            if (v > max) {
                max = v;
            }
        }
        return value::Double::create(max);
        break;
    } case SUM: {
        double sum = 0;
        for (unsigned int i=1; i < outMat.rows(); i++) {
            sum += outMat.getDouble(vleout.colIndex, i);
        }
        return value::Double::create(sum);
        break;
    } case LAST: {
        if (vleout.shared) {
            const std::unique_ptr<value::Value>& res =
                    outMat.get(vleout.colIndex, outMat.rows() - 1);
            return res->clone();
        } else {
            return std::move(outMat.give(vleout.colIndex, outMat.rows() - 1));
        }
        break;
    } case MSE: {
        double sum_square_error = 0;
        double nbVal = 0;
        for (unsigned int i=0; i< vleout.mse_times->size(); i++) {
            int t = std::floor(vleout.mse_times->at(i));
            if (t >= 0 and (t+1) < (int) outMat.rows()) {
                sum_square_error += std::pow(
                        (outMat.getDouble(vleout.colIndex,t+1)
                                - vleout.mse_observations->at(i)), 2);
                nbVal++;
            }
        }
        return value::Double::create(sum_square_error/nbVal);
        break;
    } default:{
        //not possible
        break;
    }}
    return nullptr;
}

AccuMulti&
DelegateOut::getAccu(std::map<int, std::unique_ptr<AccuMulti>>& accus,
        unsigned int index, const ManOutput& vleout)
{
    std::map<int, std::unique_ptr<AccuMulti>>::iterator itf =
            accus.find(index);
    if (itf != accus.end()) {
        return *(itf->second);
    }
    std::unique_ptr<AccuMulti> ptr(new AccuMulti(
            vleout.replicateAggregationType));
    AccuMulti& ref = *ptr;
    ref.setDefaultQuantile(vleout.replicateAggregationQuantile);
    accus.insert(std::make_pair(index, std::move(ptr)));
    return ref;
}

AccuMono&
DelegateOut::getAccu(std::map<int, std::unique_ptr<AccuMono>>& accu,
        unsigned int index, const ManOutput& vleout)
{
    std::map<int, std::unique_ptr<AccuMono>>::iterator itf =
            accu.find(index);
    if (itf != accu.end()) {
        return *(itf->second);
    }
    std::unique_ptr<AccuMono> ptr(new AccuMono(
            vleout.replicateAggregationType));
    AccuMono& ref = *ptr;
    ref.setDefaultQuantile(vleout.replicateAggregationQuantile);
    accu.insert(std::make_pair(index, std::move(ptr)));
    return ref;
}

DelOutStd::DelOutStd(ManOutput& vleout) : DelegateOut(vleout, true)
{
    minputAccu.reset(new AccuMono(vleOut.inputAggregationType));
}


std::unique_ptr<vle::value::Value>
DelOutStd::insertReplicate(vle::value::Matrix& outMat, unsigned int currInput)
{
    //start insertion for double management only
    std::unique_ptr<value::Value> intVal = std::move(
            integrateReplicate(vleOut, outMat));
    if (vleOut.nbReplicates == 1) {
        minputAccu->insert(intVal->toDouble().value());
    } else {
        AccuMono& accuRepl = DelegateOut::getAccu(mreplicateAccu, currInput,
                vleOut);
        accuRepl.insert(intVal->toDouble().value());
        //test if aggregating replicates is finished
        if (accuRepl.count() == vleOut.nbReplicates) {
            minputAccu->insert(accuRepl.getStat(
                    vleOut.replicateAggregationType));
            mreplicateAccu.erase(currInput);

        }
    }
    //test if aggregating inputs is finished
    if (minputAccu->count() == vleOut.nbInputs) {
        double res = minputAccu->getStat(vleOut.inputAggregationType);
        minputAccu.reset(nullptr);
        return value::Double::create(res);
    }
    return nullptr;
}

DelOutIntAggrALL::DelOutIntAggrALL(ManOutput& vleout, bool managedouble):
        DelegateOut(vleout, managedouble), mreplicateAccu(),
        minputAccu(nullptr), nbInputsFilled(0)
{
}

std::unique_ptr<vle::value::Value>
DelOutIntAggrALL::insertReplicate(vle::value::Matrix& outMat, unsigned int currInput)
{
    if (not minputAccu) {
        if (manageDouble) {
            minputAccu.reset(new value::Table(vleOut.nbInputs,
                    outMat.rows()-1));
        } else {
            minputAccu.reset(new value::Matrix(vleOut.nbInputs,
                    outMat.rows()-1, 10, 10));
        }
    }
    //resize if necessary
    if (manageDouble) {
        if (minputAccu->toTable().height() < outMat.rows()) {
            minputAccu->toTable().resize(vleOut.nbInputs, outMat.rows()-1);
        }
    } else {
        if (minputAccu->toMatrix().rows() < outMat.rows()) {
            minputAccu->toMatrix().resize(vleOut.nbInputs, outMat.rows()-1);
        }
    }
    //insert
    if (vleOut.nbReplicates == 1){//one can put directly into results
        for (unsigned int i=1; i < outMat.rows(); i++) {
            if (manageDouble) {
                minputAccu->toTable().get(currInput, i-1) =
                        outMat.getDouble(vleOut.colIndex, i);
            } else if (vleOut.shared) {
                minputAccu->toMatrix().set(currInput, i-1,
                        outMat.get(vleOut.colIndex, i)->clone());
            } else {
                minputAccu->toMatrix().set(currInput, i-1,
                        std::move(outMat.give(vleOut.colIndex, i)));
            }
        }
        nbInputsFilled++;
    } else {
        AccuMulti& accuRepl = DelegateOut::getAccu(mreplicateAccu, currInput,
                vleOut);
        accuRepl.insertColumn(outMat, vleOut.colIndex);
        if (accuRepl.count() == vleOut.nbReplicates) {
            accuRepl.fillStat(minputAccu->toTable(),
                    currInput, vleOut.replicateAggregationType);
            mreplicateAccu.erase(currInput);
            nbInputsFilled++;
        }
    }
    if (nbInputsFilled == vleOut.nbInputs) {
        return std::move(minputAccu);
    }
    return nullptr;
}

DelOutIntALL::DelOutIntALL(ManOutput& vleout): DelegateOut(vleout, true),
    mreplicateAccu(), minputAccu(nullptr)
{
}

std::unique_ptr<vle::value::Value>
DelOutIntALL::insertReplicate(vle::value::Matrix& outMat, unsigned int currInput)
{
    if (not minputAccu) {
        minputAccu.reset(new AccuMulti(vleOut.inputAggregationType));
    }
    if (vleOut.nbReplicates == 1){//one can put directly into results
        minputAccu->insertColumn(outMat, vleOut.colIndex);
    } else {
        AccuMulti& accuRepl = DelegateOut::getAccu(mreplicateAccu, currInput,
                vleOut);
        accuRepl.insertColumn(outMat, vleOut.colIndex);
        if (accuRepl.count() == vleOut.nbReplicates) {
            minputAccu->insertAccuStat(accuRepl,
                    vleOut.replicateAggregationType);
            mreplicateAccu.erase(currInput);
        }
    }
    if (minputAccu->count() == vleOut.nbInputs) {
        std::unique_ptr<value::Table> res(new value::Table(1,
                minputAccu->size()));
        minputAccu->fillStat(*res, 0, vleOut.inputAggregationType);
        return std::move(res);
    }
    return nullptr;
}

DelOutAggrALL::DelOutAggrALL(ManOutput& vleout, bool managedouble):
        DelegateOut(vleout, managedouble), mreplicateAccu(),
        minputAccu(nullptr), nbInputsFilled(0)
{
    if (manageDouble) {
        minputAccu.reset(new value::Table(vleOut.nbInputs,1));
    } else {
        minputAccu.reset(new value::Matrix(vleOut.nbInputs,1,10,10));
    }
}

std::unique_ptr<vle::value::Value>
DelOutAggrALL::insertReplicate(vle::value::Matrix& outMat, unsigned int currInput)
{
    std::unique_ptr<value::Value> intVal = std::move(
            integrateReplicate(vleOut, outMat));

    if (vleOut.nbReplicates == 1){//one can put directly into results
        if (manageDouble) {
            minputAccu->toTable().get(currInput, 0) =
                    intVal->toDouble().value();
        } else {
            minputAccu->toMatrix().set(currInput, 0, std::move(intVal));

        }

        nbInputsFilled++;
    } else {
        AccuMono& accuRepl = DelegateOut::getAccu(mreplicateAccu, currInput,
                vleOut);
        accuRepl.insert(intVal->toDouble().value());
        if (accuRepl.count() == vleOut.nbReplicates) {
            minputAccu->toTable().get(currInput, 0)=
                    accuRepl.getStat(vleOut.replicateAggregationType);
            mreplicateAccu.erase(currInput);
            nbInputsFilled++;
        }
    }
    if (nbInputsFilled == vleOut.nbInputs) {
        return std::move(minputAccu);
    }
    return nullptr;
}


ManOutput::ManOutput() : id(), view(), absolutePort(), colIndex(-1),
        shared(true), integrationType(LAST), replicateAggregationType(S_mean),
        inputAggregationType(S_at), nbInputs(0), nbReplicates(0),
        delegate(nullptr), mse_times(nullptr), mse_observations(nullptr),
        replicateAggregationQuantile(0.5)
{
}

ManOutput::ManOutput(const std::string& _id, const vle::value::Value& val) :
        id(_id), view(), absolutePort(),  colIndex(-1), shared(true),
        integrationType(LAST), replicateAggregationType(S_mean),
        inputAggregationType(S_at), nbInputs(0), nbReplicates(0),
        delegate(nullptr), mse_times(nullptr), mse_observations(nullptr),
        replicateAggregationQuantile(0.5)
{
    std::string tmp;
    if (val.isString()) {
        if (not parsePath(val.toString().value())) {
            throw utils::ArgError(utils::format(
                    "[Manager] : error in configuration of the output "
                    "'output_%s' with a string; got: '%s'",
                    id.c_str(),  val.toString().value().c_str()));
        }
    } else if (val.isMap()) {
        const value::Map& m = val.toMap();
        bool error = false;
        if (m.exist("path")) {
            if (not parsePath(m.getString("path"))) {
                throw utils::ArgError(utils::format(
                        "[Manager] : error in configuration of the output "
                        " '%s%s' when parsing the path",
                        "output_",  id.c_str()));
            }
        }
        if (m.exist("aggregation_replicate")) {
            tmp = m.getString("aggregation_replicate");
            if (tmp == "mean") {
                replicateAggregationType = S_mean;
            } else if (tmp == "quantile"){
                replicateAggregationType = S_quantile;
                if (m.exist("replicate_quantile")) {
                    replicateAggregationQuantile = m.getDouble(
                            "replicate_quantile");
                }
            } else if (tmp == "variance"){
                replicateAggregationType = S_variance;
            } else {
                error = true;
            }
        }
        if (m.exist("aggregation_input")) {
            tmp = m.getString("aggregation_input");
            if (tmp == "mean") {
                inputAggregationType = S_mean;
            } else if (tmp == "quantile"){
                inputAggregationType = S_quantile;
            } else if (tmp == "max"){
                inputAggregationType = S_max;
            } else if (tmp == "all"){
                inputAggregationType = S_at;
            } else {
                throw utils::ArgError(utils::format(
                        "[Manager] : error in configuration of the output "
                        "'%s%s' when parsing the aggregation type of inputs",
                        "output_",  id.c_str()));
            }
        }
        if (not error) {
            if (m.exist("integration")) {
                tmp = m.getString("integration");
                if (tmp == "last") {
                    integrationType = LAST;
                } else if(tmp == "max") {
                    integrationType = MAX;
                } else if(tmp == "sum") {
                    integrationType = SUM;
                } else if(tmp == "mse") {
                    integrationType = MSE;
                } else if(tmp == "all") {
                    integrationType = ALL;
                } else {
                    throw utils::ArgError(utils::format(
                            "[Manager] : error in configuration of the output "
                            "'%s%s' when parsing the integration type: '%s'",
                            "output_",  id.c_str(), tmp.c_str()));
                }
            } else {
                integrationType = LAST;
            }
        }
        if (not error and integrationType == MSE) {
            if (not m.exist("mse_times") or
                    not m.exist("mse_observations")) {
                error = true;
            } else {
                mse_times.reset(new value::Tuple(m.getTuple("mse_times")));
                mse_observations.reset(new value::Tuple(
                        m.getTuple("mse_observations")));
                error = mse_times->size() != mse_observations->size();
            }
        }
        if (error) {
            throw utils::ArgError(utils::format(
                    "[Manager] : error in configuration of the output "
                    "'%s%s' with a map",
                    "output_",  id.c_str()));
        }
    }
}

ManOutput::~ManOutput() {}

std::string
ManOutput::getId() const
{
    return id;
}


bool
ManOutput::extractAtomPathAndPort(std::string& atomPath, std::string& port)
{
    std::vector<std::string> tokens;
    utils::tokenize(absolutePort, tokens, ".", false);
    if (tokens.size() != 2) {
        atomPath.clear();
        port.clear();
        return false;
    }
    atomPath.assign(tokens[0]);
    port.assign(tokens[1]);
    tokens.clear();
    utils::tokenize(atomPath, tokens, ":", false);
    if (tokens.size() != 2) {
        atomPath.clear();
        port.clear();
        return false;
    }
    atomPath.assign(tokens[0]);
    std::string atomName = tokens[1];
    tokens.clear();
    utils::tokenize(atomPath, tokens, ",", false);
    atomPath.clear();
    for (unsigned int i=1; i<tokens.size(); i++) {
        //note: first cpled model is removed in order
        //to use BaseModel::findModelFromPath
        atomPath.append(tokens[i]);
        atomPath.append(",");
    }
    atomPath.append(atomName);
    return true;
}

std::unique_ptr<value::Value>
ManOutput::insertReplicate(value::Map& result,
        unsigned int currInput, unsigned int nbInputs,
        unsigned int nbReplicates)
{
    value::Map::iterator it = result.find(view);
    if (it == result.end()) {
        throw vu::ArgError(utils::format(
                "[Manager] view '%s' not found)",
                view.c_str()));
    }
    value::Matrix& outMat = value::toMatrixValue(*it->second);
    return insertReplicate(outMat, currInput, nbInputs, nbReplicates);
}


std::unique_ptr<value::Value>
ManOutput::insertReplicate(vle::value::Matrix& outMat, unsigned int currInput,
        unsigned int nbIn, unsigned int nbRepl)
{
    if (not delegate){
        nbReplicates = nbRepl;
        nbInputs = nbIn;
        //performs some checks on output matrix
        if (outMat.rows() < 2){
            throw vu::ArgError("[Manager] expect at least 2 rows");
        }
        //get col index
        colIndex = 9999;
        for (unsigned int i=0; i < outMat.columns(); i++) {
            if (outMat.getString(i,0) == absolutePort) {
                colIndex = i;
            }
        }
        if (colIndex == 9999) {
            throw vu::ArgError(utils::format(
                    "[Manager] view.port '%s' not found)",
                    absolutePort.c_str()));
        }
        bool manageDouble = true;
        if (not outMat.get(colIndex,1)->isDouble()) {
            if (nbReplicates != 1 or
                    (integrationType != ALL  and integrationType != LAST) or
                    (inputAggregationType != S_at)){
                throw vu::ArgError(utils::format(
                        "[Manager] since data is not double no "
                        "aggregation is possible for output '%s'",
                        id.c_str()));
            }
            manageDouble = false;
        }
        if (integrationType == ALL) {
            if (inputAggregationType == S_at) {
                delegate.reset(new DelOutIntAggrALL(*this, manageDouble));
            } else {
                delegate.reset(new DelOutIntALL(*this));
            }
        } else {
            if (inputAggregationType == S_at) {
                delegate.reset(new DelOutAggrALL(*this, manageDouble));
            } else {
                delegate.reset(new DelOutStd(*this));
            }
        }
    }
    return delegate->insertReplicate(outMat, currInput);
}

bool
ManOutput::parsePath(const std::string& path)
{
    std::vector<std::string> tokens;
    utils::tokenize(path, tokens, "/", false);
    if (tokens.size() == 2) {
        view.assign(tokens[0]);
        absolutePort.assign(tokens[1]);
        return true;
    } else {
        return false;
    }
}

}} // namespace vle manager



#endif
