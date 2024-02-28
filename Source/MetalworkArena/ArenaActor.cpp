#pragma warning(disable:4458 4855) // declaration hides class member

#include "ArenaActor.h"
#include "ArenaSettings.h"
#include "RigidObject.h"
#include "Body.h"

void AArenaActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FVector Location = GetActorLocation();
	Location.Y = 0;
	SetActorLocation(Location);
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

void AArenaActor::AttachToRig(RigidObject* Rig)
{
	this->Rig = Rig;

	UDataTable* Models = LoadObject<UDataTable>(nullptr, L"/Game/ModelParts");

	auto MakeComponent = [&](Body* Body)
	{
		FVehicleModelPartRow* ModelRow = Models->FindRow<FVehicleModelPartRow>(*Body->model, nullptr);
		USceneComponent* Component = NewObject<USceneComponent>(this, ModelRow->ComponentClass.Get());
		if (RootComponent)
			Component->SetupAttachment(RootComponent);
		Component->RegisterComponent();
		Component->SetAbsolute(true, true, true);
		Cast<IModelPart>(Component)->Body = Body;
		return Component;
	};

	if (Rig->root)
	{
		if (RootComponent) RootComponent->DestroyComponent();
		SetRootComponent(MakeComponent(Rig->root));
	}

	for (Body* Part : Rig->parts | cptr | drop_value(Rig->root))
		if (Part->model) MakeComponent(Part);

	SyncPose();
}

void AArenaActor::ArenaTick(float DeltaTime)
{
	SyncPose();
}

void AArenaActor::SyncPose()
{
	ForEachComponent<USceneComponent>(false, [](USceneComponent* Comp)
	{
		if (IModelPart* Pocket = Cast<IModelPart>(Comp))
		{
			Comp->SetWorldLocation(Pocket->Body->GetPosition() * UEScale);
			Comp->SetWorldRotation(FRotator(FMath::RadiansToDegrees(Pocket->Body->GetAngle()), 0, 0));
		}
	});

	if (Rig->root)
		GetRootComponent()->SetWorldLocation(Rig->root->GetPosition() * UEScale);
}