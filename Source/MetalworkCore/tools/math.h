#pragma once

#include <cmath>
#include <concepts>
#include <numbers>

constexpr auto sqr( auto value ) { return value * value; }

template< std::floating_point type >
constexpr auto sigmoid( type x, std::type_identity_t<type> midpoint = 1 )
{
	return std::atan(x / midpoint) * (2*std::numbers::inv_pi_v<type>);
}

template< std::floating_point float_t >
constexpr float_t sigmoid( float_t x,
		std::type_identity_t<float_t> ceiling,
		std::type_identity_t<float_t> floor,
		std::type_identity_t<float_t> midpoint = 1 )
{
	using namespace std::numbers;
	float_t range = ceiling - floor;
	float_t offset = 1 / std::tan(pi_v<float_t> * floor / range);
	return (ceiling + floor) / 2 + std::atan(x / midpoint + offset) * range * inv_pi_v<float_t>;
}