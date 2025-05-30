#pragma once
#include <variant>
#include <iterator>
#include <filesystem>
#include <string>
#include <variant>
#include <fstream>
#include <set>
#include <type_traits>
#include <map>
#include <optional>
#include <utility>
#include <unordered_map>
#include <vector>
namespace fs = std::filesystem;

class Dir
{
public:
	class Iterator
	{
	public:
		using DirIter = fs::directory_iterator;
		using RecDirIter = fs::recursive_directory_iterator;

		using value_type = fs::directory_entry;
		using difference_type = std::ptrdiff_t;
		using pointer = const value_type *;
		using reference = const value_type &;
		using iterator_category = std::input_iterator_tag;
		using VariantIter = std::variant<DirIter, RecDirIter>;

		Iterator() = default;

		Iterator(const std::string &path, bool recursive)
		{

			if (recursive)
				iter = RecDirIter(path);
			else
				iter = DirIter(path);
		}

		static Iterator end(bool recursive)
		{
			Iterator it;
			if (recursive)
				it.iter = RecDirIter{};
			else
				it.iter = DirIter{};
			return it;
		}

		reference operator*() const
		{
			return std::visit([](auto &&it) -> const value_type &
							  { return *it; }, iter);
		}
		pointer operator->() const
		{
			return std::visit([](auto &&it) -> const value_type *
							  { return &(*it); }, iter);
		}
		Iterator &operator++()
		{
			std::visit([](auto &&it)
					   { ++it; }, iter);
			return *this;
		}

		bool operator!=(const Iterator &other) const
		{
			return iter != other.iter;
		}

	private:
		VariantIter iter;
	};
	Dir() = default;

	explicit Dir(const std::string &path, bool recurs)
		: path(path), recursive(recurs) {}

	Iterator begin() const
	{
		return Iterator(path, recursive);
	}

	Iterator end() const
	{
		return Iterator::end(recursive);
	}

private:
	std::string path;
	bool recursive;
};

template <class FUNC>
class Filter
{
public:
	Filter(FUNC func) : func(func) {}

	template <typename Iter>
	class Iterator
	{
	public:
		using value_type = typename std::iterator_traits<Iter>::value_type;
		using difference_type = std::ptrdiff_t;
		using pointer = const value_type *;
		using reference = const value_type &;
		using iterator_category = std::input_iterator_tag;

		Iterator(Iter current, Iter end, FUNC func)
			: current(current), end(end), func(func)
		{
			skip_invalid();
		}

		reference operator*() const
		{
			return *current;
		}
		pointer operator->() const
		{
			return &(*current);
		}

		Iterator &operator++()
		{
			++current;
			skip_invalid();
			return *this;
		}

		bool operator!=(const Iterator &other) const
		{
			return current != other.current;
		}

	private:
		Iter current, end;
		FUNC func;

		void skip_invalid()
		{
			while (current != end && !func(*current))
			{
				++current;
			}
		}
	};

	template <typename Container>
	struct View
	{
		std::shared_ptr<Container> container;
		FUNC func;

		auto begin() const
		{
			return Iterator<decltype(std::begin(*container))>(
				std::begin(*container), std::end(*container), func);
		}
		auto end() const
		{
			return Iterator<decltype(std::begin(*container))>(
				std::end(*container), std::end(*container), func);
		}
	};

	template <typename Container>
	View<Container> apply(const Container &container) const
	{
		return View<Container>{std::make_shared<Container>(container), func};
	}

private:
	FUNC func;
};

class OpenFiles
{
public:
	template <typename Iter>
	class Iterator
	{
	public:
		using value_type = std::ifstream;
		using difference_type = std::ptrdiff_t;
		using pointer = std::ifstream *;
		using reference = std::ifstream &;
		using iterator_category = std::input_iterator_tag;

		Iterator(Iter current, Iter end)
			: current(current), end(end), file_stream(std::make_unique<std::ifstream>())
		{
			skip_invalid();
			open_file();
		}

		reference operator*() const
		{
			return *file_stream;
		}

		pointer operator->() const
		{
			return file_stream.get();
		}

		Iterator &operator++()
		{
			++current;
			skip_invalid();
			open_file();
			return *this;
		}

		bool operator!=(const Iterator &other) const
		{
			return current != other.current;
		}

	private:
		Iter current, end;
		std::unique_ptr<std::ifstream> file_stream;

