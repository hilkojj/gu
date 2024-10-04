
#include "AssetManager.h"

#include "../files/file_utils.h"
#include "../utils/string_utils.h"
#include "../gl_includes.h"

#include <cassert>
#include <iostream>


void loaded_asset::moveFrom(loaded_asset *other)
{
    assert(typeHash == other->typeHash);

    obj = other->obj;
    destructor();
    destructor = other->destructor;
    other->destructor = [] {};
    loadedSinceTime = other->loadedSinceTime;
}

loaded_asset::~loaded_asset()
{
    destructor();
}

double loaded_asset::getTime()
{
    return glfwGetTime();
}

bool AssetManager::AssetLoader::match(const std::string &filePath) const
{
    for (const std::string &suffix : fileSuffixes)
    {
        if (su::endsWith(filePath, suffix))
        {
            return true;
        }
    }
    return false;
}

void AssetManager::AssetLoader::removeSuffix(std::string &path) const
{
    for (const std::string &suffix : fileSuffixes)
    {
        if (su::endsWith(path, suffix))
        {
            path.resize(path.size() - suffix.size());
            break;
        }
    }
}

void AssetManager::loadDirectory(const char *directory, bool bPrintLoadedFiles)
{
    std::vector<std::string> filePaths;
    fu::iterateDirectoryRecursively(directory, [&](auto path, bool bIsDir)
    {
        if (!bIsDir)
        {
            filePaths.push_back(path);
        }
    });
    // Sort assets to load by the loader that will load them.
    std::sort(filePaths.begin(), filePaths.end(), [&] (const std::string &a, const std::string &b)
    {
        AssetLoader *assetLoader = nullptr;
        int aIndex = -1, bIndex = -1;
        findLoader(a, aIndex, assetLoader);
        findLoader(b, bIndex, assetLoader);
        return aIndex < bIndex;
    });
    for (const std::string &path : filePaths)
    {
        loadFile(path, std::string(directory) + "/", bPrintLoadedFiles);
    }
}

void AssetManager::loadFile(const std::string &path, const std::string &removePreFix, bool bPrintLoadedFile)
{
    int loaderIndex = 0;
    AssetLoader *loader = nullptr;
    if (findLoader(path, loaderIndex, loader))
    {
        if (bPrintLoadedFile)
        {
            std::cout << "Loading " << loader->typeName << "-asset '" << path << "'..." << std::endl;
        }

        try
        {
            std::shared_ptr<loaded_asset> loaded(loader->loadFunction(path));
            std::string key = su::split(path, removePreFix)[0];

            auto existing = getAssets()[loaded->typeHash][key];

            if (existing)
                existing->moveFrom(loaded.get());
            else
                getAssets()[loaded->typeHash][key] = loaded;

            loader->removeSuffix(key);
            if (!existing)
            {
                getAssets()[loaded->typeHash][key] = loaded;
                getAssets()[loaded->typeHash][path] = loaded;
            }

            loaded->fullPath = path;
            loaded->shortPath = key;
        }
        catch (const std::exception &exception)
        {
            std::cerr << "Error while loading asset: " << path << ":\n" << exception.what() << std::endl;
        }
    }
}

std::map<size_t, AssetManager::AssetsByPath> &AssetManager::getAssets()
{
    static std::map<size_t, AssetManager::AssetsByPath> assets;
    return assets;
}

bool AssetManager::findLoader(const std::string &assetPath, int &outLoaderIndex, AssetLoader *&outLoader)
{
    outLoaderIndex = 0;
    for (AssetLoader &loader : getLoaders())
    {
        if (loader.match(assetPath))
        {
            outLoader = &loader;
            return true;
        }
        outLoaderIndex++;
    }
    return false;
}

std::list<AssetManager::AssetLoader> &AssetManager::getLoaders()
{
    static std::list<AssetManager::AssetLoader> loaders;
    return loaders;
}
