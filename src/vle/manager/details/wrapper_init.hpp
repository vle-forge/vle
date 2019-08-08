
#ifndef VLE_MANAGER_DETAILS_WRAPPER_INIT_HPP_
#define VLE_MANAGER_DETAILS_WRAPPER_INIT_HPP_

#include <vle/devs/InitEventList.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Integer.hpp>

namespace vle {
namespace manager {

/**
 * @brief wrapper to an devs::InitEventList and value::Map
 */
class wrapper_init
{
private:
    const devs::InitEventList* mInitEventList;
    const value::Map* mMap;

    devs::InitEventList::const_iterator itbI;
    value::Map::const_iterator itbM;

    devs::InitEventList::const_iterator iteI;
    value::Map::const_iterator iteM;

    value::Null def;
public:

    wrapper_init(const devs::InitEventList* init_evt_list):
        mInitEventList(init_evt_list), mMap(0), itbI(),
        itbM(), iteI(), iteM(), def()
    {
        itbI =  mInitEventList->begin();
        iteI =  mInitEventList->end();
    }

    wrapper_init(const value::Map* init_map):
        mInitEventList(0), mMap(init_map), itbI(),
        itbM(), iteI(), iteM(), def()
    {
        itbM =  mMap->begin();
        iteM =  mMap->end();
    }


    ~wrapper_init()
    {
        mInitEventList = 0;
        mMap = 0;
    }


    void begin()
    {
        if (mInitEventList) {
            itbI =  mInitEventList->begin();
        } else {
            itbM =  mMap->begin();
        }
    }


    bool isEnded() const
    {
        if (mInitEventList and itbI != iteI) {
            return false;
        }
        if (mMap and itbM != iteM) {
            return false;
        }
        return true;
    }

    bool next()
    {
        if (mInitEventList and itbI != iteI) {
            itbI++ ;
            return true;
        }
        if (mMap and itbM != iteM) {
            itbM++;
            return true;
        }
        return false;
    }

    /**
     * @brief get current value
     *
     * @param [out] key, the key of the current value
     * @param [out] status, the status of the command
     */
    const value::Value& current(std::string& key, bool& status)
    {
        if (mInitEventList and itbI != iteI) {
            const value::Value& res_val = *itbI->second.get();
            status = true;
            key.assign(itbI->first);
            return res_val;
        }
        if (mMap and itbM != iteM) {
            const value::Value& res_val = *itbM->second.get();
            status = true;
            key.assign(itbM->first);
            return res_val;
        }
        key.assign("");
        status = false;
        return def;
    }

    /**
     * @brief tells if a key exists
     *
     * @param [in] key, the key to look for
     * @param [out] status, the status of the call
     */
    bool exist(const std::string& key, bool& status) const
    {
        if (mInitEventList) {
            status = true;
            return mInitEventList->exist(key);
        }
        if (mMap) {
            status = true;
            return mMap->exist(key);
        }
        status = false;
        return false;
    }

    /**
     * @brief get a specific value
     *
     * @param [in] key, the key to look into
     * @param [out] status, the status of the call
     */
    const value::Value& get(const std::string& key, bool& status) const
    {
        if (mInitEventList) {
            const std::shared_ptr<const value::Value>& res_val =
                    mInitEventList->get(key);
            if (res_val) {
                status = true;
                return *res_val.get();
            }
        }
        if (mMap) {
            const std::unique_ptr<value::Value>& res_val =
                    mMap->get(key);
            if (res_val) {
                status = true;
                return *res_val.get();
            }
        }
        status = false;
        return def;
    }


    std::string getString(const std::string& key, bool& status) const
    {
        const value::Value& v = get(key, status);
        if (status and v.isString()) {
            return v.toString().value();
        }
        return "";
    }

    int getInt(const std::string& key, bool& status) const
    {
        const value::Value& v = get(key, status);
        if (status and v.isInteger()) {
            return v.toInteger().value();
        }
        return 0;
    }

    int getBoolean(const std::string& key, bool& status) const
    {
        const value::Value& v = get(key, status);
        if (status and v.isBoolean()) {
            return v.toBoolean().value();
        }
        return 0;
    }



};

}
}//namespaces vle manager

#endif
