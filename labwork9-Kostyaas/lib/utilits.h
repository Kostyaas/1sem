#pragma once
#include <cstddef>
#include <utility>
#include <type_traits>

namespace my
{
    template <typename T>
    struct remove_reference
    {
        using type = T;
    };

    template <typename T>
    struct remove_reference<T &>
    {
        using type = T;
    };

    template <typename T>
    struct remove_reference<T &&>
    {
        using type = T;
    };
    template <typename T>
    using remove_reference_t = typename remove_reference<T>::type;

    template <typename T>
    struct is_class
    {
    private:
        template <typename U>
        static char test(int U::*);
        template <typename U>
        static int test(...);

    public:
        static constexpr bool value = sizeof(test<T>(nullptr)) == sizeof(char);
    };

    template <typename T>
    struct is_class_decay
    {
        using PureT = typename remove_reference<T>::type;
        static constexpr bool value = is_class<PureT>::value;
    };
    template <typename T, typename = void>
    struct has_value_type : std::false_type {};

    template <typename T>
    struct has_value_type<T, std::void_t<typename T::value_type>> : std::true_type {};

    template <class T, typename = std::enable_if_t<!is_class_decay<T>::value>>
    T &&my_declval_for_function() noexcept;
    template <class T, typename = std::enable_if_t<is_class_decay<T>::value && has_value_type<T>::value>>
    typename T::value_type&& my_declval_for_function() noexcept;
    template<class T>
    T&& declval();
    template <std::size_t... Ints>
    struct index_sequence
    {
        static constexpr std::size_t size = sizeof...(Ints);
    };

    template <std::size_t N, std::size_t... Is>
    struct make_index_sequence_impl : make_index_sequence_impl<N - 1, N - 1, Is...> {};

    template <std::size_t... Is>
    struct make_index_sequence_impl<0, Is...>
    {
        using type = index_sequence<Is...>;
    };
    template <std::size_t N>
    using make_index_sequence = typename make_index_sequence_impl<N>::type;

    template <typename T>
    struct tuple_size;

    template <typename T>
    struct is_member_function_pointer
    {
        static constexpr const bool value = false;
    };

    template <typename C, typename Ret>
    struct is_member_function_pointer<Ret (C::*)>
    {
        static constexpr const bool value = true;
    };

 
    
    template <typename T>
    inline constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;

    template <typename Lambda, typename... Args>
    using return_type_function = decltype(declval<Lambda>()(my_declval_for_function<Args>()...));

    template <typename T>
    auto unwrap_arg(T &&val)
    {
        if constexpr (requires { val.call(); })
        {
            return val.call();
        }
        else
        {
            return std::forward<T>(val);
        }
    }

    template <bool B, class T = void>
    struct enable_if
    {
    };

    template <class T>
    struct enable_if<true, T>
    {
        using value = T;
    };

    template <bool Tp, class T = void>
    using enable_if_t = typename enable_if<Tp, T>::value;
};
