#pragma warning(disable:4458) // declaration hides class member

#include "VesselActor.h"

void AArenaActor::AttachToRig(RigidObject* Rig)
{
	this->Rig = Rig;

	UDataTable* models = LoadObject<UDataTable>(nullptr, L"/Game/ModelParts");

	auto MakeComponent = [&](RigidBody* Body)
	{
		FVehicleModelPartRow* ModelRow = models->FindRow<FVehicleModelPartRow>(ANSI_TO_TCHAR(Body->model_name.c_str()), nullptr);
		USceneComponent* Component = NewObject<USceneComponent>(this, ModelRow->component_class.Get());
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

