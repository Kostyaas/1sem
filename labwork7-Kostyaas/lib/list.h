#pragma once
#include <iostream>
#include <memory>
#include <initializer_list>
#include <VectorIterators.h>
#include <algorithm>

namespace list
{
    template <class T, size_t MaxSize, class Allocator = std::allocator<T>>
    class MyVector
    {
    private:
        using value_type = T;
        using AllocTraits = std::allocator_traits<Allocator>;
        using Iterator = list::Iterator<T>;
        using ConstIterator = list::ConstIterator<T>;
        using ReverseIterator = std::reverse_iterator<Iterator>;
        using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

        Allocator alloc;
        T *data;
        size_t size_;

    public:
        MyVector() : size_(0), data(AllocTraits::allocate(alloc, MaxSize)) {}

        MyVector(const MyVector &other)
            : alloc(AllocTraits::select_on_container_copy_construction(other.alloc)),
              size_(other.size_),
              data(AllocTraits::allocate(alloc, MaxSize))
        {
            try
            {
                for (size_t i = 0; i < size_; ++i)
                {
                    AllocTraits::construct(alloc, data + i, other.data[i]);
                }
            }
            catch (...)
            {
                clear();
                AllocTraits::deallocate(alloc, data, MaxSize);
                throw("Error copy object vector");
            }
        }

        MyVector &operator=(const MyVector &other)
        {
            if (this != &other)
            {
                clear();
                if (other.size_ > MaxSize)
                {
                    throw std::overflow_error("Cannot assign: source size exceeds MaxSize");
                }
                size_ = other.size_;
                try
                {
                    for (size_t i = 0; i < size_; ++i)
                    {
                        AllocTraits::construct(alloc, data + i, other.data[i]);
                    }
                }
                catch (...)
                {
                    clear();
                    throw("error vector =");
                }
            }
            return *this;
        }

        ~MyVector()
        {
            clear();
            AllocTraits::deallocate(alloc, data, MaxSize);
        }
        void push_begin(const T &value)
        {
            if (size_ >= MaxSize)
            {
                throw std::overflow_error("Vector is full, cannot push to beginning");
            }

            try
            {
                if (size_ > 0)
                {
                    std::move_backward(begin(), end(), end() + 1);
                }
                AllocTraits::construct(alloc, data, value);
                ++size_;
            }
            catch (...)
            {
                clear();
                throw std::runtime_error("Error in push_begin");
            }
        }
        void push_back(const T &value)
        {
            if (size_ < MaxSize)
            {
                AllocTraits::construct(alloc, data + size_, value);
                ++size_;
            }
            else
            {
                throw std::overflow_error("Vector is full");
            }
        }

        void clear()
        {
            for (size_t i = 0; i < size_; ++i)
            {
                AllocTraits::destroy(alloc, data + i);
            }
            size_ = 0;
        }

        T &operator[](size_t index)
        {
            if (index >= size_)
                throw std::out_of_range("Index out of range");
            return data[index];
        }
        Iterator erase_range(Iterator first, Iterator last)
        {
            if (first == last)
            {
                return first;
            }
            if (first < begin() || last > end() || first > last)
            {
                throw std::out_of_range("Invalid iterator range");
            }
            if (size_ == 0)
            {
                throw std::out_of_range("Cannot erase from empty vector");
            }
            try
            {
                size_t start_index = first - begin();
                size_t end_index = last - begin();
                for (size_t i = start_index; i < end_index; ++i)
                {
                    AllocTraits::destroy(alloc, data + i);
                }

                if (last != end())
                {
                    std::move(last, end(), first);
                }

                size_t deleted_count = end_index - start_index;
                size_ -= deleted_count;

                return first;
            }
            catch (...)
            {
                clear();
                throw std::runtime_error("Error in erase range");
            }
        }
        const T &operator[](size_t index) const
        {
            if (index >= size_)
                throw std::out_of_range("Index out of range");
            return data[index];
        }
        bool empty() { return size_ == 0; }
        size_t size() const { return size_; }
        void pop_back()
        {
            AllocTraits::destroy(alloc, data + size_-- - 1);
        }
        Iterator begin() { return Iterator(data); }
        Iterator end() { return Iterator(data + size_); }
        ConstIterator begin() const { return ConstIterator(data); }
        ConstIterator end() const { return ConstIterator(data + size_); }
        ConstIterator cbegin() const { return ConstIterator(data); }
        ConstIterator cend() const { return ConstIterator(data + size_); }
        ReverseIterator rbegin() { return ReverseIterator(end()); }
        ReverseIterator rend() { return ReverseIterator(begin()); }
        ConstReverseIterator rbegin() const { return ConstReverseIterator(end()); }
        ConstReverseIterator rend() const { return ConstReverseIterator(begin()); }
        ConstReverseIterator crbegin() const { return ConstReverseIterator(cend()); }
        ConstReverseIterator crend() const { return ConstReverseIterator(cbegin()); }
        template <typename Iter>
        void insert(Iter it, const value_type &value)
        {
            if (size_ >= MaxSize)
            {
                throw std::overflow_error("Vector is full, cannot insert");
            }
            if (it < begin() || it > end())
            {
                throw std::out_of_range("Iterator out of range");
            }
            size_t index = it - begin();
            std::move_backward(begin() + index, end(), end() + 1);
            AllocTraits::construct(alloc, data + index, value);
            ++size_;
        }

        void shrink_to_size(size_t new_size)
        {
            if (new_size > MaxSize)
            {
                throw std::out_of_range("New size exceeds MaxSize");
            }
            if (new_size < size_)
            {
                for (size_t i = new_size; i < size_; ++i)
                {
                    AllocTraits::destroy(alloc, data + i);
                }
            }
            size_ = new_size;
        }
        void move(size_t n)
        {
            n %= size_;
            std::rotate(rbegin(), rbegin() + n, rend());
        }
        void pop_front()
        {
            if (empty())
                return;
            AllocTraits::destroy(alloc, data);
            std::rotate(data, data + 1, data + size_);
            --size_;
        }
    };
}