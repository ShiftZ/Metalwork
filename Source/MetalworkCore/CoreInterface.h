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

struct METALWORKCORE_API DisplayLog
{
	int key = -1;
	float time = 10;
	operator size_t() const { return std::type_index(typeid(DisplayLog)).hash_code(); }
};

METALWORKCORE_API void add_logger( const DisplayLog&, std::function<void(std::string)>&& );