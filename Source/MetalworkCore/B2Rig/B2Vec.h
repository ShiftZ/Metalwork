#pragma once

#include "box2d/b2_math.h"

using Vec2 = b2Vec2;
using Float = float;

inline Float Length(const Vec2& V) { return V.Length(); }
inline Float SqLen(const Vec2& V) { return V.LengthSquared(); }
inline Vec2 Rotated90CW(Vec2& V) { return {V.y, -V.x}; }
inline Float Angle(const Vec2& V) { return atan2(V.y, V.x); }

template< typename length_t = Float >
b2Vec2 Normalized( const Vec2& v, length_t&& len = Float() )
{
	len = Length(v);
	Float k = 1 / len;
	return {v.x * k, v.y * k};
}

inline b2Vec2 operator*( const Vec2& v, Float s ) { return {s * v.x, s * v.y}; }
inline bool operator!=( const Vec2& v, nullptr_t ) { return v.x != 0 || v.y != 0; }
inline bool operator==( const Vec2& v, nullptr_t ) { return v.x == 0 && v.y == 0; }