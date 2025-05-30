#pragma once
#include <iterator>
#include <iostream>
#include <memory>
#include <IteratorsULL.h>
#include <list.h>
#include <IteratorHelp.h>

template <typename T, size_t NodeMaxSize = 10, class Allocator = std::allocator<T>>
class unrolled_list
{
private:
    struct Node
    {
        using value_type = list::MyVector<T, NodeMaxSize, Allocator>;
        Node *next;
        Node *prev;
        list::MyVector<T, NodeMaxSize, Allocator> data_;

        Node() : next(nullptr),
                 prev(nullptr),
                 data_(list::MyVector<T, NodeMaxSize, Allocator>())
        {
        }

        Node(const Node &other) : next(nullptr),
                                  prev(nullptr),
                                  data_(other.data_)
        {
        }

        Node &operator=(const Node &other)
        {
            if (this != &other)
            {
                data_ = other.data_;
            }
            return *this;
        }
        void clear() { data_.clear(); }
        bool empty() { return data_.empty(); }
        void add(const T &val)
        {
            data_.push_back(val);
        }
        void front(const T &value)
        {
            data_.push_begin(value);
        }
        size_t size()
        {
            return data_.size();
        }

        ~Node() = default;
    };
    using IteratorNode = linkedList::BasicIterator<Node, false>;
    using ConstIteratorNode = linkedList::BasicIterator<Node, true>;
    using AllocNode = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using AllocTraitsNode = typename std::allocator_traits<AllocNode>;

    using Iterator = ull::Iterator<T, Node, NodeMaxSize>;
    using ConstIterator = ull::ConstIterator<T, Node, NodeMaxSize>;
    using ReverseIterator = ull::ReverseIterator<Iterator>;
    using ReverseConstIterator = ull::ReverseIterator<ConstIterator>;

    AllocNode alloc_node;
    IteratorNode begin_;
    IteratorNode end_;
    size_t size_;

public:
    unrolled_list(const Allocator &allocator = Allocator()) : alloc_node(allocator), begin_(IteratorNode()), end_(begin_), size_(0) {}
    template <class Iterator>
    unrolled_list(const Iterator &begin, const Iterator &end, const Allocator &allocator = Allocator()) : alloc_node(allocator)
    {
        for (auto it = begin; it != end; ++it)
        {
            push_back(*it);
        }
    }
    unrolled_list(std::initializer_list<T> init, const Allocator &allocator = Allocator())
        : alloc_node(allocator), begin_(IteratorNode()), end_(begin_), size_(0)
    {
        for (const T &value : init)
        {
            push_back(value);
        }
    }
    ~unrolled_list()
    {
        if (is_it_null(begin_))
        {
            return;
        }
        Node *current = &(*begin_);
        while (current)
        {
            Node *tmp = current->next;
            AllocTraitsNode::destroy(alloc_node, current);
            AllocTraitsNode::deallocate(alloc_node, current, 1);
            current = tmp;
        }
    }
    template <typename... Args>
    void emplace_back(Args &&...arg)
    {
        static_assert(sizeof...(Args) == 1, "emplace_back принимает только один аргумент");
        if (is_it_null(begin_))
        {
            Node *new_node = nullptr;
            try
            {
                new_node = AllocTraitsNode::allocate(alloc_node, 1);
                AllocTraitsNode::construct(alloc_node, new_node);
                new_node->add(T(arg...));
                begin_ = IteratorNode(new_node);
                end_ = begin_;
            }
            catch (...)
            {
                if (new_node)
                {
                    AllocTraitsNode::destroy(alloc_node, new_node);
                    AllocTraitsNode::deallocate(alloc_node, new_node, 1);
                }
                throw;
            }
            ++size_;
            return;
        }
        end_->add(T(arg...));
        shift(end_);
        ++size_;
    }
    template<class It>
    void shift(const It &first, size_t number_added_elements = 0, const It &prev = It())
    {
        if (first->size() >= NodeMaxSize)
        {
            It second = first + 1;
            if (is_it_null(second))
            {
                Node *new_node = nullptr;
                try
                {
                    new_node = AllocTraitsNode::allocate(alloc_node, 1);
                    AllocTraitsNode::construct(alloc_node, new_node);
                    new_node->next = nullptr;
                    new_node->prev = &(*first);
                    first->next = new_node;
                    second = It(new_node);
                }
                catch (...)
                {
                    AllocTraitsNode::destroy(alloc_node, new_node);
                    AllocTraitsNode::deallocate(alloc_node, new_node, 1);
                    throw;
                }
                size_t mid = (first->data_.size() + number_added_elements) / 2;
                auto it = first->data_.begin() + mid;
                try
                {
                    while (it != first->data_.end())
                    {
                        second->data_.push_back(std::move(*it)); // либо просто убрать move - но так быстрее так как передаем обхект по rvalue ссылки
                        ++it;
                    }
                    first->data_.shrink_to_size(mid);
                }
                catch (...)
                {
                    throw;
                }
                ++end_;
            }
            else
            {
                size_t mid = (first->data_.size() + number_added_elements) / 2;
                shift(second, first->data_.size() - mid, first);
                auto it = first->data_.begin() + mid;
                try
                {
                    while (it != first->data_.end())
                    {
                        second->data_.push_begin(*it);
                        ++it;
                    }
                    first->data_.erase_range(first->data_.begin() + mid, first->data_.end());
                }
                catch (...)
                {
                    throw;
                }
            }
        }
        else
        {
            return;
        }
    }
    size_t calculate_memory()
    {
        size_t total_memory = sizeof(*this);
        IteratorNode current = begin_;

        while (!is_it_null(current))
        {
            total_memory += sizeof(Node);
            total_memory += NodeMaxSize * sizeof(T);
            current = current + 1;
        }
        return total_memory;
    }
    Iterator begin()
    {
        if (is_it_null(begin_))
        {
            return Iterator(begin_);
        }
        return Iterator(begin_, begin_->data_.begin());
    }

