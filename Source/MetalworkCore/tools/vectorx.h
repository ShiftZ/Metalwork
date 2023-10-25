#pragma once

#include <algorithm>
#include <type_traits>

inline constexpr struct null_vector {} nullvec;

template< typename, int >
struct vectorx;

template< typename type  >
using vector2 = vectorx<type, 2>;

template< typename type >
using vector3 = vectorx<type, 3>;

template< typename type >
using vector4 = vectorx<type, 4>;

template< typename, int >
struct super_vectorx;

template< typename type, int dim >
void is_vectorx(const vectorx<type, dim>&);

template< typename type >
struct super_vectorx<type, 2>
{
	type x, y;

	super_vectorx() = default;
	super_vectorx( null_vector ) { y = x = 0; }

	super_vectorx( type x, type y ) : x(x), y(y) {}

	template< typename vec_t > requires requires (vec_t v, type s) { v.x = v.y = s; }
	super_vectorx( const vec_t& v ) : x(v.x), y(v.y) {}

	void set( type x, type y ) { this->x = x, this->y = y; }
	void operator=( null_vector ) { y = x = 0; }

	template< typename vec_t > requires (!requires (vec_t v) { is_vectorx(v); } &&
		(requires (vec_t v, type s) { v.x = v.y = s, vec_t{s, s}; } ||
		 requires (vec_t v, type s) { v.X = v.Y = s, vec_t{s, s}; }))
	operator vec_t() const { return vec_t{x, y}; }
};

template< typename type >
struct super_vectorx<type, 3>
{
	type x, y, z;

	super_vectorx() = default;
	super_vectorx( null_vector ) { z = y = x = 0; }
	super_vectorx( type x, type y, type z ) : x(x), y(y), z(z) {}

	template< typename unknown_t > requires requires (unknown_t v, type s) { v.x = v.y = v.z = s; }
	super_vectorx( const unknown_t& v ) : x(v.x), y(v.y), z(v.z) {}

	void set( type x, type y, type z ) { this->x = x, this->y = y, this->z = z; }
	void operator=( null_vector ) { z = y = x = 0; }

	template< typename vec_t > requires (requires (vec_t v, type s) { v.x = v.y = v.z = s, vec_t{s, s, s}; } ||
										 requires (vec_t v, type s) { v.X = v.Y = v.Z = s, vec_t{s, s, s}; })
	operator vec_t() const { return vec_t{x, y, z}; }
};

template< typename type >
struct super_vectorx<type, 4>
{
	type x, y, z, w;

	super_vectorx() = default;
	super_vectorx( null_vector ) { w = z = y = x = 0; }
	super_vectorx( type x, type y, type z, type w ) : x(x), y(y), z(z), w(w) {}

	void set( type x, type y, type z, type w ) { this->x = x, this->y = y, this->z = z, this->w = w; }
	void operator=( null_vector ) { w = z = y = x = 0; }

	template< typename vec_t > requires requires (vec_t v, type s) { v.x = v.y = v.z = v.w = s, vec_t{s, s, s, s}; }
	operator vec_t() const { return vec_t{x, y, z, w}; }
};

template< typename type, int dim >
struct super_vector : super_vectorx<type, dim>
{
	using super_vectorx<type, dim>::x;
	using super_vectorx<type, dim>::super_vectorx;

	super_vector() = default;

	template< typename src_type, int src_dim > requires (std::is_floating_point_v<type> || !std::is_floating_point_v<src_type>)
	super_vector( const super_vectorx<src_type, src_dim>& src )
	{
		for (int i = 0; i < std::min(src_dim, dim); ++i)
			at(i) = type(src.v[i]);
	}

	template< typename src_type, int srclen >
	explicit super_vector( const super_vector<src_type, srclen>& src )
	{
		for (int i = 0; i < std::min(srclen, dim); ++i)
			at(i) = type(src[i]);

		for (int i = srclen; i < dim; ++i)
			at(i) = 0;
	}

	super_vector( type value )
	{
		for (int i = 0; i < dim; ++i)
			at(i) = value;
	}

	type dot( const super_vector& other ) const
	{
		type result = at(0) * other[0];
		for (int i = 1; i < dim; i++)
			result += at(i) * other[i];
		return result;
	}

	type sqlen() const
	{
		type result = at[0] * at[0];
		for (int i = 1; i < dim; i++)
			result += at[i] * at[i];
		return result;
	}

