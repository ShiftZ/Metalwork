#pragma once

#include "vectorx.h"

template< typename type >
struct vectorx<type, 4> : public super_vector<type, 4>
{
	using super_vector<type, 4>::x;
	using super_vector<type, 4>::y;
	using super_vector<type, 4>::z;
	using super_vector<type, 4>::w;

	using super_vector<type, 4>::super_vector;
	
	template< typename init_type >
	void operator=( std::initializer_list<init_type> list ) { super_vector<type, 4>::operator=(move(list)); }
};