		void skip_invalid()
		{
			while (current != end && !current->is_regular_file())
			{
				++current;
			}
		}

		void open_file()
		{
			if (current != end)
			{
				file_stream = std::make_unique<std::ifstream>(current->path());
				if (!file_stream->is_open())
				{
					++(*this);
				}
			}
		}
	};

	template <typename Container>
	struct View
	{
		std::shared_ptr<Container> container;
		auto begin() const
		{
			return Iterator<decltype(std::begin(*container))>(
				std::begin(*container), std::end(*container));
		}

		auto end() const
		{
			return Iterator<decltype(std::begin(*container))>(
				std::end(*container), std::end(*container));
		}
	};

	template <typename Container>
	View<Container> apply(const Container &container) const
	{
		return View<Container>{std::make_shared<Container>(container)};
	}
};

class Split
{
public:
	template <typename Iter>
	class Iterator
	{
	public:
		using value_type = std::string;
		using difference_type = std::ptrdiff_t;
		using pointer = const value_type *;
		using reference = const value_type &;
		using iterator_category = std::input_iterator_tag;
		Iterator() = default;
		Iterator(Iter current, Iter end, const bool *delimiters, bool is_end = false)
			: current_iter(std::move(current)), end_iter(std::move(end)), delimiters(delimiters), end_flag(is_end)
		{
			if (!end_flag && current_iter != end_iter)
				readNext();
			else
				end_flag = true;
		}

		reference operator*() const { return current; }
		pointer operator->() const { return &current; }

		Iterator &operator++()
		{
			readNext();
			return *this;
		}

		bool operator==(const Iterator &other) const
		{
			return !(end_flag != other.end_flag) && !(current_iter != other.current_iter);
		}

		bool operator!=(const Iterator &other) const
		{
			return !(*this == other);
		}

	private:
		Iter current_iter;
		Iter end_iter;
		std::istream *stream = nullptr;
		const bool *delimiters;
		std::string current;
		bool end_flag;

		void readNext()
		{
			current.clear();

			while (true)
			{
				if (!stream && current_iter != end_iter)
				{
					stream = &(*current_iter);
					++current_iter;
				}

				if (!stream || !(*stream))
				{
					stream = nullptr;
					if (current.empty())
					{
						if (!(current_iter != end_iter))
						{
							end_flag = true;
						}
					}
					return;
				}

				char ch;
				while (stream && stream->get(ch))
				{
					if (delimiters[static_cast<unsigned char>(ch)])
					{
						if (!current.empty())
							return;
					}
					else
					{
						current += ch;
					}
				}

				stream = nullptr;
				if (!current.empty())
					return;
			}
		}
	};
	Split(const std::string &delim)
	{
		std::fill(std::begin(delim_flags), std::end(delim_flags), false);
		for (unsigned char ch : delim)
		{
			delim_flags[ch] = true;
		}
	}

	template <typename Container>
	class View
	{
	public:
		View(std::shared_ptr<Container> c, const bool *flags) : container(c), delimiters(flags) {}
		auto begin() const
		{
			return Iterator<decltype(std::begin(*container))>(
				std::begin(*container), std::end(*container), delimiters);
		}
		auto end() const
		{
			return Iterator<decltype(std::begin(*container))>(
				std::end(*container), std::end(*container), delimiters, true);
		}

	private:
		std::shared_ptr<Container> container;
		const bool *delimiters;
	};

	template <typename Container>
	View<Container> apply(const Container &container) const
	{
		return View<Container>{std::make_shared<Container>(container), delim_flags};
	}

private:
	bool delim_flags[256];
};

template <class FUNC>
class Transform
{
public:
	Transform(const FUNC &func) : func(func) {}

	template <class Iter>
	class Iterator
	{
	public:
		using input_type = Iter::reference;
		using value_type = std::invoke_result_t<FUNC, input_type>;
		using pointer = const value_type *;
		using reference = const value_type &;

		Iterator(Iter it, FUNC func) : current(std::move(it)), func(func) { Get_Result(); }

		reference operator*() const
		{
			return result;
		}
		pointer operator->() const
		{
			return *result;
		}
		Iterator &operator++()
		{
			++current;
			Get_Result();
			return *this;
		}

		bool operator!=(const Iterator &other) const
		{
			return current != other.current;
		}