    Iterator end()
    {
        if (is_it_null(end_))
        {
            return Iterator(end_);
        }
        return Iterator(end_, end_->data_.end());
    }

    ReverseIterator rbegin()
    {
        if (is_it_null(end_))
        {
            return ReverseIterator(Iterator(end_));
        }
        return ReverseIterator(Iterator(end_, end_->data_.end() - 1));
    }

    ReverseIterator rend()
    {
        if (is_it_null(begin_))
        {
            return ReverseIterator(Iterator(begin_));
        }
        return ReverseIterator(Iterator(begin_, begin_->data_.begin() - 1));
    }
    ConstIterator begin() const
    {
        if (is_it_null(begin_))
        {
            return ConstIterator(begin_);
        }
        return ConstIterator(begin_, begin_->data_.cbegin());
    }

    ConstIterator end() const
    {
        if (is_it_null(end_))
        {
            return ConstIterator(end_);
        }
        return ConstIterator(end_, end_->data_.cend());
    }

    ConstIterator cbegin() const
    {
        if (is_it_null(begin_))
        {
            return ConstIterator(begin_);
        }
        return ConstIterator(begin_, begin_->data_.cbegin());
    }

    ConstIterator cend() const
    {
        if (is_it_null(end_))
        {
            return ConstIterator(end_);
        }
        return ConstIterator(end_, end_->data_.cend());
    }

    ReverseConstIterator crbegin() const
    {
        if (is_it_null(end_))
        {
            return ReverseConstIterator(ConstIterator(end_));
        }
        return ReverseConstIterator(ConstIterator(end_, end_->data_.cend() - 1));
    }

