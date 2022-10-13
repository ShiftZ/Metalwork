#pragma warning(disable:4456) // declaration hides previous local declaration

#include "MetalGameModeBase.h"

#include "CoreEngine.h"
#include "VehicleActor.h"
#include "Network.h"

using namespace std;

DEFINE_LOG_CATEGORY(LogNetwork);

void AMetalworkArenaGameModeBase::InitGame(const FString& map_name, const FString& options, FString& error)
{
	AGameModeBase::InitGame(map_name, options, error);

	int player = 0;
	FParse::Value(FCommandLine::Get(), L"player", player);

	int target_port = 22150 + (player + 1) % 2;

	Network::Target target;
	target.player = (player + 1) % 2;
	target.ip = address::from_string("127.0.0.1");
	target.port = {target_port, target_port};

	Network::Logger logger;

	if (player % 2 == 1)
	{
		logger = {[=](string text){ UE_LOG(LogNetwork, Log, L"%s", ANSI_TO_TCHAR(text.c_str())); },
				  [=](string text){ UE_LOG(LogNetwork, Error, L"%s", ANSI_TO_TCHAR(text.c_str())); }};
	}

	xfuture<unique_ptr<Network>> connection = Network::Connect({target}, player, logger);

	unique_ptr<Network> network;
	try { network = connection.get(); }
	catch (exception& e)
	{
		string err = e.what();
		DebugBreak();
	}
	
	core = make_unique<MetalCore>(player, move(network));
	arena = make_unique<Arena>(0.02);

	for (shared_ptr<Vehicle>& vehicle : arena->vehicles)
	{
		AVehicleActor* actor = GetWorld()->SpawnActor<AVehicleActor>();
		actor->AttachToObject(vehicle->body.get());

		if (vehicle->weapon)
		{
			AWeaponActor* actor = GetWorld()->SpawnActor<AWeaponActor>();
			actor->AttachToObject(vehicle->weapon->body.get());
		}
	}

	core->Ready();
	//core->Start();
}

void AMetalworkArenaGameModeBase::Tick(float dt)
{
	Super::Tick(dt);
	//core->Step();
}
