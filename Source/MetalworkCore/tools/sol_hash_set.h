#pragma once

#include <type_traits>
#include <functional>
#include <memory>
#include <bit>
#include <limits>
#include <iterator>

inline size_t bit_reverse( size_t value )
{
	static constexpr std::array<uint8_t, 256> bit_reverse_lookup = []() consteval
	{
		std::array<uint8_t, 256> table;
		for (int i = 0; i < table.size(); ++i)
			table[i] = uint8_t(((i * 0x0802lu & 0x22110lu) | (i * 0x8020lu & 0x88440lu)) * 0x10101lu >> 16);
		return table;
	}();

	size_t result;
	uint8_t* v = (uint8_t*)&value;
	uint8_t* r = (uint8_t*)&result;

	for (int i = 0; i < sizeof(value); i++)
		r[i] = bit_reverse_lookup[v[sizeof(value) - 1 - i]];

	/* this is non-lookup-table bit reverse version. works only on 64bit
	for (int i = 0; i < sizeof(value); i++)
		r[i] = uint8_t(((v[sizeof(value) - 1 - i] * 0x80200802ULL) & 0x0884422110ULL) * 0x0101010101ULL >> 32);
	*/

    return result;
}

template< typename value_t,
		  typename hash_t = std::hash<value_t>,
		  typename compare_t = std::equal_to<value_t>,
		  typename allocator_t = std::allocator<value_t> >
class sol_hash_set /* split-order list hash set */
{
protected:
	struct node
	{
		size_t r_hash;
		std::atomic<node*> next;
		value_t& value() { return ((value_node*)this)->value; }
	};

	struct value_node : node
	{
		union { value_t value; };
		~value_node() { if (~node::r_hash & 1) value.~value_t(); }
	};

	struct table
	{
		size_t mask, capacity;

		std::atomic<node*>* buckets() { return (std::atomic<node*>*)(this + 1); }
		size_t size() const { return mask + 1; }
		static size_t bytes( size_t size ) { return sizeof(table) + sizeof(std::atomic<node*>) * size; }
	};

public:
	using value_type = value_t;
	using compare = compare_t;
	using node_type = value_node;

	static constexpr size_t load_factor = 85;
	static constexpr size_t minimal_size = 8;

public:
	class iterator : public std::forward_iterator_tag
	{
	public:
		using difference_type = size_t;
		using value_type = value_t;

	protected:
		node* n = nullptr;

	public:
		iterator() = default;
		iterator( node* n ) : n(n) {}

		iterator& operator++()
		{
			do n = n->next.load(std::memory_order::acquire);
			while (n->r_hash & 1 && n->r_hash != std::numeric_limits<size_t>::max());
			return *this;
		}

		iterator operator++(int)
		{
			iterator tmp = *this;
			operator++();
			return tmp;
		}

		bool operator==( const iterator& other ) const { return n == other.n; }
		bool operator!=( const iterator& other ) const { return n != other.n; }
		value_type& operator*() const { return n->value(); }
	};

public:
	constexpr sol_hash_set() = default;

	std::pair<iterator, bool> insert( const value_type& value ) { return insert_value(value); }
	std::pair<iterator, bool> insert( value_type&& value ) { return insert_value(std::move(value)); }

	template< typename key_t > requires requires { typename hash_t::is_transparent; typename compare_t::is_transparent; }
	std::pair<iterator, bool> insert( key_t&& key ) { return insert_value(std::forward<key_t>(key)); }

	iterator find( const value_type& value ) { return find_value(value); }

	template< typename key_t > requires requires { typename hash_t::is_transparent; typename compare_t::is_transparent; }
	iterator find( const key_t& key ) { return find_value(key); }

	iterator begin() { return ++iterator(&_head); }
	iterator end() { return iterator(&_tail); }

	size_t size() const { return _count.load(std::memory_order::relaxed); }
	bool empty() const { return size() == 0; }

	bool check_integrity() const
	{
		const node* next = nullptr;
		for (const node* n = &_head; n != &_tail; n = n->next.load(std::memory_order::relaxed))
		{
			next = n->next.load(std::memory_order::relaxed);
			if (n->r_hash > next->r_hash) return false;
		}
		return true;
	}

protected:
	using table_allocator = typename std::allocator_traits<allocator_t>::template rebind_alloc<std::byte>;
	using node_allocator = typename std::allocator_traits<allocator_t>::template rebind_alloc<value_node>;

	struct node_del
	{
		node_allocator* al;
		void operator()( value_node* n ) const { n->~value_node(), al->deallocate(n, 1); }
	};

	struct table_del
	{
		table_allocator* al;
		void operator()( table* tbl ) const
		{
			size_t bytes = table::bytes(tbl->size());
			tbl->~table(), al->deallocate((std::byte*)tbl, bytes);
		}
	};
	
protected:
	static void search( node*& prev, node*& next, size_t r_hash )
	{
		while (next->r_hash < r_hash)
		{
			prev = next;
			next = prev->next.load(std::memory_order::acquire);
		}
	}

