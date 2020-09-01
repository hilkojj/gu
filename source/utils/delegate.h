
#ifndef GAME_DELEGATE_H
#define GAME_DELEGATE_H

#include <functional>
#include <memory>
#include <list>
#include <any>

struct delegate_method
{
    void reset()
    {
        ptr_to_callable = NULL;
    }

  protected:

    template<typename>
    friend struct delegate_base;

    std::any ptr_to_callable = NULL;
};


template<typename func_signature>
struct delegate_base
{
    typedef std::function<func_signature> function;

    typedef std::shared_ptr<function> callable_ptr;
    typedef std::weak_ptr<function> weak_callable_ptr;

    delegate_method operator+=(const function &func)
    {
        callable_ptr c = std::make_shared<function>(func);
        weakCallables.push_back(c); // this is safe to do while iterating over weakCallables, because weakCallables is a std::list.

        delegate_method cb;
        cb.ptr_to_callable = c;
        return cb;
    }

  protected:
    std::list<weak_callable_ptr> weakCallables;

    void callAll(const std::function<void(function)> &functionCaller)
    {
        auto i = this->weakCallables.begin();
        while (i != this->weakCallables.end())
        {
            if (auto f = (*i).lock())
            {
                functionCaller(*f);
                i++;
            }
            else
                this->weakCallables.erase(i++);
        }
    }

};

template<typename func_signature>
struct delegate;


template<typename ReturnType, typename ...Args>
struct delegate<ReturnType(Args...)> : public delegate_base<ReturnType(Args...)>
{

    typedef std::function<void(ReturnType)> ReturnTypeHandler;

    void operator ()(Args... args, const ReturnTypeHandler &returnHandler)
    {
        this->callAll([&](const auto &func) {

            returnHandler(
                func(std::forward<Args>(args)...)
            );
        });
    }

    struct return_values
    {
        std::list<ReturnType> values;

        bool anyEquals(const ReturnType &v)
        {
            for (auto &v1 : values)
                if (v == v1)
                    return true;
            return false;
        }

        bool allEqual(const ReturnType &v)
        {
            for (auto &v1 : values)
                if (v != v1)
                    return false;
            return true;
        }
    };

    return_values operator ()(Args... args)
    {
        return_values vals;

        this->callAll([&](const auto &func) {
            vals.values.push_back(func(std::forward<Args>(args)...));
        });
        return vals;
    }

};


template<typename ...Args>
struct delegate<void(Args...)> : public delegate_base<void(Args...)>
{
    void operator ()(Args... args)
    {
        this->callAll([&](const auto &func) {
            func(std::forward<Args>(args)...);
        });
    }

};


#endif //GAME_DELEGATE_H