    ReverseConstIterator crend() const
    {
        if (is_it_null(begin_))
        {
            return ReverseConstIterator(ConstIterator(begin_));
        }
        return ReverseConstIterator(ConstIterator(begin_, begin_->data_.cbegin() - 1));
    }
    void push_back(const T &value)
    {
        if (is_it_null(begin_))
        {
            Node *new_node = nullptr;
            try
            {
                new_node = AllocTraitsNode::allocate(alloc_node, 1);
                AllocTraitsNode::construct(alloc_node, new_node);
                new_node->add(value);
                begin_ = IteratorNode(new_node);
                end_ = begin_;
            }
            catch (...)
            {
                if (new_node)
                {
                    AllocTraitsNode::destroy(alloc_node, new_node);
                    AllocTraitsNode::deallocate(alloc_node, new_node, 1);
                }
                throw;
            }
            ++size_;
            return;
        }
        end_->add(value);
        shift(end_);
        ++size_;
    }
    void push_front(const T &value)
    {
        if (is_it_null(begin_))
        {
            push_back(value);
            return;
        }
        begin_->front(value);
        shift(begin_);
        ++size_;
    }
    size_t size()
    {
        return size_;
    }
    bool empty()
    {
        return size_ == 0;
    }
    size_t max_size()
    {
        return NodeMaxSize;
    }
    void clear()
    {
        if (is_it_null(begin_))
        {
            return;
        }
        Node *current = &(*begin_);
        while (current)
        {
            Node *next = current->next;
            current->data_.clear();
            AllocTraitsNode::destroy(alloc_node, current);
            AllocTraitsNode::deallocate(alloc_node, current, 1);
            current = next;
        }

        begin_ = IteratorNode();
        end_ = begin_;
        size_ = 0;
    }
    void pop_back()
    {
        if (empty())
        {
            std::cerr << "arr is empty, cannot pop" << std::endl;
            return;
        }

        --size_;

        end_->data_.pop_back();

        if (end_->data_.size() == 0)
        {
            Node *current = &(*end_);

            if (begin_ == end_)
            {
                AllocTraitsNode::destroy(alloc_node, current);
                AllocTraitsNode::deallocate(alloc_node, current, 1);
                begin_ = IteratorNode();
                end_ = begin_;
            }
            else
            {
                --end_;
                end_->next = nullptr;

                AllocTraitsNode::destroy(alloc_node, current);
                AllocTraitsNode::deallocate(alloc_node, current, 1);
            }
        }
    }
    void pop_front()
    {
        if (empty())
        {
            std::cerr << "List is empty: ... ? pop is doesnt" << std::endl;
            return;
        }
        begin_->data_.pop_front();
        --size_;
        if (begin_->data_.empty())
        {
            Node *current = &(*begin_);

            if (begin_ == end_)
            {
                AllocTraitsNode::destroy(alloc_node, current);
                AllocTraitsNode::deallocate(alloc_node, current, 1);
                begin_ = IteratorNode();
                end_ = begin_;
            }
            else
            {
                ++begin_;
                begin_->prev = nullptr;
                AllocTraitsNode::destroy(alloc_node, current);
                AllocTraitsNode::deallocate(alloc_node, current, 1);
            }
        }
    }
    template <class Iterator>
    void erase_range(Iterator &first, Iterator &second)
    {
        if (first == second)
        {
            return;
        }

        if (first.is_same_node(second))
        {
            // Случай 1: Итераторы указывают на одну ноду
            IteratorNode node_it = first.get_node_iterator();
            auto arr_first = first.get_arr_iterator();
            auto arr_second = second.get_arr_iterator();

            size_t count = std::distance(arr_first, arr_second);
            size_ -= count;

            node_it->data_.erase_range(arr_first, arr_second);

            if (node_it->data_.empty())
            {
                Node *current = &(*node_it);

                if (begin_ == end_)
                {
                    AllocTraitsNode::destroy(alloc_node, current);
                    AllocTraitsNode::deallocate(alloc_node, current, 1);
                    begin_ = IteratorNode();
                    end_ = begin_;
                }
                else if (node_it == begin_)
                {
                    begin_ = begin_ + 1;
                    begin_->prev = nullptr;
                    AllocTraitsNode::destroy(alloc_node, current);
                    AllocTraitsNode::deallocate(alloc_node, current, 1);
                }
                else if (node_it == end_)
                {
                    end_ = end_ - 1;
                    end_->next = nullptr;
                    AllocTraitsNode::destroy(alloc_node, current);
                    AllocTraitsNode::deallocate(alloc_node, current, 1);
                }
                else
                {
                    IteratorNode prev_node = node_it - 1;
                    IteratorNode next_node = node_it + 1;
                    prev_node->next = &(*next_node);
                    next_node->prev = &(*prev_node);
                    AllocTraitsNode::destroy(alloc_node, current);
                    AllocTraitsNode::deallocate(alloc_node, current, 1);
                }
            }

            first = second;
        }
        else
        {
            IteratorNode first_node = first.get_node_iterator();
            IteratorNode second_node = second.get_node_iterator();

            // 1. Удаляем элементы в первой ноде (от first до конца data_)
            auto arr_first = first.get_arr_iterator();
            size_t count = std::distance(arr_first, first_node->data_.end());
            size_ -= count;
            first_node->data_.erase_range(arr_first, first_node->data_.end());

            // 2. Удаляем промежуточные ноды (между first_node и second_node)
            IteratorNode current_node = first_node + 1;
            while (current_node != second_node)
            {
                IteratorNode next_node = current_node + 1;
                size_ -= current_node->data_.size();
                Node *current = &(*current_node);
                AllocTraitsNode::destroy(alloc_node, current);
                AllocTraitsNode::deallocate(alloc_node, current, 1);
                current_node = next_node;
            }

            // 3. Удаляем элементы в последней ноде (от начала до second)
            auto arr_second = second.get_arr_iterator();
            count = std::distance(second_node->data_.begin(), arr_second);
            size_ -= count;
            second_node->data_.erase_range(second_node->data_.begin(), arr_second);

            // 4. Обновляем связи между оставшимися нодами
            if (first_node->data_.empty() && second_node->data_.empty())
            {
                // Обе ноды пустые
                if (first_node == begin_ && second_node == end_)
                {
                    // Удаляем все ноды
                    begin_ = IteratorNode();
                    end_ = begin_;
                }
                else if (first_node == begin_)
                {
                    // Удаляем first_node, second_node - последняя
                    begin_ = second_node + 1;
                    if (!is_it_null(begin_))
                    {
                        begin_->prev = nullptr;
                    }
                    end_ = IteratorNode();
                }
                else if (second_node == end_)
                {
                    // Удаляем second_node, first_node - первая
                    end_ = first_node - 1;
                    end_->next = nullptr;
                }
                else
                {
                    // Удаляем обе ноды в середине
                    IteratorNode prev_first = first_node - 1;
                    IteratorNode next_second = second_node + 1;
                    prev_first->next = &(*next_second);
                    next_second->prev = &(*prev_first);
                }

                // Удаляем сами ноды
                if (first_node->data_.empty())
                {
                    Node *first_ptr = &(*first_node);
                    AllocTraitsNode::destroy(alloc_node, first_ptr);
                    AllocTraitsNode::deallocate(alloc_node, first_ptr, 1);
                }
                if (second_node->data_.empty() && first_node != second_node)
                {
                    Node *second_ptr = &(*second_node);
                    AllocTraitsNode::destroy(alloc_node, second_ptr);
                    AllocTraitsNode::deallocate(alloc_node, second_ptr, 1);
                }
            }
            else if (first_node->data_.empty())
            {
                // Только первая нода пустая
                if (first_node == begin_)
                {
                    begin_ = second_node;
                    begin_->prev = nullptr;
                }
                else
                {
                    IteratorNode prev_first = first_node - 1;
                    prev_first->next = &(*second_node);
                    second_node->prev = &(*prev_first);
                }
                Node *first_ptr = &(*first_node);
                AllocTraitsNode::destroy(alloc_node, first_ptr);
                AllocTraitsNode::deallocate(alloc_node, first_ptr, 1);
            }
            else if (second_node->data_.empty())
            {
                // Только вторая нода пустая
                if (second_node == end_)
                {
                    end_ = first_node;
                    end_->next = nullptr;
                }
                else
                {
                    IteratorNode next_second = second_node + 1;
                    first_node->next = &(*next_second);
                    next_second->prev = &(*first_node);
                }
                Node *second_ptr = &(*second_node);
                AllocTraitsNode::destroy(alloc_node, second_ptr);
                AllocTraitsNode::deallocate(alloc_node, second_ptr, 1);
            }
            else
            {
                // Обе ноды не пустые, просто обновляем связи
                first_node->next = &(*second_node);
                second_node->prev = &(*first_node);
            }
            first = second;
        }
    }
    void insert(const Iterator& it, const T& value){
        
        it.get_node_iterator()->data_.insert(it.get_arr_iterator(),value);
        shift(it.get_node_iterator());
        ++size_;
    }
};
