#include "Camera.h"

#include "CoreInterface.h"
#include "RigidWorld.h"

void ACamera::SetTarget(RigidObject* Target)
{
	this->Target = Target;
}

bool isInsideEllipse(double a, double b, double x, double y)
{
    return (sqr(x)/sqr(a) + sqr(y)/sqr(b)) <= 1.0;
}


void ACamera::Tick(float DeltaSeconds)
{
	Vec2 Boundry = {2, 2};

	Vec2 CameraPosition = GetActorLocation() / UEScale;
	Vec2 TargetVec = Target->GetPosition() - CameraPosition;
	TargetVec.x -= clamp(TargetVec.x, -Boundry.x, Boundry.x);
	TargetVec.y -= clamp(TargetVec.y, -Boundry.y, Boundry.y);

	TargetVec *= 2 * DeltaSeconds;

	FVector NewCameraPosition = (CameraPosition + TargetVec) * UEScale;
	NewCameraPosition.Y = GetActorLocation().Y;

	log(DisplayLog(0), "TargetVec = {},{}", TargetVec.x, TargetVec.y);

	SetActorLocation(NewCameraPosition);
}