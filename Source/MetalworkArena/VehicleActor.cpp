#pragma warning(disable:4458) // declaration hides class member

#include "VehicleActor.h"

#include <ranges>
#include "vector3.h"

using namespace std;

void AArenaActor::AttachToObject(RigidObject* object)
{
	UDataTable* models = LoadObject<UDataTable>(nullptr, L"/Game/ModelParts");

	auto MakeComponent = [&](RigidBody* body)
	{
		FVehicleModelPartRow* model_row = models->FindRow<FVehicleModelPartRow>(ANSI_TO_TCHAR(body->model_name.c_str()), nullptr);
		USceneComponent* component = NewObject<USceneComponent>(this, model_row->component_class.Get());
		component->SetupAttachment(GetRootComponent());
		component->RegisterComponent();
		component->SetAbsolute(true, true, true);
		Cast<IComponentPocket>(component)->body = body;
		return component;
	};

	USceneComponent* root = MakeComponent(object->root);
	SetRootComponent(root);

	for (shared_ptr<RigidBody>& part : views::values(object->parts))
		if (part.get() != object->root) MakeComponent(part.get());

	SyncPose();

	this->object = object;
}

void AArenaActor::SyncPose()
{
	ForEachComponent<USceneComponent>(false, [](USceneComponent* component)
	{
		if (IComponentPocket* pocket = Cast<IComponentPocket>(component))
			component->SetWorldLocation(pocket->body->GetPosition3D() * 10);
	});
}

void AArenaActor::Tick(float dt)
{
	Super::Tick(dt);
	SyncPose();
}

