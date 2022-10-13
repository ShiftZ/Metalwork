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

template< typename type, int len >
struct super_vectorx
{
	union
	{
		type v[len];
		type x;
	};
};

template< typename type >
struct super_vectorx<type, 2>
{
	union
	{
		struct { type x, y; };
		struct { type width, height; };
		type v[2];
	};

	super_vectorx() = default;
	super_vectorx( null_vector ) { y = x = 0; }

	super_vectorx( type x, type y ) : x(x), y(y) {}

	template< typename unknown_t > requires requires (unknown_t v, type s) { v.x = v.y = s; }
	super_vectorx( const unknown_t& v ) : x(v.x), y(v.y) {}

	void set( type x, type y ) { this->x = x, this->y = y; }
	void operator=( null_vector ) { y = x = 0; }

	template< typename unknown_t > requires (requires (unknown_t v, type s) { v.x = v.y = s, unknown_t{s, s}; } || 
											 requires (unknown_t v, type s) { v.X = v.Y = s, unknown_t{s, s}; })
	operator unknown_t() const { return unknown_t{x, y}; }
};

template< typename type >
struct super_vectorx<type, 3>
{
	union
	{
		struct { type x, y, z; };
		type v[3];
	};

	super_vectorx() = default;
	super_vectorx( null_vector ) { z = y = x = 0; }

	super_vectorx( type x, type y, type z ) : x(x), y(y), z(z) {}

	template< typename unknown_t > requires requires (unknown_t v, type s) { v.x = v.y = v.z = s; }
	super_vectorx( const unknown_t& v ) : x(v.x), y(v.y), z(v.z) {}

	void set( type x, type y, type z ) { this->x = x, this->y = y, this->z = z; }
	void operator=( null_vector ) { z = y = x = 0; }

	template< typename unknown_t > requires (requires (unknown_t v, type s) { v.x = v.y = v.z = s, unknown_t{s, s, s}; } ||
											 requires (unknown_t v, type s) { v.X = v.Y = v.Z = s, unknown_t{s, s, s}; })
	operator unknown_t() const { return unknown_t{x, y, z}; }
};

template< typename type >
struct super_vectorx<type, 4>
{
	union
	{
		struct { type x, y, z, w; };
		struct { type r, g, b, a; };
		type v[4];
	};

	super_vectorx() = default;
	super_vectorx( null_vector ) { w = z = y = x = 0; }

	template<typename x_type, typename y_type, typename z_type, typename w_type>
	super_vectorx( x_type x, y_type y, z_type z, w_type w ) : x(x), y(y), z(z), w(w) {}

	void set( type x, type y, type z, type w ) { this->x = x, this->y = y, this->z = z, this->w = w; }
	void operator=( null_vector ) { w = z = y = x = 0; }

	template< typename unknown_t > requires requires (unknown_t v, type s) { v.x = v.y = v.z = v.w = s, unknown_t{s, s, s, s}; }
	operator unknown_t() const { return unknown_t{x, y, z, w}; }
};

template< typename type, int len >
struct super_vector : super_vectorx<type, len>
{
	using super_vectorx<type, len>::v;
	using super_vectorx<type, len>::super_vectorx;

	super_vector() = default;

	template< typename src_type, int srclen > requires std::is_floating_point_v<type> || !std::is_floating_point_v<src_type>
	super_vector( const super_vectorx<src_type, srclen>& src )
	{
		int a = srclen;
		int b = len;
		int m = std::min(srclen, len);
		for (int i = 0; i < std::min(srclen, len); i++)
			v[i] = type(src.v[i]);
	}

	template< typename src_type, int srclen >
	explicit super_vector( const super_vector<src_type, srclen>& src )
	{
		for (int i = 0; i < std::min(srclen, len); i++)
			v[i] = type(src.v[i]);

		for (int i = srclen; i < len; i++)
			v[i] = 0;
	}

	super_vector( type value ) { for (type& val : v) val = value; }

	type dot( const super_vector& other ) const
	{
		type result = v[0] * other.v[0];
		for (int i = 1; i < len; i++)
			result += v[i] * other.v[i];
		return result;
	}

	type sqlen() const
	{
		type result = v[0] * v[0];
		for (int i = 1; i < len; i++)
			result += v[i] * v[i];
		return result;
	}

	type length() const { return sqrt(sqlen()); }

	type normalize()
	{
		type l = length();
		type k = 1 / l;
		for (type& val : v) val *= k;
		return l;
	}

	vectorx<type, len> normalized() const
	{
		vectorx<type, len> n;
		type k = 1 / length();
		for (int i = 0; i < len; i++)
			n.v[i] = v[i] * k;
		return n;
	}

	void lerp( const super_vector& other, type t )
	{
		type rt = 1 - t;
		for (int i = 0; i < len; i++)
			v[i] = v[i] * rt + other.v[i] * t;
	}

	void project( const super_vector& vec )
	{
		float k = dot(vec) / vec.sqlen();
		for (int i = 0; i < len; i++)
			v[i] = vec.v[i] * k;
	}

	void operator=( type value )
	{
		for (type& val : v) val = value;
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
		for (int i = 0; i < len; i++)
			v[i] += other.v[i];
	}

	void operator-=( const super_vector& other )
	{
		for (int i = 0; i < len; i++)
			v[i] -= other.v[i];
	}

	void operator*=( const super_vector& other )
	{
		for (int i = 0; i < len; i++)
			v[i] *= other.v[i];
	}

	void operator/=( const super_vector& other )
	{
		for (int i = 0; i < len; i++)
			v[i] /= other.v[i];
	}

	type& operator[]( int idx ) { return v[idx]; }
	const type& operator[]( int idx ) const { return v[idx]; }

	vectorx<type, len> operator-() const
	{
		vectorx<type, len> result;
		for (int i = 0; i < len; i++)
			result[i] = -v[i];
		return result;
	}

	bool operator<( type radius ) const { return sqlen() < radius * radius; }
	bool operator<=( type radius ) const { return sqlen() <= radius * radius; }
	bool operator>( type radius ) const { return sqlen() > radius * radius; }
	bool operator>=( type radius ) const { return sqlen() > radius * radius; }

	bool operator==( type value ) const
	{
		bool result = v[0] == value;
		for (int i = 1; i < len; i++)
			result &= v[i] == value;
		return result;
	}

	bool operator!=( type value ) const
	{
		bool result = v[0] != value;
		for (int i = 1; i < len; i++)
			result |= v[i] != value;
		return result;
	}

	bool operator==( null_vector ) const
	{
		bool result = v[0] == 0;
		for (int i = 1; i < len; i++)
			result &= v[i] == 0;
		return result;
	}

	bool operator!=( null_vector ) const
	{
		bool result = v[0] != 0;
		for (int i = 1; i < len; i++)
			result |= v[i] != 0;
		return result;
	}

	template< int dstlen > requires (len >= dstlen)
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