	type length() const { return sqrt(sqlen()); }

	type normalize()
	{
		type l = length();
		type k = 1 / l;
		for (int i = 0; i < dim; ++i)
			at(i) *= k;
		return l;
	}

	vectorx<type, dim> normalized() const
	{
		vectorx<type, dim> n;
		type k = 1 / length();
		for (int i = 0; i < dim; i++)
			n.v[i] = at(i) * k;
		return n;
	}

	void lerp( const super_vector& other, type t )
	{
		type rt = 1 - t;
		for (int i = 0; i < dim; i++)
			at(i) = at(i) * rt + other[i] * t;
	}

	void project( const super_vector& vec )
	{
		float k = dot(vec) / vec.sqlen();
		for (int i = 0; i < dim; i++)
			at(i) = vec[i] * k;
	}

	auto& at( this auto& self, int idx ) { return *(&self.x + idx); }

	void operator=( type value )
	{
		for (int i = 0; i < dim; ++i)
			at(i) = value;
	}

	/*template<typename init_type>
	void operator=( std::initializer_list<init_type> list )
	{
		assert(list.size() == len);

		auto it = list.begin();
		for (type& val : v)
		{
			val = type(*it);
			++it;
		}
	}*/

	void operator+=( const super_vector& other )
	{
		for (int i = 0; i < dim; i++)
			at(i) += other[i];
	}

	void operator-=( const super_vector& other )
	{
		for (int i = 0; i < dim; i++)
			at(i) -= other[i];
	}

	void operator*=( const super_vector& other )
	{
		for (int i = 0; i < dim; i++)
			at(i) *= other[i];
	}

	void operator/=( const super_vector& other )
	{
		for (int i = 0; i < dim; i++)
			at(i) /= other[i];
	}

	auto& operator[]( this auto& self, int idx ) { return *(&self.x + idx); }

	vectorx<type, dim> operator-() const
	{
		vectorx<type, dim> result;
		for (int i = 0; i < dim; i++)
			result[i] = -at(i);
		return result;
	}

	int operator<=>( type radius ) const { return sqlen() <=> radius * radius; }

	bool operator==( type value ) const
	{
		bool result = (at(0) == value);
		for (int i = 1; i < dim; i++)
			result &= (at(i) == value);
		return result;
	}

	bool operator!=( type value ) const
	{
		bool result = (at(0) != value);
		for (int i = 1; i < dim; i++)
			result |= (at(i) != value);
		return result;
	}

	bool operator==( null_vector ) const
	{
		bool result = at(0) == 0;
		for (int i = 1; i < dim; i++)
			result &= at(i) == 0;
		return result;
	}

	bool operator!=( null_vector ) const
	{
		bool result = at(0) != 0;
		for (int i = 1; i < dim; i++)
			result |= at(i) != 0;
		return result;
	}

	template< int dstlen > requires (dim >= dstlen)
	operator const vectorx<type, dstlen>&() const { return *(const vectorx<type, dstlen>*)this; }

	/*template< int dstlen > requires (len >= dstlen)
	operator vectorx<type, dstlen>&() const { return *(vectorx<type, dstlen>*)this; }*/
};

template< typename a_type, typename b_type, int len >
auto operator+( const vectorx<a_type, len>& a, const vectorx<b_type, len>& b )
{
	vectorx<std::common_type_t<a_type, b_type>, len> result;
	for (int i = 0; i < len; i++)
		result[i] = a[i] + b[i];
	return result;
}

template< typename a_type, typename b_type, int len >
auto operator-( const vectorx<a_type, len>& a, const vectorx<b_type, len>& b )
{
	vectorx<std::common_type_t<a_type, b_type>, len> result;
	for (int i = 0; i < len; i++)
		result[i] = a[i] - b[i];
	return result;
}

template< typename type, int len, typename scalar_type > requires std::is_arithmetic_v<scalar_type>
vectorx<type, len> operator*( const vectorx<type, len>& v, scalar_type scalar )
{
	vectorx<type, len> result;
	for (int i = 0; i < len; i++)
		result[i] = v[i] * scalar;
	return result;
}

template< typename type, int len, typename scalar_type > requires std::is_arithmetic_v<scalar_type>
vectorx<type, len> operator/( const vectorx<type, len>& v, scalar_type scalar )
{
	vectorx<type, len> result;
	for (int i = 0; i < len; i++)
		result[i] = v[i] / scalar;
	return result;
}

