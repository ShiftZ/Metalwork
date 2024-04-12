//#pragma warning(disable:4456 4459)

#include "CoreMode.h"

#include "ArenaSettings.h"
#include "Camera.h"
#include "CoreInterface.h"
#include "DebugDrawer.h"
#include "MetalSettings.h"
#include "NiagaraFunctionLibrary.h"
#include "Vessel.h"
#include "Weapon.h"
#include "VesselActor.h"

FAutoConsoleVariable CVarDebugDraw(L".DebugDraw", false, L"");

DEFINE_LOG_CATEGORY(LogNetwork);

void ACoreMode::InitGame(const FString& MapName, const FString& Options, FString& Error)
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

void ACoreMode::RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot)
{
	Super::RestartPlayerAtPlayerStart(NewPlayer, StartSpot);
	ACamera* Camera = NewPlayer->GetPawn<ACamera>();
	Camera->SetTarget(Core->arena.vessels[Player]);
}

void ACoreMode::BeginPlay()
{
	Super::BeginPlay();

	SoundData = GetDefault<UMetalSettings>()->Sound.LoadSynchronous();
	EffectsData = GetDefault<UMetalSettings>()->Effects.LoadSynchronous();

	UInputComponent* PlayerInput = GetWorld()->GetFirstPlayerController()->InputComponent;
	PlayerInput->BindKey(EKeys::P, IE_Pressed, this, &ACoreMode::Pause);
}

void ACoreMode::Pause()
{
	Core->SingleStep();
}

void ACoreMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	unique_lock Lock(Core->arena.step_mtx);

	for (TActorIterator<AArenaActor> ActorIt(GetWorld()); ActorIt; ++ActorIt)
		ActorIt->ArenaTick(DeltaTime);

	if (CVarDebugDraw->GetBool())
		Core->arena.rigid_world->DebugDraw(DebugDrawer(GetWorld(), false));

	ProcessEvents();
}

void ACoreMode::ProcessEvents()
{
	static const auto WeaponImpact = [](UWorld* World, ImpactEvent* Event)
	{
		TArray<UArenaEvent*> Events;

		auto [BodyA, BodyB] = GetBodies(Event->contact);
		FVector Location = GetLocation(Event->contact) * UEScale;

		const UMetalSettings* Settings = GetDefault<UMetalSettings>();

		if (UMetaSoundSource* SoundSource = Settings->Sound->GetImpactSound(BodyA->material, BodyB->material))
		{
			USoundEvent* SoundEvent = NewObject<USoundEvent>();
			UAudioComponent* Component = UGameplayStatics::SpawnSoundAtLocation(World, SoundSource, Location);
			Component->SetFloatParameter(ImpulseName, Event->impulse);
			Component->Play();

			SoundEvent->Component = Component;
			Events.Add(SoundEvent);
		}

		if (UNiagaraSystem* EffectTemplate = Settings->Effects->GetImpactEffect(BodyA->material, BodyB->material))
		{
			UNiagaraComponent* Component = UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, EffectTemplate, Location);
			UEffectEvent* EffectEvent = NewObject<UEffectEvent>();
			EffectEvent->Component = Component;
			Events.Add(EffectEvent);
		}

		return Events;
	};

	static unordered_map<type_index, function<TArray<UArenaEvent*>(UWorld*, ArenaEvent*)>> EventConstructor =
	{
		{typeid(ImpactEvent), (TArray<UArenaEvent*>(*)(UWorld*, ArenaEvent*))+WeaponImpact}
	};

	for (ArenaEvent* Event : Core->arena.GetNewEvents())
	{
		TArray<UArenaEvent*> NewEvents = EventConstructor[type_index(typeid(*Event))](GetWorld(), Event);
		Events.Append(NewEvents);
	}

	for (ArenaEvent* Denied : Core->arena.GetDeniedEvents() | cptr)
	{
		int RemovedIdx = Algo::RemoveIf(Events, [&](UArenaEvent* Event){ return Event->CoreEvent == Denied; });
		while (Events.Num() != RemovedIdx)
		{
			Events.Last()->Denied();
			Events.Pop(false);
		}
	}

	for (ArenaEvent* Confirmed : Core->arena.GetConfirmedEvents() | cptr)
	{
		int RemovedIdx = Algo::RemoveIf(Events, [&](UArenaEvent* Event){ return Event->CoreEvent == Confirmed; });
		while (Events.Num() != RemovedIdx)
		{
			Events.Last()->Confirmed();
			Events.Pop(false);
		}
	}
}
