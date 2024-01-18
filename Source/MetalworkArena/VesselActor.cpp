#include "VesselActor.h"

#include "CoreDefinitions.h"
#include "CoreInterface.h"
#include "Weapon.h"
#include "Kismet/GameplayStatics.h"

void AVesselActor::ArenaTick(float DeltaTime)
{
	Super::ArenaTick(DeltaTime);
}

void AChainActor::BeginPlay()
{
	Super::BeginPlay();
	Audio = NewObject<UAudioComponent>(this);
	Audio->SetupAttachment(RootComponent);
	Audio->bAllowSpatialization = false;
	Audio->bAutoActivate = false;
	Audio->SetSound(ChainSound);
	Audio->RegisterComponent();
}

void AChainActor::ArenaTick(float DeltaTime)
{
	Super::ArenaTick(DeltaTime);

	Float NewRopeForce = GetRig()->rope_joint->GetForce();

	Float AccAngle = ranges::fold_left(GetRig()->joints | cptr, 0., [&](Float Acc, Joint* J) {
		return Acc + abs(Unwind(J->GetBodyA()->GetAngle() - J->GetBodyB()->GetAngle()));
	});

	Float AccForce = ranges::fold_left(GetRig()->joints | cptr, 0., [&](Float Acc, Joint* J) {
		return Acc + J->GetForce();
	});

	float Speed = abs(ChainAngle - AccAngle) * fps;
	ChainAngle = AccAngle;

	AngleChangeSpeed = lerp(AngleChangeSpeed, Speed, 1.0);

	if (AngleChangeSpeed > 25)
	{
		if (!Audio->IsPlaying())
		{
			Audio->Play();
			Audio->SetPitchMultiplier(AngleChangeSpeed / 25.f);
			Audio->SetVolumeMultiplier(pow(AngleChangeSpeed / 100, 0.1));
		}
	}

	log(DisplayLog(0), "AngleChangeSpeed = {}", AngleChangeSpeed);

	//if (!Audio->IsPlaying() && LastRopeForce == 0 && NewRopeForce > 0) 
	//	Audio->Play();
	LastRopeForce = NewRopeForce;
}
