#pragma once

#include "../utils/type_name.h"

#include <string>
#include <functional>
#include <memory>
#include <map>
#include <list>

struct loaded_asset
{
    template<typename type>
    explicit loaded_asset(type *obj) :
        obj((void *) obj),
        typeHash(typeid(type).hash_code()),
        typeName(typename_utils::getTypeName<type>())
    {
        loadedSinceTime = getTime();
        destructor = [obj]
        {
            delete obj;
        };
    }

    void moveFrom(loaded_asset *other);

    ~loaded_asset();

    void *obj;
    const std::size_t typeHash;
    const std::string typeName;
    std::string fullPath, shortPath;
    double loadedSinceTime;
  private:
    std::function<void()> destructor;

    static double getTime();
};

class AssetManager
{
    struct AssetLoader
    {
        const size_t type;
        const std::string typeName;

        const std::vector<std::string> fileSuffixes;

        const std::function<loaded_asset *(const std::string &path)> loadFunction;

        bool match(const std::string &filePath) const;

        void removeSuffix(std::string &path) const;
    };

  public:

    typedef std::map<std::string, std::shared_ptr<loaded_asset>> AssetsByPath;

    template<typename type>
    static void addAssetLoader(
        const std::vector<std::string> &assetFileSuffixes,
        std::function<type *(const std::string &path)> loadFunction
    )
    {
        if (assetFileSuffixes.empty())
        {
            return;
        }
        getLoaders().push_back({
            typeid(type).hash_code(),
            typename_utils::getTypeName<type>(),
            assetFileSuffixes,
            [=] (const std::string &path) {
                return new loaded_asset(loadFunction(path));
            }
        });
        // Sort the loaders by the suffix length. This way ".specific.type" can be loaded by a more specialized loader than ".type"
        getLoaders().sort([] (auto l1, auto l2) {
            return l1.fileSuffixes[0].size() > l2.fileSuffixes[0].size();
        });
    }

    static void loadDirectory(const char *directory, bool bPrintLoadedFiles = false);

    static void loadFile(const std::string &path, const std::string &removePreFix, bool bPrintLoadedFile = false);

    template <typename type>
    static const AssetsByPath &getAssetsForType()
    {
        return getAssets()[typeid(type).hash_code()];
    }

    static std::map<size_t, AssetsByPath> &getAssets();

  private:

    static bool findLoader(const std::string &assetPath, int &outLoaderIndex, AssetLoader *&outLoader);

    static std::list<AssetLoader> &getLoaders();

};
