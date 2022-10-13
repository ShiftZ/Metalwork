#pragma once

#include <vector>
#include <algorithm>

template< typename type, typename compare = std::less<type>, typename allocator = std::allocator<type> >
class flat_set : public std::vector<type, allocator>
{
public:
	using native = std::vector<type, allocator>;
	using typename native::iterator;
	using key_type = type;
	using value_compare = compare;

private:
	using native::push_back;
	using native::emplace_back;

public:
	using native::begin;
	using native::end;
	using native::erase;

public:
	flat_set() = default;
	flat_set( std::initializer_list<type> list ) : native(move(list))
	{
		std::sort(begin(), end(), compare());
	}

	template< typename iterator_type >
	flat_set( iterator_type first, iterator_type last ) : native(first, last)
	{
		std::sort(begin(), end(), compare());
	}

	std::pair<iterator, bool> insert( const type& val )
	{
		iterator it = std::lower_bound(begin(), end(), val, compare());
		if (it != end() && !compare()(val, *it)) return {it, false};
		return {native::insert(it, val), true};
	}

	std::pair<iterator, bool> insert( type&& val )
	{
		iterator it = std::lower_bound(begin(), end(), val, compare());
		if (it != end() && !compare()(val, *it)) return {it, false};
		return {native::emplace(it, move(val)), true};
	}

	template< typename it_type >
	void insert( it_type first, const it_type& last )
	{
		while (first != last)
		{
			insert(*first);
			++first;
		}
	}
	 
	template< typename... types >
	std::pair<iterator, bool> emplace( types&&... args )
	{
		return insert(type(std::forward<types>(args)...));
	}

	template< typename lookup_type >
	iterator find( const lookup_type& val )
	{
		iterator it = std::lower_bound(begin(), end(), val, compare());
		return (it != end() && !compare()(val, *it)) ? it : end();
	}

	void erase( const type& val )
	{
		iterator it = std::lower_bound(begin(), end(), val, compare());
		if (it != end() && !compare()(val, *it)) native::erase(it);
	}
};

template< typename type, typename compare = std::less<type>, typename allocator = std::allocator<type> >
class flat_multiset : public std::vector<type, allocator>
{
public:
	using native = std::vector<type, allocator>;
	using typename native::iterator;
	using typename native::const_iterator;
	using value_compare = compare;
	using key_type = type;

private:
	using native::push_back;
	using native::emplace_back;

public:
	using native::begin;
	using native::end;
	using native::erase;

public:
	flat_multiset() = default;
	flat_multiset( std::initializer_list<type> list ) : native(list) { std::sort(begin(), end(), compare()); }

	template< typename iterator_type >
	flat_multiset( iterator_type first, iterator_type last ) : native(first, last)
	{
		std::sort(begin(), end(), compare());
	}

	iterator insert( const type& val )
	{
		iterator it = std::upper_bound(begin(), end(), val, compare());
		return native::insert(it, val);
	}

	iterator insert( type&& val )
	{
		iterator it = std::upper_bound(begin(), end(), val, compare());
		return native::insert(it, move(val));
	}

	template< typename... types >
	iterator emplace( types&&... args )
	{
		return insert(type(std::forward<types>(args)...));
	}

	template< typename lookup_type >
	iterator find( const lookup_type& val )
	{
		iterator it = std::lower_bound(begin(), end(), val, compare());
		return (it != end() && !compare()(val, *it)) ? it : end();
	}

	template< typename lookup_type >
	std::pair<iterator, iterator> equal_range( const lookup_type& val )
	{
		std::pair<iterator, iterator> range;
		range.first = range.second = find(val);
		while (range.second != end() && !compare()(val, *range.second)) ++range.second;
		return range;
	}

	void erase( const type& val )
	{
		std::pair<iterator, iterator> range = equal_range(val);
		if (range.first != range.second) native::erase(range.first, range.second);
	}
};

