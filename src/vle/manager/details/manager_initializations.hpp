
#ifndef VLE_MANAGER_DETAILS_MANAGER_INITIALIZATIONS_HPP_
#define VLE_MANAGER_DETAILS_MANAGER_INITIALIZATIONS_HPP_

#include <vle/vpz/Vpz.hpp>
#include <vle/vpz/AtomicModel.hpp>

#include "manager/details/vle_api_utils.hpp"

namespace vle {
namespace manager {

/******************************/
std::unique_ptr<vpz::Vpz>
build_embedded_model(
        const wrapper_init& init,
        vle::utils::ContextPtr ctx,
        manager::Error& err)
{
    std::string idVpz = "";
    std::string idPackage = "";
    std::string vpzFile = "";
    bool status = true;
    if (init.exist("vpz", status)) {
        idVpz = init.getString("vpz", status);
    } else {
        err.code = -1;
        err.message = "[Manager] missing 'vpz'";
        return nullptr;
    }
    if (init.exist("package", status)) {
        idPackage = init.getString("package", status);
        utils::Package pkg(ctx, idPackage);
        vpzFile = pkg.getExpFile(idVpz, vle::utils::PKG_BINARY);
    } else {
        vpzFile = idVpz;
    }
    std::unique_ptr<vpz::Vpz> model;
    try {
        model.reset(new vpz::Vpz(vpzFile));
        model->project().experiment().setCombination("linear");
    } catch (const std::exception& e){
        err.code = -1;
        err.message = "[Manager] ";
        err.message += e.what();
        return nullptr;
    }
    return model;
}


/******************************/
void
post_define(vpz::Vpz& model,
        const std::vector<std::unique_ptr<ManDefine>>& defines,
        manager::Error& err)
{
    try {
        vpz::Conditions& conds = model.project().experiment().conditions();
        for (unsigned int i=0; i < defines.size(); i++) {
            ManDefine& tmp_def = *defines[i];
            vpz::Condition& cond = conds.get(tmp_def.cond);
            if (cond.exist(tmp_def.port)) {
                if (not tmp_def.to_add) {
                    cond.del(tmp_def.port);
                }
            } else {
                if (tmp_def.to_add) {
                    cond.add(tmp_def.port);
                }
            }
        }
    } catch (const std::exception& e){
        err.code = -1;
        err.message = "[Manager] error in 'define' settings: ";
        err.message += e.what();
    }

}

/******************************/
void
post_propagates(vpz::Vpz& model,
        const std::vector<std::unique_ptr<ManPropagate>>& propagates,
        const wrapper_init& init,
        manager::Error& err)
{
    try {
        vpz::Conditions& conds = model.project().experiment().conditions();
        //post propagate
        for (unsigned int i=0; i < propagates.size(); i++) {
            ManPropagate& tmp_propagate = *propagates[i];
            vpz::Condition& cond = conds.get(tmp_propagate.cond);
            const value::Value& exp = tmp_propagate.value(init);
            cond.setValueToPort(tmp_propagate.port, exp.clone());
        }
    } catch (const std::exception& e){
        err.code = -1;
        err.message = "[Manager] error in 'propagate' settings: ";
        err.message += e.what();
    }
}

/******************************/
void
config_views(vpz::Vpz& model,
             const std::vector<std::unique_ptr<ManOutput>>& outputs,
             manager::Error& err)
{
    {//remove useless views, observables and observables ports.
        vle::vpz::BaseModel* baseModel = model.project().model().node();
        std::set<std::string> viewsToKeep;
        std::set<std::string> obsAndPortTokeep;//of the form obs,port
        std::string atomPath;
        std::string obsPort;

        for (auto& o : outputs) {
            viewsToKeep.insert(o->view);
            if (not o->extractAtomPathAndPort(atomPath, obsPort)) {
                err.code = -1;
                err.message = utils::format(
                        "[Manager]: error in analysis of output '%s/%s'",
                        o->view.c_str(), o->absolutePort.c_str());
                return ;
            }
            vle::vpz::AtomicModel* atomMod = baseModel->findModelFromPath(
                    atomPath)->toAtomic();
	    if (not atomMod) {
		 err.code = -1;
		 err.message = utils::format(
		        "[Manager]: atomic model not found : '%s'",
                       atomPath.c_str());
                return ;
	    }
            obsPort = atomMod->observables() + "," + obsPort;
            obsAndPortTokeep.insert(obsPort);
        }
        VleAPIutils::keepOnly(model, viewsToKeep, obsAndPortTokeep);
    }
    std::vector<std::unique_ptr<ManOutput>>::const_iterator itb =
            outputs.begin();
    std::vector<std::unique_ptr<ManOutput>>::const_iterator ite =
            outputs.end();
    for (; itb != ite; itb++) {
        VleAPIutils::changePlugin(model, (*itb)->view, "storage");
    }
}


/******************************/
void
init_embedded_model(
        vpz::Vpz& model, const ManagerObjects& manObj,
        const wrapper_init& init, manager::Error& err)
{
    config_views(model, manObj.mOutputs, err);
    if (err.code) return ;
    post_define(model, manObj.mDefine, err);
    if (err.code) return ;
    post_propagates(model, manObj.mPropagate, init, err);
}

/******************************/
std::unique_ptr<value::Map>
init_results(const std::vector<std::unique_ptr<ManOutput>>& outputs,
        manager::Error& err)
{
    try {
        //build output matrix with header
        std::unique_ptr<value::Map> results(new value::Map());
        for (unsigned int j=0; j<outputs.size();j++) {
            results->add(outputs[j]->id, value::Null::create());
        }
        return results;
    } catch (const std::exception& e){
        err.code = -1;
        err.message = "[Manager] error in results initialization: ";
        err.message += e.what();
        return nullptr;
    }
}

}} // namespace vle manager

#endif
