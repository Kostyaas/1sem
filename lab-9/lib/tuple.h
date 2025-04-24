#pragma once
#include <cstddef>
#include <utility>
#include <type_traits>
#include <utilits.h>
namespace my
{
    template <typename... Types>
    struct my_tuple;

    template <>
    struct my_tuple<>
    {
    };

    template <typename Head, typename... Tail>
    struct my_tuple<Head, Tail...>
    {
        Head head;
        my_tuple<Tail...> tail;

        template <typename H, typename... T,
                  typename = std::enable_if_t<sizeof...(T) == sizeof...(Tail)>>
        my_tuple(H &&h, T &&...t)
            : head(std::forward<H>(h)), tail(std::forward<T>(t)...) {}
    };

    template <typename... Types>
    struct tuple_size<my_tuple<Types...>>
    {
        static constexpr size_t value = sizeof...(Types);
    };

    template <size_t N, typename Head, typename... Tail>
    auto &get(my_tuple<Head, Tail...> &t)
    {
        static_assert(N < sizeof...(Tail) + 1, "Tuple index out of bounds");
        if constexpr (N == 0)
            return t.head;
        else
            return get<N - 1>(t.tail);
    }

    template <size_t N, typename Head, typename... Tail>
    const auto &get(const my_tuple<Head, Tail...> &t)
    {
        static_assert(N < sizeof...(Tail) + 1, "Tuple index out of bounds");
        if constexpr (N == 0)
            return t.head;
        else
            return get<N - 1>(t.tail);
    }

    template <typename F, typename Tuple, size_t... I>
    constexpr auto apply_impl(F &&f, Tuple &&t, my::index_sequence<I...>)
    {
        return std::forward<F>(f)(unwrap_arg(get<I>(std::forward<Tuple>(t)))...);
    }

    template <typename F, typename Tuple>
    constexpr auto my_apply(F &&f, Tuple &&t)
    {
        using bare_tuple = my::remove_reference_t<Tuple>;
        constexpr size_t size = my::tuple_size<bare_tuple>::value;
        return apply_impl(
            std::forward<F>(f),
            std::forward<Tuple>(t),
            my::make_index_sequence<size>{});
    }
};