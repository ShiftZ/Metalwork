#pragma warning(disable:4456 4459)

#include "MetalGameModeBase.h"

#include "CoreInterface.h"
#include "VesselActor.h"
#include "DebugDrawer.h"
#include "EngineUtils.h"

DEFINE_LOG_CATEGORY(LogNetwork);

void AMetalworkArenaGameModeBase::InitGame(const FString& map_name, const FString& options, FString& error)
{
	AGameModeBase::InitGame(map_name, options, error);

	FString player_str;
	FParse::Value(FCommandLine::Get(), L"player", player_str);

	int player = 0, players = 1;

	swscanf(*player_str, L"%d/%d", &player, &players);
	--player;

	unique_ptr<INetwork> network;

	if (players > 1)
	{
		int target_port = 22150 + (player + 1) % 2;

		RemotePlayer target;
		target.player = (player + 1) % 2;
		target.ip = "127.0.0.1";
		target.port = {target_port, target_port};

		vector targets = {target};
		unique_ptr connection = INetwork::Connect(targets, player);
		
		try { network = connection->Get(); }
		catch (exception& e)
		{
			string err = e.what();
			UE_DEBUG_BREAK();
		}
	}
	
	Core = MakeUnique<MetalCore>(player, move(network));

	for (Vessel* vessel : Core->Arena().Vessels() | cptr)
	{
		AVesselActor* actor = GetWorld()->SpawnActor<AVesselActor>();
		actor->AttachToRig(vessel->body.get());
		Actors.Emplace(actor);

		if (vessel->weapon)
		{
			AWeaponActor* actor = GetWorld()->SpawnActor<AWeaponActor>();
			actor->AttachToRig(vessel->weapon->body.get());
			Actors.Emplace(actor);
		}
	}

	Core->Ready();
	//Core->Start();
}

void AMetalworkArenaGameModeBase::Tick(float dt)
{
	Super::Tick(dt);

	unique_lock lock(Core->arena.step_mtx);
	for (AArenaActor* Actor : Actors)
		Actor->SyncPose();

	DebugDrawer Drawer(GetWorld());
	for (TActorIterator<AArenaActor> Actor(GetWorld()); Actor; ++Actor)
		(*Actor)->Rig->DrawShapes(Drawer);
}		
