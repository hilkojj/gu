#pragma once

#include <utility>
#include <functional>

/**
 * CONSTRUCT ON FIRST USE.
 *
 * example:
 *
 * cofu<MyObject> myObject("apple", 4);
 * // at this point there's no MyObject constructed yet.
 *
 * myObject->myFunction(); // Here the MyObject is actually constructed, because 'myObject' is used for the first time here.
 *
 */
template<typename type>
class cofu
{
    type *obj = nullptr;
    std::function<void()> constructor;

  public:

    template <typename ...Args>
    explicit cofu(Args&&... args) : constructor([&] {

        obj = new type(std::forward<Args>(args)...);
    })
    {}

    type* operator->()
    {
        return getPtrToObj();
    }

    const type* operator->() const
    {
        return getPtrToObj();
    }

    type &get()
    {
        return *(this->operator->());
    }

    const type &get() const
    {
        return *(this->operator->());
    }

    ~cofu()
    {
        delete obj;
    }

  private:
    type *getPtrToObj() const
    {
        if (!obj)
            constructor();
        return obj;
    }

};
