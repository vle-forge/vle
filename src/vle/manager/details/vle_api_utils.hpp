
/*
 * Copyright (C) 2009-2014 INRA
 *
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

#ifndef VLE_MANAGER_VLE_API_UTILS_HPP_
#define VLE_MANAGER_VLE_API_UTILS_HPP_

#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/manager/Manager.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>

#include <vle/oov/Plugin.hpp>
#include <vle/devs/Dynamics.hpp>

namespace vle {
namespace manager {

namespace vu = vle::utils;
namespace vm = vle::manager;
namespace vz = vle::vpz;
namespace vv = vle::value;
namespace vo = vle::oov;
namespace vd = vle::devs;

/**
 * @brief Class that provides static methods for
 * handling vle API.
 */
class VleAPIutils
{
public:

     /**
      * @brief remove observable and remove observation of atomic models based
      * on it
      * @param[out] vpz, vpz to update
      * @param obsName, name of observable
      */
     static bool removeObservable(vz::Vpz& vpz, const std::string& obsName)
     {
         vz::Views& viewsIn  = vpz.project().experiment().views();

         viewsIn.observables().del(obsName);
         std::vector<vz::AtomicModel*> aList;
         vz::BaseModel::getAtomicModelList(vpz.project().model().node(), aList);
         for (auto& a : aList){
             if (a->observables() == obsName) {
                 a->setObservables("");
             }
         }
         return true;
     }

     /**
      * @brief remove observable and remove observation of atomic models based
      * on it
      * @param[out] vpz, vpz to update
      * @param obsName, name of observable
      */
     static bool removeView(vz::Vpz& vpz, const std::string& viewName)
     {
         vz::Views& viewsIn  = vpz.project().experiment().views();
         //remove output if not used by another view
         std::string output_name = viewsIn.get(viewName).output();
         bool removeOutput = true;
         for (auto& v : viewsIn.viewlist()) {
             if (v.first != viewName and
                     viewsIn.get(v.first).output() == output_name) {
                 removeOutput = false;
             }
         }
         if (removeOutput) {
             viewsIn.outputs().del(viewsIn.get(viewName).output());
         }
         //remove view
         viewsIn.del(viewName);
         //detach view to all observable port
         for (auto& o : viewsIn.observables().observablelist()) {
             for (auto& p : o.second.observableportlist()) {
                 p.second.del(viewName);
             }
         }
         return true;
     }

     /**
      * @brief a function to remove all view, observable and observable ports
      * not required into a vpz
      * @param[out] vpz, vpz to update
      * @param views, views to keep
      * @param obsPort, set str of the form observable,port identifying
      * the observable ports to keep.
      */
     static bool keepOnly(vz::Vpz& vpz, const std::set<std::string>& views,
             const std::set<std::string>& obsPorts)
     {
         vz::Views& viewsIn  = vpz.project().experiment().views();
         std::set<std::string> viewsToRemove;
         for (auto& v : viewsIn.viewlist()) {
             if (views.find(v.first) == views.end()) {
                 viewsToRemove.insert(v.first);
             }
         }
         for (auto& v : viewsToRemove) {
             removeView(vpz, v);
         }
         std::set<std::string> obsPortsToKeep;
         std::vector<std::string> tokenizeRes;
         for (auto& o : viewsIn.observables().getKeys()) {
             obsPortsToKeep.clear();
             for (auto& obsPortTokeep : obsPorts) {
                 tokenizeRes.clear();
                 vu::tokenize(obsPortTokeep, tokenizeRes, ",", true);
                 if (tokenizeRes[0] == o) {
                     obsPortsToKeep.insert(tokenizeRes[1]);
                 }
             }
             if (obsPortsToKeep.size() == 0){//remove observable ports
                 removeObservable(vpz, o);
             } else {
                 vz::Observable& obs = viewsIn.observables().get(o);
                 std::set<std::string> obsPortsToRm;
                 for (auto& p : obs.observableportlist()) {
                     if (obsPortsToKeep.find(p.first) == obsPortsToKeep.end()) {
                         obsPortsToRm.insert(p.first);
                     }
                 }
                 for (auto& p : obsPortsToRm) {
                     obs.del(p);
                 }
             }
         }
         return true;
     }

    /**
     * @brief Change the output plugin for a view
     * @param vpz, the model to modify
     * @param view, the name of the view
     * @param plugin, the name of the new plugin
     */
    static void changePlugin(vz::Vpz& vpz, const std::string& view,
        const std::string& plugin)
    {
        vz::Views& vle_views = vpz.project().experiment().views();
        vz::Output& out = vle_views.outputs().get(view);
        out.setStream("",plugin,"vle.output");

        //configure output plugins for column names
        if(plugin == "storage"){
            std::unique_ptr<vv::Value> configOutput = vv::Map::create();
            configOutput->toMap().addString("header","top");
            out.setData(std::move(configOutput));

        }

    }

};

}
}//namespaces

#endif
