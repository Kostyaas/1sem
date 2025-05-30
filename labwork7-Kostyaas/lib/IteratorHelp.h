#pragma once
#include <type_traits>

template <class Iterator>
bool is_it_null(const Iterator &iter)
{
    if constexpr (std::is_pointer_v<Iterator>) 
    {
        return iter == nullptr;
    }
    else 
    {
        return iter.operator->() == nullptr;
    }
}