
#include "asset.h"
#include "AssetManager.h"

#include "../utils/gu_error.h"
#include "../graphics/external/gl_includes.h"

asset_impl::asset_impl() :
    lastReloadCheckTime(glfwGetTime())
{

}

void *asset_impl::get() const
{
    if (!loadedAsset)
    {
        throw gu_err("Tried to get unset asset.");
    }
    return loadedAsset->obj;
}

void asset_impl::set(size_t typeHash, const char *path)
{
    loadedAsset = AssetManager::getAssets()[typeHash][path];
    if (!loadedAsset)
    {
        throw gu_err(std::string("Asset '") +  path + "' is not loaded.");
    }
    if (loadedAsset->typeHash != typeHash)
    {
        throw gu_err(loadedAsset->typeName + "-asset '"  +  path + "' was assigned to another type.");
    }
}

bool asset_impl::hasReloaded()
{
    if (loadedAsset == nullptr)
    {
        return false;
    }
    if (loadedAsset->loadedSinceTime > lastReloadCheckTime)
    {
        lastReloadCheckTime = glfwGetTime();
        return true;
    }
    return false;
}
