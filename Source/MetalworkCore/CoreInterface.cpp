#include "CoreInterface.h"
#include "Network.h"
#include "CoreEngine.h"

unique_ptr<IFuture<INetwork>> INetwork::Connect(span<RemotePlayer> targets, int player)
{
	return make_unique<Task<INetwork>>(Network::Connect(targets, player));
}

unique_ptr<IMetalCore> IMetalCore::Make(int player, unique_ptr<INetwork> net)
{
	return make_unique<MetalCore>(player, unique_ptr<Network>((Network*)net.release()));
}
