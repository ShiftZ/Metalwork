#include "PropActor.h"

#include "ArenaSettings.h"
#include "CoreInterface.h"
#include "RigidObject.h"

TArray<FName> APropActor::GetPropModels()
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
	if (HasAnyFlags(RF_ClassDefaultObject)) return;

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
			Rig = Arena->RigWorld->AddObject(make_shared<RigidObject>(*GetName(), this));

		FString PropName = L"Props/" + RigModel.ToString();
		Rig->LoadModel(GetJson(*PropName));
		Rig->SetPosition(GetActorLocation() / UEScale);

		for (UActorComponent* Component : GetComponentsByInterface(UModelPart::StaticClass()))
		{
			Body* Part = Rig->FindPart(*Component->GetName());
			if (auto* PartComponent = Cast<IModelPart>(Component))
				PartComponent->Body = Part;
		}
	
		if (auto* PartComponent = Cast<IModelPart>(GetRootComponent()))
			PartComponent->Body = Rig->root;
	}

	if (ModelName == NAME_None && Rig)
	{
		Arena->RigWorld->RemoveObject(Rig);
		Rig = nullptr;
	}

	RigModel = ModelName;
}
