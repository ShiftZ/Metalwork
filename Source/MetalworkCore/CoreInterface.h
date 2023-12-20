#pragma once

#include "Name.h"

template<typename Type>
class IFuture
{
public:
	virtual ~IFuture() = default;
	virtual bool IsRead() const = 0;
	virtual unique_ptr<Type> Get() = 0;
};

struct RemotePlayer
{
	int player;
	string ip;
	struct { int tcp, udp; } port;
};

class METALWORKCORE_API INetwork
{
public:
	virtual ~INetwork() = default;
	static unique_ptr<IFuture<INetwork>> Connect(span<RemotePlayer> targets, int player);
};

class METALWORKCORE_API IMetalCore
{
public:
	virtual void Ready() = 0;
	virtual ~IMetalCore() = default;
};

struct METALWORKCORE_API DisplayLog : log_category
{
	int key;
	float time = 10;
	DisplayLog(int key = -1) : key(key) {}
};

struct METALWORKCORE_API NetworkLog : log_category
{
	int player;
	NetworkLog(int player = -1) : player(player) {}
};

struct Color
{
	float r, g, b, a;

	Color() = default;
	Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

	template< typename Type > requires requires (Type col) { {col.r + col.g + col.b + col.a} -> std::convertible_to<float>; }
	Color(const Type& col) : r(col.r), g(col.g), b(col.b), a(col.a) {}
};

class IDebugDrawer
{
public:
	virtual void Circle(vec2 center, float radius, Color color) = 0;
	virtual void SolidCircle(vec2 center, vec2 axis, float radius, Color color) = 0;
	virtual void Poly(span<vec2> verts, Color color) = 0;
	virtual void SolidPoly(span<vec2> verts, Color color) = 0;
	virtual void Line(vec2 a, vec2 b, Color color) = 0;
	virtual void Point(vec2 p, float size, Color color) = 0;
	virtual void Edge(vec2 a, vec2 b) = 0;
	virtual ~IDebugDrawer() = default;
};

namespace Json { class Value; }

METALWORKCORE_API void SetContentPath(path dir);
METALWORKCORE_API Json::Value& GetJson(Name name);
METALWORKCORE_API Json::Value& MakeJson(string_view json_str);
