#pragma once

#include "vectorx.h"

template< typename type >
struct vectorx<type, 3> : super_vector<type, 3>
{
	using super_vector<type, 3>::x;
	using super_vector<type, 3>::y;
	using super_vector<type, 3>::z;

	using super_vector<type, 3>::super_vector;

	vectorx cross( const vectorx& vec ) const
	{
		return vectorx(y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x);
	}

	template< typename init_type >
	void operator=( std::initializer_list<init_type> list ) { super_vector<type, 3>::operator=(move(list)); }
};
