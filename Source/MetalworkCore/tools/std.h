#pragma once

#include <tuple>
#include <memory>
#include <ranges>

template< typename type, int n = 1 >
struct value
{
	type& val;

	template< typename tuple_t > requires requires (tuple_t& t){ std::get<n>(t); }
	value( tuple_t& t ) : val(std::get<n>(t)) {}

	type* operator->() const { return &val; }
	type& operator*() const { return val; }
	operator type&() const { return val; }
};

template< typename tuple_t >
value( tuple_t& ) -> value<std::tuple_element_t<1, tuple_t>, 1>;

template< typename type >
using key = value<type, 0>;

template< typename type, typename base >
std::unique_ptr<type> static_pointer_cast( std::unique_ptr<base> p ) noexcept
{
	return std::unique_ptr<type>(static_cast<type*>(p.release()));
}

inline auto drop_nth( size_t n )
{
    return std::views::filter([count = 0u, n](auto&&) mutable { return count++ != n; });
}

/*namespace std::ranges::views
{
	template< range range_t >
	class adjacent_view : view_interface<adjacent_view<range_t>>
	{
		using base_iterator = iterator_t<range_t>;
		using base_value = range_value_t<range_t>;

		class iterator : public forward_iterator_tag
		{
		public:
			using difference_type = typename base_iterator::difference_type;
			using value_type = pair<base_value&, base_value&>;

		private:
			base_iterator first, second;

		public:
			iterator() = default;
			iterator( base_iterator it, base_iterator last ) : first(it), second(it) { if (it != last) ++second; }

			iterator& operator++() { first = second, ++second; return *this; }
			iterator operator++(int) { unreachable(); }
			bool operator==( const iterator& other ) const { return second == other.second; }
			bool operator!=( const iterator& other ) const { return second != other.second; }
			value_type operator*() const { return {*first, *second}; }
		};

	private:
		range_t base;
		base_iterator first, last;

	public:
		adjacent_view( range_t r ) : base(r), first(r.begin()), last(r.end()) {}
		
		iterator begin() { return {first, last}; }
		iterator end() { return {last, last}; }
	};

	template< range range_t >
	adjacent_view( range_t&& r ) -> adjacent_view<all_t<range_t>>;

	constexpr struct adjacent_view_adapter {} adjacent;

	template< typename range_t >
	auto operator|( range_t&& r, adjacent_view_adapter ) { return adjacent_view(forward<range_t>(r)); }
}*/