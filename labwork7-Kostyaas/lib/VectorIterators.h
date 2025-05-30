#pragma once
#include <type_traits>
#include <IteratorHelp.h>

namespace list
{
    template <class T, bool IsConst>
    class BaseIteratorVector
    {
    protected:
        using pointer_type = typename std::conditional<IsConst, const T *, T *>::type;
        pointer_type ptr;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = typename std::conditional<IsConst, const T, T>::type;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type *;
        using reference = value_type &;

        BaseIteratorVector(pointer_type p = nullptr) : ptr(p) {}

        reference operator*() const { return *ptr; }
        pointer operator->() const { return ptr; }

        BaseIteratorVector &operator++()
        {
            ++ptr;
            return *this;
        }

        BaseIteratorVector operator++(int)
        {
            BaseIteratorVector temp = *this;
            ++ptr;
            return temp;
        }

        BaseIteratorVector &operator--()
        {
            --ptr;
            return *this;
        }

        BaseIteratorVector operator--(int)
        {
            BaseIteratorVector temp = *this;
            --ptr;
            return temp;
        }

        BaseIteratorVector operator+(difference_type n) const
        {
            return BaseIteratorVector(ptr + n);
        }

        BaseIteratorVector operator-(difference_type n) const
        {
            return BaseIteratorVector(ptr - n);
        }

        difference_type operator-(const BaseIteratorVector &other) const
        {
            return  ptr - other.ptr;
        }
        BaseIteratorVector &operator+=(difference_type n)
        {
            ptr = ptr + n;
            return *this;
        }

        BaseIteratorVector &operator-=(difference_type n)
        {
            ptr = ptr - n;
            return *this;
        }
        bool operator==(const BaseIteratorVector &other) const { return ptr == other.ptr; }
        bool operator!=(const BaseIteratorVector &other) const { return ptr != other.ptr; }
        bool operator<(const BaseIteratorVector &other) const { return ptr < other.ptr; }
        bool operator>(const BaseIteratorVector &other) const { return ptr > other.ptr; }
        bool operator<=(const BaseIteratorVector &other) const { return ptr <= other.ptr; }
        bool operator>=(const BaseIteratorVector &other) const { return ptr >= other.ptr; }
    };

    template <class T>
    using Iterator = BaseIteratorVector<T, false>;

    template <class T>
    using ConstIterator = BaseIteratorVector<T, true>;

    template <class T>
    using ReverseIterator = std::reverse_iterator<T>;

    template <class T>
    using ReverseConstIterator = std::reverse_iterator<T>;
}
