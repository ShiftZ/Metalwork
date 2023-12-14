#pragma warning(disable:4458) // declaration hides class member

#include "ArenaActor.h"
#include "ArenaSettings.h"
#include "Kismet/GameplayStatics.h"

void AArenaActor::AttachToRig(RigidObject* Rig)
{
	this->Rig = Rig;

	UDataTable* Models = LoadObject<UDataTable>(nullptr, L"/Game/ModelParts");

	auto MakeComponent = [&](RigidBody* Body)
	{
		FVehicleModelPartRow* ModelRow = Models->FindRow<FVehicleModelPartRow>(ANSI_TO_TCHAR(*Body->model), nullptr);
		USceneComponent* Component = NewObject<USceneComponent>(this, ModelRow->ComponentClass.Get());
		Component->SetupAttachment(GetRootComponent());
		Component->RegisterComponent();
		Component->SetAbsolute(true, true, true);
		Cast<IComponentPocket>(Component)->Body = Body;
		return Component;
	};

	USceneComponent* Root = MakeComponent(Rig->root);
	SetRootComponent(Root);

	for (RigidBody* Part : Rig->parts | cptr)
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
			Comp->SetWorldRotation(FRotator(Pocket->Body->GetAngle(), 0, 0));
		}
	});
}

void AArenaActor::Tick(float dt)
{
	Super::Tick(dt);
	//SyncPose();
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
	{
		FString FilePath = FPaths::ProjectContentDir() + L"Props/" + RigModel.ToString() + L".json";
		FArchive* Reader = IFileManager::Get().CreateFileReader(*FilePath);
		string Json(Reader->TotalSize(), 0);
		Reader->Serialize(Json.data(), Json.size());

		AArenaSettings* Arena = CastChecked<AArenaSettings>(GetWorldSettings());
		if (!Rig) Rig = Arena->RigWorld->AddObject(make_shared<RigidObject>(this));
		Rig->LoadModel(Arena->RigWorld.get(), Json);
	}
}

void APropActor::PostLoad()
{
	Super::PostLoad();
	if (GetFlags() & RF_ClassDefaultObject) return;

	AArenaSettings* Arena = CastChecked<AArenaSettings>(GetWorldSettings());
	string_view Name = StringCast<char>(*GetName()).Get();
	Rig = Arena->RigWorld->FindObject(Name);
}
