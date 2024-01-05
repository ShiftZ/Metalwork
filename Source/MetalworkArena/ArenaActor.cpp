#pragma warning(disable:4458 4855) // declaration hides class member

#include "ArenaActor.h"
#include "ArenaSettings.h"

void AArenaActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FVector Location = GetActorLocation();
    Location.Y = 0;
    SetActorLocation(Location);
}

void AArenaActor::AttachToRig(RigidObject* Rig)
{
	this->Rig = Rig;

	UDataTable* Models = LoadObject<UDataTable>(nullptr, L"/Game/ModelParts");

	auto MakeComponent = [&](Body* Body)
	{
		FVehicleModelPartRow* ModelRow = Models->FindRow<FVehicleModelPartRow>(*Body->model, nullptr);
		USceneComponent* Component = NewObject<USceneComponent>(this, ModelRow->ComponentClass.Get());
		Component->SetupAttachment(GetRootComponent());
		Component->RegisterComponent();
		Component->SetAbsolute(true, true, true);
		Cast<IComponentPocket>(Component)->Body = Body;
		return Component;
	};

	USceneComponent* Root = MakeComponent(Rig->root);
	SetRootComponent(Root);

	for (Body* Part : Rig->parts | cptr)
		if (Part != Rig->root) MakeComponent(Part);

	SyncPose();
}

void AArenaActor::SyncPose()
{
	ForEachComponent<USceneComponent>(false, [](USceneComponent* Comp)
	{
		if (IComponentPocket* Pocket = Cast<IComponentPocket>(Comp))
		{
			Comp->SetWorldLocation(Pocket->Body->GetPosition() * UEScale);
			Comp->SetWorldRotation(FRotator(FMath::RadiansToDegrees(Pocket->Body->GetAngle()), 0, 0));
		}
	});
}

void AArenaActor::Tick(float dt)
{
	Super::Tick(dt);
	//SyncPose();
}

void AArenaActor::Destroyed()
{
	Super::Destroyed();
	if (Rig && Rig->world)
		Rig->world->RemoveObject(Rig);
}

void AArenaActor::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);

    FVector Location = GetActorLocation();
    Location.Y = 0;
    SetActorLocation(Location);
	if (Rig) Rig->SetPosition(Location / UEScale);
}

void ATestActor::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);

	if (Event.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ATestActor, RigModel))
	{
		if (FPaths::FileExists(FPaths::ProjectContentDir() + RigModel + ".json"))
		{
			auto LoadModel = [&]
			{
				vec2 Position = GetActorLocation() / UEScale;

				if (Rig)
				{
					Position = Rig->GetPosition();
					Rig->Release();
				}
				
				TSet<UActorComponent*> Components = GetComponents();
				for (UActorComponent* Component : Components)
					Component->DestroyComponent();

				AArenaSettings* Arena = CastChecked<AArenaSettings>(GetWorldSettings());
				Name ActorName = StringCast<char>(*GetName()).Get();
				Rig = Arena->RigWorld->AddObject(make_shared<RigidObject>(ActorName, this));

				Rig->LoadModel(GetJson(*RigModel));
				Rig->SetPosition(Position);
				AttachToRig(Rig);
			};

			LoadModel();

			auto OnFileChanged = IDirectoryWatcher::FDirectoryChanged::CreateLambda([=](const TArray<FFileChangeData>& Files)
		    {
				auto IsModelFile = [&](FFileChangeData& File){ return File.Filename.Contains(RigModel + L".json"); };
				if (Files.ContainsByPredicate(IsModelFile)) LoadModel();
		    });

			IDirectoryWatcher* Watcher = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>(L"DirectoryWatcher").Get();

			if (WatchHandle.IsValid())
				Watcher->UnregisterDirectoryChangedCallback_Handle(WatchPath, WatchHandle);

			WatchPath = FPaths::GetPath(FPaths::ProjectContentDir() + RigModel);
		    Watcher->RegisterDirectoryChangedCallback_Handle(WatchPath, OnFileChanged, WatchHandle);
		}
	}
}

void ATestActor::Destroyed()
{
	Super::Destroyed();

	if (WatchHandle.IsValid())
	{
		IDirectoryWatcher* Watcher = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>(L"DirectoryWatcher").Get();
		Watcher->UnregisterDirectoryChangedCallback_Handle(WatchPath, WatchHandle);
	}
}

TArray<FName> APropActor::GetRigs()
{
	TArray<FName> Rigs;

	FString Folder = FPaths::ProjectContentDir() + L"Props";
	TArray<FString> Files;
	IFileManager::Get().FindFiles(Files, *Folder, L"json");
    for (FString& File : Files)
		Rigs.Add(*FPaths::GetBaseFilename(*File));

	return Rigs;
}

void APropActor::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);
	if (Event.GetPropertyName() == GET_MEMBER_NAME_CHECKED(APropActor, RigModel))
		SetModel(RigModel);
}

void APropActor::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();
	SetModel(RigModel);
}

void APropActor::SetModel(FName ModelName)
{
	AArenaSettings* Arena = CastChecked<AArenaSettings>(GetWorldSettings());

	if (ModelName != NAME_None)
	{
		if (!Rig)
		{
			Name ActorName = StringCast<char>(*GetName()).Get();
			Rig = Arena->RigWorld->AddObject(make_shared<RigidObject>(ActorName, this));
		}

		FString PropName = L"Props/" + RigModel.ToString();
		Rig->LoadModel(GetJson(*PropName));
		Rig->SetPosition(GetActorLocation() / UEScale);
	}

	if (ModelName == NAME_None && Rig)
	{
		Arena->RigWorld->RemoveObject(Rig);
		Rig = nullptr;
	}

	RigModel = ModelName;
}
