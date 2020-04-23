
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

  public:

    asset(const std::string &path)
    {
        loadedAsset = AssetManager::assets[path];
        if (!loadedAsset)
            throw gu_err(getTypeName<type>() + "-asset '"  +  path + "' is not loaded.");
        if (loadedAsset->objType != typeid(type).hash_code())
            throw gu_err(loadedAsset->objTypeName + "-asset '"  +  path + "' is not a " + getTypeName<type>());
    }

    type* operator->()
    {
        return (type *) loadedAsset->obj;
    }

};


#endif
