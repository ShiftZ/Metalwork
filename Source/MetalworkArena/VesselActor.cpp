#pragma warning(disable:4458) // declaration hides class member

#include "VesselActor.h"

#include "ArenaSettings.h"
#include "RigidWorld.h"

void AArenaActor::AttachToRig(RigidObject* Rig)
{
	this->Rig = Rig;

	UDataTable* Models = LoadObject<UDataTable>(nullptr, L"/Game/ModelParts");

	auto MakeComponent = [&](RigidBody* Body)
	{
		FVehicleModelPartRow* ModelRow = Models->FindRow<FVehicleModelPartRow>(ANSI_TO_TCHAR(Body->model_name.c_str()), nullptr);
		USceneComponent* Component = NewObject<USceneComponent>(this, ModelRow->ComponentClass.Get());
		Component->SetupAttachment(GetRootComponent());
		Component->RegisterComponent();
		Component->SetAbsolute(true, true, true);
		Cast<IComponentPocket>(Component)->Body = Body;
		return Component;
	};

	USceneComponent* Root = MakeComponent(Rig->root);
	SetRootComponent(Root);

	for (RigidBody* Part : Rig->parts | views::values | cptr)
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

string AArenaActor::GetName()
{
	return TCHAR_TO_ANSI(*AActor::GetName());
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
		string Json(Reader->TotalSize(), '');
		Reader->Serialize(Json.data(), Json.size());

		AArenaSettings* Arena = CastChecked<AArenaSettings>(GetWorldSettings());
		Arena->Rig->
	}
}

