#pragma once

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

	static unique_ptr<IMetalCore> Make(int player, unique_ptr<INetwork> net);
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

class IDebugDrawer
{
public:
	virtual void Circle(vec2 center, float radius) = 0;
	virtual void Poly(span<vec2> verts) = 0;
	virtual void Edge(vec2 A, vec2 B) = 0;
};