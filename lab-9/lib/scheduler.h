#pragma once
#include "tuple.h"
#include <iostream>
#include <type_traits>
#include <vector>
// TODO: Implement scheduler

class TTaskScheduler
{
public:
    struct ITask
    {
        virtual void activate() {}
        virtual ~ITask() = default;
    };
    template <class FUNC, class... Args>
    struct ID : ITask
    {
        using value_type = my::return_type_function<FUNC, Args...>;
        FUNC func;
        my::my_tuple<Args...> params;
        ID(FUNC func, Args... params)
            : func(std::move(func)), params(std::forward<Args>(params)...) {}
        void activate() override
        {
            if (!is_count)
            {
                value = my_apply(func, params);
                is_count = true;
            }
        }
        value_type call()
        {
            if (is_count)
            {
                return value;
            }
            else
            {
                value = my_apply(func, params);
                is_count = true;
                return value;
            }
        }
        bool is_count = false;
        value_type value;
    };
    template <class T, class FUNC, class... Args>
    struct FutureResultId
    {
        using value_type = T;
        std::shared_ptr<ID<FUNC, Args...>> ptr_id;
        value_type call()
        {
            return ptr_id->call();
        }
        FutureResultId(ID<FUNC, Args...> &id)
            : ptr_id(std::make_shared<ID<FUNC, Args...>>(id)) {}
    };

    template <class T, class FUNC, class... Args>
    FutureResultId<T, FUNC, Args...> getFutureResult(ID<FUNC, Args...> &id)
    {
        return FutureResultId<T, FUNC, Args...>(id);
    }
    template <class T, class FUNC, class... Args>
    T getResult(ID<FUNC, Args...> &id)
    {
        return static_cast<T>(id.call());
    }
    template <typename FUNC, typename... Args, typename = my::enable_if_t<!my::is_member_function_pointer_v<FUNC>>>
    auto add(FUNC &&func, Args &&...params)
    {
        using IDType = ID<FUNC, std::decay_t<Args>...>;
        auto task = std::make_shared<IDType>(std::forward<FUNC>(func), std::forward<Args>(params)...);

        base_.push_back(task);

        return *task;
    }
    template <typename FUNC, typename Obj, typename... Args, typename = my::enable_if_t<my::is_member_function_pointer_v<FUNC>>>
    auto add(FUNC &&f, Obj &&obj, Args &&...args)
    {
        auto lambda = [f = std::forward<FUNC>(f),
                       obj = std::forward<Obj>(obj)](auto &&...ts)
        {
            return (obj.*f)(std::forward<decltype(ts)>(ts)...);
        };

        using LambdaType = decltype(lambda);
        using IDType = ID<LambdaType, std::decay_t<Args>...>;

        auto ptr = std::make_shared<IDType>(std::move(lambda), std::forward<Args>(args)...);
        base_.push_back(ptr);
        return *ptr;
    }
    void executeAll()
    {
        for (auto &&tasks : base_)
        {
            tasks->activate();
        }
    }

private:
    std::vector<std::shared_ptr<ITask>> base_;
};