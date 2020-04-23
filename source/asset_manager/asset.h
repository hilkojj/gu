
#ifndef GAME_ASSET_H
#define GAME_ASSET_H

#include <memory>
#include "../utils/gu_error.h"
#include "../utils/type_name.h"

#include "AssetManager.h"

template<typename type>
class asset
{

    std::shared_ptr<loaded_asset> loadedAsset;
    double lastCheckTime = glfwGetTime();

  public:

    asset() = default;

    asset(const std::string &path)
    {
        set(path);
    }

    type* operator->()
    {
        if (!loadedAsset)
            throw gu_err("trying to use an asset that is not initialized!");
        return (type *) loadedAsset->obj;
    }

    type &get()
    {
        return *(this->operator->());
    }

    bool hasReloaded()
    {
        if (loadedAsset->loadedSinceTime > lastCheckTime)
        {
            lastCheckTime = glfwGetTime();
            return true;
        }
        return false;
    }

    void set(const std::string &path)
    {
        loadedAsset = AssetManager::assets[typeid(type).hash_code()][path];
        if (!loadedAsset)
            throw gu_err(getTypeName<type>() + "-asset '"  +  path + "' is not loaded.");
        if (loadedAsset->objType != typeid(type).hash_code())
            throw gu_err(loadedAsset->objTypeName + "-asset '"  +  path + "' is not a " + getTypeName<type>());
    }
};


#endif
