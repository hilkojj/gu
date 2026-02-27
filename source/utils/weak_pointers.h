#pragma once

#include <memory>
#include <vector>

template<typename type>
class weak_pointers
{
  public:
    std::vector<std::shared_ptr<type>> getSharedPointers() const
    {
        std::vector<std::shared_ptr<type>> strong;
        for (const std::weak_ptr<type> &weak : pointers)
        {
            if (!weak.expired())
            {
                strong.push_back(weak.lock());
            }
        }
        return strong;
    }

    std::vector<type *> getExistingPointers() const
    {
        std::vector<type *> strong;
        for (const std::weak_ptr<type> &weak : pointers)
        {
            if (!weak.expired())
            {
                strong.push_back(&*weak.lock());
            }
        }
        return strong;
    }

    void add(std::shared_ptr<type> pointer)
    {
        pointers.emplace_back(pointer);
    }

    void prune()
    {
        auto it = pointers.begin();
        while (it != pointers.end())
        {
            if (it->expired())
            {
                it = pointers.erase(it);
                continue;
            }
            ++it;
        }
    }

  private:
    std::vector<std::weak_ptr<type>> pointers;
};

