
#ifndef GAME_ASSETMANAGER_H
#define GAME_ASSETMANAGER_H

#include <functional>
#include <cstring>
#include <map>
#include <memory>
#include <list>
#include "../utils/string.h"
#include "../files/file.h"
#include "../gu/game_utils.h"

struct loaded_asset
{
    void *obj;
    std::size_t objType;
    std::string objTypeName;

    double loadedSinceTime;

    std::function<void()> destructor;

    template<typename type>
    loaded_asset(type *obj) : obj((void *) obj)
    {
        objType = typeid(type).hash_code();
        objTypeName = getTypeName<type>();
        loadedSinceTime = glfwGetTime();
        destructor = [=] {
            delete obj;
        };
    }

    void replace(loaded_asset *other)
    {
        assert(objType == other->objType);

        obj = other->obj;
        destructor();
        destructor = other->destructor;
        other->destructor = [] {};
        loadedSinceTime = other->loadedSinceTime;
    }

    ~loaded_asset()
    {
        std::cout << "k byeeeeeeeeee\n";
        destructor();
    }
};

class AssetManager
{

    template<typename> friend class asset;

    inline static std::map<std::string, std::shared_ptr<loaded_asset>> assets;

    struct AssetLoader
    {
        size_t type;
        const std::string
            typeName,
            fileSuffix;
        std::function<loaded_asset *(const std::string &path)> loadFunction;

        bool match(const std::string &filePath)
        {
            return stringEndsWith(filePath, fileSuffix);
        }
    };

    inline static std::list<AssetLoader> loaders;

  public:

    template<typename type>
    static void addAssetLoader(
            const std::string &assetFileSuffix,
            std::function<type *(const std::string &path)> function
    )
    {
        loaders.push_back({
            typeid(type).hash_code(),
            getTypeName<type>(),
            assetFileSuffix,
            [=] (const std::string &path) {
                return new loaded_asset(function(path));
            }
        });
    }

    static void load(const char *directory)
    {
        loaders.sort([] (auto l1, auto l2) {
            return l1.fileSuffix.size() > l2.fileSuffix.size();
        });

        File::iterateFilesRecursively(directory, [&](auto path) {
            for (AssetLoader &loader : loaders)
            {
                if (!loader.match(path))
                    continue;

                std::cout << "Loading " << loader.typeName << "-asset '" << path << "'...\n";

                loaded_asset *loaded = loader.loadFunction(path);
                std::string key = splitString(path, std::string(directory) + "/")[0];
                key.resize(key.size() - loader.fileSuffix.size());

                auto existing = assets[key];

                if (existing)
                    existing->replace(loaded);
                else
                    assets[key] = std::shared_ptr<loaded_asset>(loaded);
            }
        });

    }

    static void unloadAll()
    {
        assets.clear();
    }

};


#endif
