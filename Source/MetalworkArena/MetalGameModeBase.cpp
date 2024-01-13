#pragma warning(disable:4456 4459)

#include "MetalGameModeBase.h"

#include "CoreInterface.h"
#include "VesselActor.h"
#include "ArenaSettings.h"
#include "Vessel.h"
#include "MetalSettings.h"

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

	for (Vessel* Ves : Core->Arena().Vessels())
	{
		AVesselActor* Actor = GetWorld()->SpawnActor<AVesselActor>();
		Actor->AttachToRig(Ves);

		if (Chain* ChainWeapon = dynamic_cast<Chain*>(Ves->weapon))
		{
			auto* Settings = GetDefault<UMetalSettings>();

			AChainActor* ChainActor = GetWorld()->SpawnActor<AChainActor>(Settings->ChainClass.LoadSynchronous());
			ChainActor->AttachToRig(ChainWeapon);

			AArenaActor* AnchorActor = GetWorld()->SpawnActor<AArenaActor>();
			AnchorActor->AttachToRig(ChainWeapon->anchor);
		}
	}

	Core->Ready();
	//Core->Start();
    
}

void AMetalworkArenaGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UInputComponent* PlayerInput = GetWorld()->GetFirstPlayerController()->InputComponent;
	PlayerInput->BindKey(EKeys::P, IE_Pressed, this, &AMetalworkArenaGameModeBase::Pause);
}

void AMetalworkArenaGameModeBase::Pause()
{
	Core->SingleStep();
}

void AMetalworkArenaGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	unique_lock lock(Core->arena.step_mtx);

	for (TActorIterator<AArenaActor> ActorIt(GetWorld()); ActorIt; ++ActorIt)
		ActorIt->ArenaTick(DeltaTime);

	Core->arena.rigid_world->DebugDraw(DebugDrawer(GetWorld(), false));
}		
