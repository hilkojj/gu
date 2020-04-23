
#ifndef GAME_ASSETMANAGER_H
#define GAME_ASSETMANAGER_H

#include <functional>
#include <cstring>
#include <map>
#include <memory>
#include <list>
#include <assert.h>
#include "../utils/string.h"
#include "../files/file.h"
#include "../gu/game_utils.h"

struct loaded_asset
{
    void *obj;
    std::size_t objType;
    std::string objTypeName, fullPath, shortPath;

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
        destructor();
    }
};

class AssetManager
{

    template<typename> friend class asset;

    inline static std::map<size_t, std::map<std::string, std::shared_ptr<loaded_asset>>> assets;

    struct AssetLoader
    {
        size_t type;
        const std::string typeName;

        std::vector<std::string> fileSuffixes;

        std::function<loaded_asset *(const std::string &path)> loadFunction;

        bool match(const std::string &filePath) const
        {
            for (auto &suff : fileSuffixes)
                if (stringEndsWith(filePath, suff))
                    return true;
            return false;
        }

        void removeSuffix(std::string &path) const
        {
            for (auto &suff : fileSuffixes)
            {
                if (stringEndsWith(path, suff))
                {
                    path.resize(path.size() - suff.size());
                    break;
                }
            }
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
            splitString(assetFileSuffix, "|"),
            [=] (const std::string &path) {
                return new loaded_asset(function(path));
            }
        });
        loaders.sort([] (auto l1, auto l2) {
            return l1.fileSuffixes[0].size() > l2.fileSuffixes[0].size();
        });
    }

    static void load(const char *directory)
    {
        File::iterateDirectoryRecursively(directory, [&](auto path, bool isDir) {
            if (!isDir)
                loadFile(path, std::string(directory) + "/");
        });
    }

    static void loadFile(const std::string &path, const std::string &removePreFix)
    {
        for (const AssetLoader &loader : loaders)
        {
            if (!loader.match(path))
                continue;

            std::cout << "Loading " << loader.typeName << "-asset '" << path << "'...\n";

            try
            {
                loaded_asset *loaded = loader.loadFunction(path);
                std::string key = splitString(path, removePreFix)[0];
                auto shared = std::shared_ptr<loaded_asset>(loaded);

                auto existing = assets[loaded->objType][key];

                if (existing)
                    existing->replace(loaded);
                else
                    assets[loaded->objType][key] = shared;

                loader.removeSuffix(key);
                assets[loaded->objType][key] = shared;

                loaded->fullPath = path;
                loaded->shortPath = key;
            }
            catch(const std::exception& e)
            {
                std::cerr << "Error while loading asset: " << path << ":\n" << e.what() << std::endl;
            }
            break;
        }
    }

    static void unloadAll()
    {
        assets.clear();
    }

};


#endif