template< typename key, typename mapped, typename key_compare >
struct compare_pair
{
	using pair = std::pair<key, mapped>;

	bool operator()( const pair& a, const pair& b ) const { return key_compare()(a.first, b.first); }
	bool operator()( const key& a, const pair& b ) const { return key_compare()(a, b.first); }
	bool operator()( const pair& a, const key& b ) const { return key_compare()(a.first, b); }
};

template< typename key, typename mapped,
		  typename compare = std::less<key>,
		  typename allocator = std::allocator<std::pair<key, mapped>> >
class flat_map : public flat_set<std::pair<key, mapped>, compare_pair<key, mapped, compare>, allocator>
{
public:
	using base = flat_set<std::pair<key, mapped>, compare_pair<key, mapped, compare>, allocator>;
	using key_type = key;
	using mapped_type = mapped;
	using key_compare = compare;
	using value_compare = compare_pair<key, mapped, key_compare>;
	using typename base::native;
	using typename native::iterator;
	using typename native::const_iterator;

public:
	using base::flat_set;
	using native::erase;
	using native::begin;
	using native::end;
	using native::cbegin;
	using native::cend;

public:
	void erase( const key& k )
	{
		auto it = std::lower_bound(begin(), end(), k, value_compare());
		if (it != end() && !value_compare()(k, *it)) native::erase(it);
	}

	template< typename key_type, typename... types > requires std::is_convertible_v<key_type&&, const key&>
	std::pair<iterator, bool> try_emplace( key_type&& k, types&&... args )
	{
		using namespace std;
		auto it = lower_bound(begin(), end(), k, value_compare());
		if (it != end() && !value_compare()(k, *it)) return {it, false};
		it = native::emplace(it, piecewise_construct_t(), 
			forward_as_tuple(forward<key_type>(k)), forward_as_tuple(forward<types>(args)...));
		return {it, true};
	}

	template< typename key_type, typename... types > requires std::is_convertible_v<key_type&&, const key&>
	std::pair<iterator, bool> try_emplace( const_iterator hint, key_type&& k, types&&... args )
	{
		using namespace std;
		bool left = (hint == cbegin() || value_compare()(*(hint - 1), k));
		bool right = (hint == cend() || value_compare()(k, *hint));
		iterator it = begin() + distance(cbegin(), hint);
		if (left && !right)
			it = lower_bound(it, end(), k, value_compare());
		else if (!left && right)
			it = lower_bound(begin(), it, k, value_compare());
		if (!value_compare()(k, *it)) return {it, false};
		it = native::emplace(it, piecewise_construct_t(), 
			forward_as_tuple(forward<key_type>(k)), forward_as_tuple(forward<types>(args)...));
		return {it, true};
	}

	mapped& operator[]( const key& k )
	{
		auto it = std::lower_bound(begin(), end(), k, value_compare());
		if (it != end() && !value_compare()(k, *it)) return it->second;
		return native::emplace(it, k, mapped())->second;
	}

	mapped& operator[]( key&& k )
	{
		auto it = std::lower_bound(begin(), end(), k, value_compare());
		if (it != end() && !value_compare()(k, *it)) return it->second;
		return native::emplace(it, std::move(k), mapped())->second;
	}
};

template< typename key, typename mapped,
		  typename key_compare = std::less<key>,
		  typename allocator = std::allocator<std::pair<key, mapped>> >
class flat_multimap : public flat_multiset<std::pair<key, mapped>, compare_pair<key, mapped, key_compare>, allocator>
{
public:
	using base = flat_multiset<std::pair<key, mapped>, compare_pair<key, mapped, key_compare>, allocator>;
	using key_type = key;
	using mapped_type = mapped;

public:
	using base::flat_multiset;
	using base::erase;

public:
	void erase( const key& k )
	{
		auto range = equal_range(k);
		if (range.first != range.second)
			base::erase(range.first, range.second);
	}
};