	private:
		void Get_Result()
		{
			result = func(*current);
		}
		value_type result;
		Iter current;
		FUNC func;
	};
	template <typename Container>
	struct View
	{
		std::shared_ptr<Container> container;
		const FUNC &func;
		auto begin() const
		{
			return Iterator<decltype(std::begin(*container))>(
				std::begin(*container), func);
		}

		auto end() const
		{
			return Iterator<decltype(std::begin(*container))>(
				std::end(*container), func);
		}
	};

	template <typename Container>
	View<Container> apply(const Container &container) const
	{
		return View<Container>{std::make_shared<Container>(container), func};
	}

private:
	FUNC func;
};

template <typename Aggregator, typename KeyExtractor>
class AggregateByKey
{
public:
	AggregateByKey(Aggregator aggregator, KeyExtractor keyExtractor)
		: aggregator(std::move(aggregator)), keyExtractor(std::move(keyExtractor)) {}

	template <typename Container>
	auto apply(const Container &container) const
	{
		using Item = decltype(*container.begin());
		using Key = std::decay_t<decltype(keyExtractor(std::declval<Item>()))>;
		using Value = std::size_t;

		std::map<Key, Value> result;

		for (const auto &item : container)
		{
			Key key = keyExtractor(item);
			auto it = result.find(key);

			if (it != result.end())
			{
				aggregator(it->second);
			}
			else
			{
				result[key] = 1; // начинаем с 1
			}
		}

		return result;
	}

private:
	Aggregator aggregator;
	KeyExtractor keyExtractor;
};

class Out
{	
public:
	Out(std::ostream &output) : output(output) {}
	template<class container>
	void apply(const container& obj){
		for(auto&& elemets : obj){
			output << elemets << std::endl;
		}
	}
private:
	std::ostream &output;
};




template <typename K, typename V>
struct KV {
    K key;
    V value;
    
    KV(K k, V v) : key(std::move(k)), value(std::move(v)) {}
};

template <typename L, typename R>
struct JoinResult {
    std::optional<L> left;
    std::optional<R> right;
    
    JoinResult(std::optional<L> l, std::optional<R> r) 
        : left(std::move(l)), right(std::move(r)) {}
};

template <typename T, typename Source>
class DropNullopt {
public:
    explicit DropNullopt(Source source) : source_(std::move(source)) {}
    
    template <typename Container>
    auto apply(const Container& container) const {
        using ValueType = typename Container::value_type;
        static_assert(std::is_same_v<ValueType, std::optional<T>>, 
                     "DropNullopt requires container of std::optional<T>");
        
        std::vector<T> result;
        for (const auto& item : container) {
            if (item.has_value()) {
                result.push_back(std::move(*item));
            }
        }
        return result;
    }

private:
    Source source_;
};

template <typename K, typename L, typename R, typename LeftSource, typename RightSource>
class Join {
public:
    Join(LeftSource left, RightSource right) 
        : left_(std::move(left)), right_(std::move(right)) {}

    template <typename Container>
    auto apply(const Container& left_container) const {
        std::vector<JoinResult<L, R>> result;
        
        std::unordered_map<K, R> right_table;
        for (const auto& item : right_.apply(left_container)) {
            right_table.emplace(item.key, std::move(item.value));
        }
        
        for (const auto& left_item : left_container) {
            auto it = right_table.find(left_item.key);
            if (it != right_table.end()) {
                result.emplace_back(
                    std::move(left_item.value),
                    std::move(it->second)
                );
            } else {
                result.emplace_back(
                    std::move(left_item.value),
                    std::nullopt
                );
            }
        }
        
        return result;
    }

private:
    LeftSource left_;
    RightSource right_;
};

template <typename K, typename L, typename R, typename LeftSource, typename RightSource>
auto make_join(LeftSource&& left, RightSource&& right) {
    return Join<K, L, R, LeftSource, RightSource>(
        std::forward<LeftSource>(left),
        std::forward<RightSource>(right)
    );
}
template<class OBJ_>
std::vector<typename OBJ_::value_type >As_Vector(OBJ_&& obj) {
    using value_type = typename std::iterator_traits<
        decltype(std::begin(obj))>::value_type;
    
    return std::vector<value_type>(
        std::make_move_iterator(std::begin(obj)),
        std::make_move_iterator(std::end(obj))
    );
}
template <typename USED, typename USE>
auto operator|(USED &&range, USE &&filter)
{
	return filter.apply(range);
}

