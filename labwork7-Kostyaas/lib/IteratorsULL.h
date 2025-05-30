#pragma once
#include <type_traits>
#include <unrolled_list.h>
#include <VectorIterators.h>
#include <IteratorHelp.h>
namespace linkedList
{

    template <class T, bool IsConst>
    class BasicIterator
    {
    protected:
        using node_pointer = typename std::conditional<IsConst, const T *, T *>::type;
        node_pointer ptr;

    public:
        using value_type = typename std::conditional<IsConst, const T, T>::type;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type *;
        using reference = value_type &;

        BasicIterator(node_pointer p = nullptr) : ptr(p) {}
        BasicIterator(const BasicIterator &it) : ptr(it.ptr) {}

        template <class U, bool OtherIsConst>
        friend class BasicIterator;

        template <bool OtherIsConst, typename std::enable_if<!OtherIsConst && IsConst, int>::type = 0>
        BasicIterator(const BasicIterator<T, OtherIsConst> &other) : ptr(other.ptr) {}
        virtual reference operator*() const
        {
            if (!ptr)
            {
                throw std::runtime_error("Попытка разыменовать nullptr");
            }
            return *ptr;
        }

        node_pointer operator->() const { return ptr; }

        BasicIterator &operator++()
        {
            ptr = ptr->next;
            return *this;
        }

        BasicIterator operator++(int)
        {
            BasicIterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }

        BasicIterator &operator--()
        {
            ptr = ptr->prev;
            return *this;
        }

        BasicIterator operator--(int)
        {
            BasicIterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }
        BasicIterator operator+(difference_type shift) const
        {
            BasicIterator it = *this;
            while (shift-- > 0)
            {
                ++it;
            }
            return it;
        }
        BasicIterator operator-(difference_type shift) const
        {
            BasicIterator it = *this;
            while (shift-- > 0)
            {
                --it;
            }
            return it;
        }
        bool operator==(const BasicIterator &other) const { return ptr == other.ptr; }
        bool operator!=(const BasicIterator &other) const { return ptr != other.ptr; }
    };

    template <class T>
    using Iterator = BasicIterator<T, false>;

    template <class T>
    using ConstIterator = BasicIterator<T, true>;
}

namespace ull
{
    template <class T_VALUE_, class T_NODE_, bool IsConst, size_t MaxSize>
    class BasicIterator
    {
    protected:
        using NodeIterator = linkedList::BasicIterator<T_NODE_, IsConst>;
        using VectorIteratos = list::BaseIteratorVector<T_VALUE_, IsConst>;

        NodeIterator ptr_node_;
        VectorIteratos ptr_value_;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;

        using NodeValue = NodeIterator::value_type;
        using NodePointer = NodeIterator::pointer;
        using NodeRefernce = NodeIterator::reference;

        using value_type = VectorIteratos::value_type;
        using pointer = value_type *;
        using reference = value_type &;

        BasicIterator(NodeIterator ptr_node_ = NodeIterator(), VectorIteratos ptr_value_ = VectorIteratos())
            : ptr_node_(ptr_node_), ptr_value_(ptr_value_) {}

        reference operator*() const { return *ptr_value_; }
        pointer operator->() const { return ptr_value_.operator->(); }
        BasicIterator &operator++()
        {
            if (!is_it_null(ptr_node_))
            {
                size_t index = abs(ptr_value_ - VectorIteratos(&(ptr_node_->data_[0])));
                if (index + 1 < ptr_node_->data_.size())
                {
                    ++ptr_value_;
                }
                else if (!is_it_null(ptr_node_ + 1))
                {
                    ++ptr_node_;
                    ptr_value_ = VectorIteratos(&ptr_node_->data_[0]);
                }
                else
                {
                    ptr_value_ = VectorIteratos(&ptr_node_->data_[0] + ptr_node_->data_.size());
                }
            }
            return *this;
        }

        BasicIterator &operator--()
        {

            if (!is_it_null(ptr_node_))
            {
                size_t index = abs(ptr_value_ - VectorIteratos(&(ptr_node_->data_[0])));
                if (index > 0)
                {
                    --ptr_value_;
                }
                else if (!is_it_null(ptr_node_ - 1))
                {
                    --ptr_node_;
                    index = ptr_node_->data_.size() - 1;
                    ptr_value_ = VectorIteratos(&(ptr_node_->data_[0]) + index);
                }
                else
                {
                    ptr_value_ = VectorIteratos(&ptr_node_->data_[0] - 1);
                }
            }
            return *this;
        }
        BasicIterator operator++(int)
        {
            BasicIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        BasicIterator operator--(int)
        {
            BasicIterator tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const BasicIterator &other) const
        {
            return ptr_node_ == other.ptr_node_ && ptr_value_ == other.ptr_value_;
        }
        bool operator!=(const BasicIterator &other) const { return !(*this == other); }
        bool is_same_node(const BasicIterator &other) const
        {
            return ptr_node_ == other.ptr_node_;
        }
        VectorIteratos get_arr_iterator() const
        {
            return ptr_value_;
        }
        NodeIterator get_node_iterator() const
        {
            return ptr_node_;
        }
    };

    template <class Iterator_>
    class ReverseIterator
    {
        Iterator_ iterator;

    public:
        using iterator_category = typename Iterator_::iterator_category;
        using value_type = typename Iterator_::value_type;
        using difference_type = typename Iterator_::difference_type;
        using pointer = typename Iterator_::pointer;
        using reference = typename Iterator_::reference;

        ReverseIterator(const Iterator_ &it) : iterator(it) {}

        reference operator*() const { return *iterator; }
        pointer operator->() const { return iterator.operator->(); }

        ReverseIterator &operator++()
        {
            --iterator;
            return *this;
        }

        ReverseIterator &operator--()
        {
            ++iterator;
            return *this;
        }

        ReverseIterator operator++(int)
        {
            ReverseIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        ReverseIterator operator--(int)
        {
            ReverseIterator tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const ReverseIterator &other) const
        {
            return iterator == other.iterator;
        }

        bool operator!=(const ReverseIterator &other) const { return !(*this == other); }

        bool is_same_node(const ReverseIterator &other) const
        {
            return iterator.is_same_node(other.iterator);
        }
        Iterator_::VectorIteratos get_arr_iterator() const
        {
            return iterator.get_arr_iterator();
        }
        Iterator_::NodeIterator get_node_iterator() const
        {
            return iterator.get_node_iterator();
        }
    };
    template <class T_VALUE_, class T_NODE_, size_t MaxSize>
    using Iterator = BasicIterator<T_VALUE_, T_NODE_, false, MaxSize>;

    template <class T_VALUE_, class T_NODE_, size_t MaxSize>
    using ConstIterator = BasicIterator<T_VALUE_, T_NODE_, true, MaxSize>;
}