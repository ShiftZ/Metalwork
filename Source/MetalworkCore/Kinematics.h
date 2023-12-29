#pragma once

#include <cstdlib>
#include <limits>
#include <algorithm>
#include <tuple>

// done, shift, accel
inline std::tuple<bool, float, float> KinematicMove( float dist, float speed, float dt, float accel, float deccel, float max_speed = std::numeric_limits<float>::max(), float eps = 0.0001 )
{
	auto sign = []( float value ) { return float((0 <= value) - (value < 0)); };

	auto brake = [&]( float to_speed )
	{
		float brake_time = (abs(speed) - to_speed) / deccel;
		float move_time = std::min(brake_time, dt);
		float next_speed = speed - deccel * move_time * sign(speed);
		dist -= 0.5 * (speed + next_speed) * move_time;
		speed = next_speed;
		dt -= move_time;
	};

	float idist = dist;
	float ispeed = speed;

	if (speed == 0 && abs(dist) < eps) return std::make_tuple(true, dist, speed);
	if (dist * speed < 0)
	{
		brake(0);
		if (dt == 0) return std::make_tuple(false, dist - idist, speed - ispeed);
	}

	if (abs(speed) > max_speed)
	{
		brake(max_speed);
		if (dt == 0) return std::make_tuple(false, dist - idist, speed - ispeed);
	}

	float peak = (abs(dist) * accel + speed * speed * 0.5) / (accel + deccel);
	float top_speed = sqrt(2 * deccel * peak);
	top_speed = std::min(top_speed, max_speed);

	if (abs(speed) < top_speed)
	{
		float time_to_peak = (top_speed - abs(speed)) / accel;
		float move_time = std::min(time_to_peak, dt);
		float next_speed = speed + accel * move_time * sign(dist);
		dist -= 0.5 * (speed + next_speed) * move_time;
		speed = next_speed;
		dt -= move_time;
	}

	if (dt == 0) return std::make_tuple(false, dist - idist, speed - ispeed);

	float brake_dist = (speed * speed) / (2 * deccel);

	if (abs(dist) > brake_dist)
	{
		float move_dist = abs(dist) - brake_dist;
		float move_time = std::min(move_dist / abs(speed), dt);
		dist -= speed * move_time;
		dt -= move_time;
	}

	if (dt == 0) return std::make_tuple(false, dist - idist, speed - ispeed);

	brake(0);

	return std::make_tuple(dt != 0, dist - idist, speed - ispeed);
}