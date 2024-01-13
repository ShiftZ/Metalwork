#pragma once

#include "box2d/b2_math.h"

using Vec2 = b2Vec2;
using Float = float; 

inline Float length( const Vec2& v ) { return v.Length(); }
inline Float sqlen( const Vec2& v ) { return v.LengthSquared(); }
inline Vec2 rotated90cw( Vec2& v ) { return {v.y, -v.x}; }
inline Float angle( const Vec2& v ) { return atan2(v.y, v.x); }

template< typename length_t = Float >
b2Vec2 normalized( const Vec2& v, length_t&& len = Float() )
{
	len = length(v);
	Float k = 1 / len;
	return {v.x * k, v.y * k};
}

inline b2Vec2 operator*( const Vec2& v, Float s ) { return {s * v.x, s * v.y}; }
inline bool operator!=( const Vec2& v, nullptr_t ) { return v.x != 0 || v.y != 0; }
inline bool operator==( const Vec2& v, nullptr_t ) { return v.x == 0 && v.y == 0; }