#pragma warning(disable:4456 4459)

#include "MetalGameModeBase.h"

#include "CoreInterface.h"
#include "VesselActor.h"
#include "ArenaSettings.h"
#include "Vessel.h"

DEFINE_LOG_CATEGORY(LogNetwork);

void AMetalworkArenaGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& Error)
{
	AGameModeBase::InitGame(MapName, Options, Error);

	FString PlayerStr;
	FParse::Value(FCommandLine::Get(), L"player", PlayerStr);

	int Player = 1, Players = 1;

	swscanf(*PlayerStr, L"%d/%d", &Player, &Players);
	--Player;

	unique_ptr<INetwork> Network;

	if (Players > 1)
	{
		int TargetPort = 22150 + (Player + 1) % 2;

		RemotePlayer Target;
		Target.player = (Player + 1) % 2;
		Target.ip = "127.0.0.1";
		Target.port = {TargetPort, TargetPort};

		vector Targets = {Target};
		unique_ptr Connection = INetwork::Connect(Targets, Player);
		
		try { Network = Connection->Get(); }
		catch (exception& Ex)
		{
			string err = Ex.what();
			UE_DEBUG_BREAK();
		}
	}

	AArenaSettings* Settings = CastChecked<AArenaSettings>(GetWorldSettings());
	unique_ptr<RigidWorld> RigWorld(Settings->RigWorld.Release());
	
	Core = MakeUnique<MetalCore>(Player, move(RigWorld), move(Network));

	for (Vessel* Ves : Core->Arena().Vessels() | cptr)
	{
		AVesselActor* Actor = GetWorld()->SpawnActor<AVesselActor>();
		Actor->AttachToRig(Ves);
		Actors.Emplace(Actor);

		if (Ves->weapon)
		{
			AWeaponActor* Actor = GetWorld()->SpawnActor<AWeaponActor>();
			Actor->AttachToRig(Ves->weapon.get());
			Actors.Emplace(Actor);
		}
	}

	Core->Ready();
	//Core->Start();
}

void AMetalworkArenaGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	unique_lock lock(Core->arena.step_mtx);
	for (AArenaActor* Actor : Actors)
		Actor->SyncPose();

	Core->arena.rigid_world->DebugDraw();

	//DebugDrawer Drawer(GetWorld());
	//for (TActorIterator<AArenaActor> Actor(GetWorld()); Actor; ++Actor)
	//	(*Actor)->Rig->DrawShapes(Drawer);
}		
