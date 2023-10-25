#pragma once

#include <stdint.h>
#include <cstdarg>
#include <unordered_map>
#include <unordered_set>

/// You can use this to change the length scale used by your game.
/// For example for inches you could use 39.4.
#define b2_lengthUnitsPerMeter 1.0f

/// The maximum number of vertices on a convex polygon. You cannot increase
/// this too much because b2BlockAllocator has a maximum object size.
#define b2_maxPolygonVertices	8

// User data

/// You can define this to inject whatever data you want in b2Body
using b2BodyUserData = class B2Body*;

/// You can define this to inject whatever data you want in b2Fixture
struct B2_API b2FixtureUserData
{
	b2FixtureUserData()
	{
		pointer = 0;
	}

	/// For legacy compatibility
	uintptr_t pointer;
};

/// You can define this to inject whatever data you want in b2Joint
struct B2_API b2JointUserData
{
	b2JointUserData()
	{
		pointer = 0;
	}

	/// For legacy compatibility
	uintptr_t pointer;
};

// Memory Allocation

/// Implement this function to use your own memory allocator.
void* b2Alloc(int size);

/// If you implement b2Alloc, you should also implement this function.
void b2Free(void* mem);

/// Default logging function
B2_API void b2Log_Default(const char* string, va_list args);

/// Implement this to use your own logging.
inline void b2Log(const char* string, ...)
{
	va_list args;
	va_start(args, string);
	b2Log_Default(string, args);
	va_end(args);
}

#ifndef box2d_EXPORTS
inline B2_API std::unordered_set<void*> b2releases;
//inline B2_API std::unordered_map<void*, int> b2allocs;

#else
inline B2_API std::unordered_map<void*, int> b2allocs;
inline B2_API std::unordered_set<void*> b2releases;
inline void b2Free(void* mem)
{
	auto alloc = b2allocs.find(mem);
	if (alloc != b2allocs.end())
	{
		b2allocs.erase(alloc);
		operator delete(mem);
	}
	else
		b2releases.insert(mem);
}

inline void* b2Alloc(int size)
{
	void* ptr = operator new(size);
	b2allocs[ptr] = size;
	return ptr;
}
#endif