	template< typename type >
	std::pair<iterator, bool> insert_value( type&& value )
	{
		using namespace std;

		size_t hash = hash_t()(value);
		size_t r_hash = bit_reverse(hash) & ~1;

		table* tbl = _table.load(memory_order::acquire);
		if (!tbl || _count.load(memory_order::relaxed) >= tbl->capacity)
			tbl = expand(tbl);

		node* n0 = get_dummy(tbl->buckets(), hash & tbl->mask);
		node* n1 = n0->next.load(memory_order::acquire);
		unique_ptr<value_node, node_del> new_n;

		for (;;)
		{
			search(n0, n1, r_hash);
			if (n1->r_hash == r_hash && (!new_n && compare()(n1->value(), value)) || (new_n && compare()(n1->value(), new_n->value)))
				return {n1, false};

			if (!new_n) new_n = make_value(r_hash, forward<type>(value));
			new_n->next.store(n1, memory_order::relaxed);
			if (n0->next.compare_exchange_strong(n1, new_n.get(), memory_order::release, memory_order::acquire))
			{
				_count.fetch_add(1, memory_order::relaxed);
				return {new_n.release(), true};
			}
		}
	}

	template< typename type >
	iterator find_value( const type& key )
	{
		using namespace std;

		size_t hash = hash_t()(key);
		size_t r_hash = bit_reverse(hash) & ~1;
		
		table* tbl = _table.load(memory_order::acquire);
		if (!tbl) return end();

		node* n0 = get_dummy(tbl->buckets(), hash & tbl->mask);
		node* n1 = n0->next.load(memory_order::acquire);
						
		search(n0, n1, r_hash);
		if (n1->r_hash == r_hash && compare()(n1->value(), key)) 
			return n1;

		return end();
	}

	node* get_dummy( std::atomic<node*>* list, size_t idx )
	{
		using namespace std;

		node* n = list[idx].load(memory_order::acquire);
		if (n) [[likely]] return n;

		int parent_order = numeric_limits<size_t>::digits - 1 - countl_zero(idx);
		n = get_dummy(list, idx ^ (1ull << parent_order));
		node* next = n->next.load(memory_order::acquire);
		size_t r_hash = bit_reverse(idx) | 1;
		unique_ptr<value_node, node_del> dummy;

		for (;;)
		{
			search(n, next, r_hash);
			if (next->r_hash == r_hash) return next;
			if (!dummy) dummy = make_dummy(r_hash);
			dummy->next.store(next, memory_order::relaxed);
			if (n->next.compare_exchange_strong(next, dummy.get(), memory_order::release, memory_order::acquire))
			{
				list[idx].store(dummy.get(), memory_order::release);
				return dummy.release();
			}
		}
	}

	table* expand( table* old )
	{
		using namespace std;

		unique_ptr<table, table_del> new_tbl = make_table(old ? old->size() * 2 : minimal_size);
		atomic<node*>* dst = new_tbl->buckets();
		dst[0].store(&_head, memory_order::relaxed);

		if (old)
		{
			for (atomic<node*> *src = old->buckets() + 1, *end = src + old->mask; src != end; ++src)
				(++dst)->store(src->load(memory_order::relaxed), memory_order::relaxed);
		}

		// waiting for c++26 hazard pointer for memory reclamation. should be relatively simple to integrate
		bool success = _table.compare_exchange_strong(old, new_tbl.get(), memory_order::release, memory_order::acquire);
		return success ? new_tbl.release() : old;
	}

	template< typename type >
	std::unique_ptr<value_node, node_del> make_value( size_t r_hash, type&& value )
	{
		value_node* n = new (node_al.allocate(1)) value_node{r_hash};
		new (&n->value) value_type(std::forward<type>(value));
		return {n, {&node_al}};
	}

	std::unique_ptr<value_node, node_del> make_dummy( size_t r_hash )
	{
		return {new (node_al.allocate(1)) value_node{r_hash}, {&node_al}};
	}

	std::unique_ptr<table, table_del> make_table( size_t size )
	{
		table* tbl = (table*)table_al.allocate(table::bytes(size));
		new (tbl) table{size - 1, (size * load_factor * 10) >> 10};
		std::for_each_n(tbl->buckets(), size, []( auto& buk ){ new (&buk) std::atomic<node*>{nullptr}; });
		return {tbl, {&table_al}};
	}

protected:
	[[no_unique_address]] table_allocator table_al;
	[[no_unique_address]] node_allocator node_al;

	std::atomic<table*> _table{nullptr};
	std::atomic_size_t _count = 0;

	node _head{0, &_tail};
	node _tail{std::numeric_limits<size_t>::max(), nullptr};
};