
#ifndef GAME_ASSET_H
#define GAME_ASSET_H

#include <memory>

struct loaded_asset;

class asset_impl
{
  public:
    asset_impl();

    void *get() const;

    void set(size_t typeHash, const char *path);

    bool hasReloaded();

    std::shared_ptr<loaded_asset> loadedAsset;
    double lastReloadCheckTime;
};

template<typename type>
class asset
{
  public:

    asset() = default;

    asset(const char *path)
    {
        set(path);
    }

    type* operator->()
    {
        return (type *) impl.get();
    }

    const type* operator->() const
    {
        return (type *) impl.get();
    }

    type &get()
    {
        return *(this->operator->());
    }

    const type &get() const
    {
        return *(this->operator->());
    }

    bool hasReloaded()
    {
        return impl.hasReloaded();
    }

    void set(const char *path)
    {
        impl.set(typeid(type).hash_code(), path);
    }

    void unset()
    {
        impl.loadedAsset = nullptr;
    }

    bool isSet() const
    {
        return impl.loadedAsset != nullptr;
    }

    const loaded_asset *getLoadedAsset() const
    {
        return impl.loadedAsset.get();
    }

  private:

    asset_impl impl;

};


#endif
