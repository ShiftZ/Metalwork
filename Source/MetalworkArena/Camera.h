#pragma once

#include "Camera.generated.h"

UCLASS()
class ACamera : public APawn
{
	GENERATED_BODY()

public:
	class RigidObject* Target = nullptr;

public:
	ACamera() { PrimaryActorTick.bCanEverTick = true; }
	void SetTarget(RigidObject* Target);
	void Tick(float DeltaSeconds) override;
};