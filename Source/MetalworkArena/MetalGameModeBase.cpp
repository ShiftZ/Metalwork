#pragma warning(disable:4456 4459)

#include "MetalGameModeBase.h"

#include "ArenaSettings.h"
#include "Camera.h"
#include "CoreInterface.h"
#include "DebugDrawer.h"
#include "MetalSettings.h"
#include "Vessel.h"
#include "VesselActor.h"

FAutoConsoleVariable CVarDebugDraw(L".DebugDraw", false, L"");

DEFINE_LOG_CATEGORY(LogNetwork);

void AMetalworkArenaGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& Error)
{
	AGameModeBase::InitGame(MapName, Options, Error);

	FString PlayerStr;
	FParse::Value(FCommandLine::Get(), L"player", PlayerStr);

	if (swscanf(*PlayerStr, L"%d/%d", &Player, &Players) == 2) --Player;

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

	RigidWorld* RigWorld = CastChecked<AArenaSettings>(GetWorldSettings())->RigWorld.Release();
	Core = MakeUnique<MetalCore>(Player, unique_ptr<RigidWorld>(RigWorld), move(Network));
	const UMetalSettings* Settings = GetDefault<UMetalSettings>();

	for (Vessel* Ves : Core->arena.vessels)
	{
		AVesselActor* Actor = GetWorld()->SpawnActor<AVesselActor>(Settings->VesselClass.LoadSynchronous());
		Actor->AttachToRig(Ves);

		if (Ves->player == 0)
		{
			AActor* Flag = GetWorld()->SpawnActor<AActor>(Settings->FlagClass.LoadSynchronous());
			Flag->AttachToActor(Actor, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}

		if (Chain* ChainWeapon = dynamic_cast<Chain*>(Ves->weapon))
		{
			AChainActor* ChainActor = GetWorld()->SpawnActor<AChainActor>(Settings->ChainClass.LoadSynchronous());
			ChainActor->AttachToRig(ChainWeapon);

			AAnchorActor* AnchorActor = GetWorld()->SpawnActor<AAnchorActor>(Settings->AnchorClass.LoadSynchronous());
			AnchorActor->AttachToRig(ChainWeapon->anchor);
		}
	}

	Core->Ready();
	//Core->Start();
}

void AMetalworkArenaGameModeBase::RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot)
{
	Super::RestartPlayerAtPlayerStart(NewPlayer, StartSpot);
	ACamera* Camera = NewPlayer->GetPawn<ACamera>();
	Camera->SetTarget(Core->arena.vessels[Player]);
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

	if (CVarDebugDraw->GetBool())
		Core->arena.rigid_world->DebugDraw(DebugDrawer(GetWorld(), false));
}		