template< typename a_type, typename b_type, int len >
auto operator*( const vectorx<a_type, len>& a, const vectorx<b_type, len>& b )
{
	vectorx<std::common_type_t<a_type, b_type>, len> result;
	for (int i = 0; i < len; i++)
		result[i] = a[i] * b[i];
	return result;
}

template< typename a_type, typename b_type, int len >
auto operator/( const vectorx<a_type, len>& a, const vectorx<b_type, len>& b )
{
	vectorx<std::common_type_t<a_type, b_type>, len> result;
	for (int i = 0; i < len; i++)
		result[i] = a[i] / b[i];
	return result;
}

template< typename a_type, typename b_type, int len >
bool operator!=( const vectorx<a_type, len>& a, const vectorx<b_type, len>& b )
{
	bool result = a.x != b.x;
	for (int i = 1; i < len; i++)
		result |= a[i] != b[i];
	return result;
}

template< typename a_type, typename b_type, int len >
bool operator==( const vectorx<a_type, len>& a, const vectorx<b_type, len>& b )
{
	bool result = a.x == b.x;
	for (int i = 1; i < len; i++)
		result &= a[i] == b[i];
	return result;
}

template< typename type, int len >
type operator^( const vectorx<type, len>& a, const vectorx<type, len>& b ) { return a.dot(b); }

template< typename type, int len >
auto operator%( const vectorx<type, len>& a, const vectorx<type, len>& b ) { return a.cross(b); }

template< typename type, int len >
vectorx<type, len> normalize( const vectorx<type, len>& v )
{
	vectorx<type, len> result;
	type k = 1 / v.length();
	for (int i = 0; i < len; i++)
		result[i] = v[i] * k;
	return result;
}

template< typename type, int len >
vectorx<type, len> lerp( const vectorx<type, len>& a, const vectorx<type, len>& b, type tau )
{
	vectorx<type, len> result;
	type rt = 1 - tau;
	for (int i = 0; i < len; i++)
		result[i] = a[i] * rt + b[i] * tau;
	return result;
}

template< typename type, int len >
type dot( const vectorx<type, len>& a, const vectorx<type, len>& b ) { return a.dot(b); }

template< typename type, int len >
auto cross( const vectorx<type, len>& a, const vectorx<type, len>& b ) { return a.cross(b); }

template< typename type, int len >
type sqlen( const vectorx<type, len>& a, const vectorx<type, len>& b )
{
	type result = sqr(a.v[0] - b.v[0]);
	for (int i = 1; i < len; i++)
		result += sqr(a.v[i] - b.v[i]);
	return result;
}

template< typename type, int len >
type sqlen( const vectorx<type, len>& v ) { return v.sqlen(); }

template< typename type, int len >
type length( const vectorx<type, len>& a, const vectorx<type, len>& b ) { return sqrt(sqlen(a, b)); }

template< typename type, int len >
type length( const vectorx<type, len>& v ) { return v.length(); }

template< typename type, int len >
vectorx<type, len> project( const vectorx<type, len>& what, const vectorx<type, len>& on )
{
   return on * (dot(what, on) / on.sqlen());
}

template< typename type, int len >
vectorx<type, len> project( const vectorx<type, len>& v, const vectorx<type, len>& a, vectorx<type, len> b )
{
	b -= a;
	return b * (dot(v - a, b) / b.sqlen()) + a;
}

template< typename type, int len >
vectorx<type, len> direction( const vectorx<type, len>& from, const vectorx<type, len>& to )
{
	return normalize(to - from);
}

template< typename type, int len >
type projection_tau( const vectorx<type, len>& of, const vectorx<type, len>& a, vectorx<type, len> b )
{
	b -= a;
	return dot(of - a, b) / b.sqlen();
}

using vec2 = vectorx<float, 2>;
using vec2d = vectorx<double, 2>;
using vec2i = vectorx<int, 2>;
using vec2f = vectorx<float, 2>;

using vec3 = vectorx<float, 3>;
using vec3d = vectorx<double, 3>;
using vec3i = vectorx<int, 3>;
using vec3f = vectorx<float, 3>;

using vec4 = vectorx<float, 4>;
using vec4d = vectorx<double, 4>;
using vec4i = vectorx<int, 4>;
using vec4f = vectorx<float, 4>;
