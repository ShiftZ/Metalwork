#pragma once

#include "vectorx.h"

template< typename type >
struct vectorx<type, 2> : super_vector<type, 2>
{
	using super_vector<type, 2>::x;
	using super_vector<type, 2>::y;
	using super_vector<type, 2>::super_vector;

	type cross( const vectorx& vec ) const { return x * vec.y - y * vec.x; }

	vectorx rotation90_ccw() const { return vectorx(-y, x); }
	vectorx rotation90_cw() const { return vectorx(y, -x); }
	vectorx reflection_v() const { return vectorx(-x, y); }
	vectorx reflection_h() const { return vectorx(x, -y); }
	vectorx swapped() const { return vectorx(y, x); }

	vectorx& rotate90_ccw()
	{
		type tmp = x;
		x = -y, y = tmp;
		return *this;
	}

	vectorx& rotate90_cw()
	{
		type tmp = x;
		x = y, y = -tmp;
		return *this;
	}

	vectorx& reflect( const vectorx& mirror )
	{
		float k = 2 * this->dot(mirror) / mirror.sqlen();
		x = mirror.x * k - x;
		y = mirror.y * k - y;
		return *this;
	}

	vectorx& warp_out( const vectorx& yaxis )
	{
		type tx = x;
		x = x * yaxis.y + y * yaxis.x;
		y = y * yaxis.y - tx * yaxis.x;
		return *this;
	}

	vectorx& warp_in( const vectorx& yaxis )
	{
		type tx = x;
		x = x * yaxis.y - y * yaxis.x;
		y = y * yaxis.y + tx * yaxis.x;
		return *this;
	}

	vectorx& rotate( const vectorx& rot )
	{
		type tx = x;
		x = x * (type)rot.x - y * (type)rot.y;
		y = y * (type)rot.x + tx * (type)rot.y;
		return *this;
	}

	vectorx& rotate_cw( const vectorx& rot )
	{
		type tx = x;
		x = x * rot.x + y * rot.y;
		y = y * rot.x - tx * rot.y;
		return *this;
	}

	vectorx& reflect_v()
	{
		x = -x;
		return *this;
	}

	vectorx& reflect_h()
	{
		y = -y;
		return *this;
	}

	vectorx rotation( const vectorx& rot ) const
	{
		return vectorx(x * rot.x - y * rot.y, x * rot.y + y * rot.x);
	}

	vectorx rotation_cw( const vectorx& rot ) const
	{
		return vectorx(x * rot.x + y * rot.y, y * rot.x - x * rot.y);
	}

	void swap() { std::swap(x, y); }

	type angle() const requires std::is_floating_point_v<type> { return atan2(y, x); }

	static vectorx from_angle(type angle, type length) { return {cos(angle) * length, sin(angle) * length}; }
	static vectorx from_angle(type angle) { return {cos(angle), sin(angle)}; }

	/*template< typename init_type >
	void operator=( std::initializer_list<init_type> list ) { super_vector<type, 2>::operator=(delta(list)); }*/
};

template< typename type >
vector2<type> rotate90ccw( const vector2<type>& v ) { return vector2<type>(-v.y, v.x); }

template< typename type >
vector2<type> rotate90cw( const vector2<type>& v ) { return vector2<type>(v.y, -v.x); }

template< typename type >
vector2<type> reflect( const vector2<type>& v, const vector2<type>& from ) { return project(v, from) * 2 - v; }

template< typename type >
vector2<type> reflect_v( const vector2<type>& v ) { return vector2<type>(-v.x, v.y); }

template< typename type >
vector2<type> reflect_h( const vector2<type>& v ) { return vector2<type>(v.x, -v.y); }

template< typename type >
vector2<type> warp_out( const vector2<type>& v, const vector2<type>& yaxis )
{
	return vector2<type>(v.x * yaxis.y + v.y * yaxis.x, v.y * yaxis.y - v.x * yaxis.x);
}

template< typename type >
vector2<type> warp_in( const vector2<type>& v, const vector2<type>& yaxis )
{
	return vector2<type>(v.x * yaxis.y - v.y * yaxis.x, v.y * yaxis.y + v.x * yaxis.x);
}

template< typename type >
vector2<type> rotate( const vector2<type>& v, const vector2<type>& rot ) { return v.rotation(rot); }

template< typename type >
vector2<type> rotate_cw( const vector2<type>& v, const vector2<type>& rot ) { return v.rotation_cw(rot); }
