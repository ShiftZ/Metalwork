#pragma once

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

template< typename type >
using key = value<type, 0>;