#pragma once

#ifdef _WIN32
	inline constexpr bool windows = true;
#else
	inline constexpr bool windows = false;
#endif

inline constexpr int fps = 60;
inline constexpr Float frame_time = 1.f / fps;
