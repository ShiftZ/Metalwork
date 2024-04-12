#include "VesselActor.h"

#include "CoreDefinitions.h"
#include "Vessel.h"
#include "Weapon.h"
#include "Joint.h"
#include "Body.h"

using namespace Chaos;

void AVesselActor::BeginPlay()
{
	Super::BeginPlay();
	Audio = NewObject<UAudioComponent>(this);
	Audio->SetSound(PropellerSound);
	Audio->SetupAttachment(GetRootComponent());
	Audio->RegisterComponent();
	Audio->SetFloatParameter(AccelName, 0);
}

void AVesselActor::ArenaTick(float DeltaTime)
{
	Super::ArenaTick(DeltaTime);

	float CurrentAccel = Audio->GetInstanceParameters().Last().FloatParam;
	float DesiredAccel = Length(GetRig()->desired_accel);

	float K = (DesiredAccel > CurrentAccel) ? 4 : 3;
	float Accel = lerp(CurrentAccel, DesiredAccel, clamp(K * DeltaTime, 0.f, 1.f));

	Audio->SetFloatParameter(AccelName, Accel);

	// this should be done in blueprints
	UPaperFlipbookComponent* CopterFlipbook = Cast<UPaperFlipbookComponent>(GetRootComponent());
	CopterFlipbook->SetPlayRate(1 + 2 * sigmoid(Accel, 10));
}

void AChainActor::BeginPlay()
{
	Super::BeginPlay();
	Audio = NewObject<UAudioComponent>(this);
	Audio->SetAutoActivate(false);
	Audio->SetSound(ChainSound);
	Audio->SetFloatParameter(ImpulseName, 1);
	Audio->SetupAttachment(GetRootComponent());
	Audio->RegisterComponent();
}

void AChainActor::ArenaTick(float DeltaTime)
{
	Super::ArenaTick(DeltaTime);

	/*Float AccAngle = ranges::fold_left(GetRig()->joints | cptr, 0., [&](Float Acc, Joint* J) {
		return Acc + abs(Unwind(J->GetBodyA()->GetAngle() - J->GetBodyB()->GetAngle()));
	});*/

	Float AccImpulse = GetRig()->rope_joint->GetForce();
	AccImpulse += ranges::fold_left(GetRig()->joints | cptr, 0, [&](Float Acc, Joint* J) { return Acc + J->GetForce(); });
	AccImpulse *= frame_time;

	Loose |= AccImpulse < 0.1;

	if (Audio->IsPlaying())
	{
		if (AccImpulse > Audio->GetInstanceParameters().Last().FloatParam)
			Audio->SetFloatParameter(ImpulseName, AccImpulse);
	}
	else if (Loose && AccImpulse > 0.33)
	{
		Audio->SetFloatParameter(ImpulseName, AccImpulse - 0.33);
		Audio->Play();
		Loose = false;
	}
}

void AAnchorActor::BeginPlay()
{
	Super::BeginPlay();
	Audio = NewObject<UAudioComponent>(this);
	Audio->SetupAttachment(GetRootComponent());
	Audio->SetSound(WhooshSound);
	Audio->RegisterComponent();
}

void AAnchorActor::ArenaTick(float DeltaTime)
{
	Super::ArenaTick(DeltaTime);
	Audio->SetFloatParameter(SpeedName, Length(GetRig()->root->GetVelocity()));
}

void AClothActor::BeginPlay()
{
	Super::BeginPlay();

	TArray<UChaosClothComponent*> ClothsComponents;
	GetComponents<UChaosClothComponent>(ClothsComponents);

	for (UChaosClothComponent* Cloth : ClothsComponents)
	for (int i = 0; i < 40; i++)
		((UActorComponent*)Cloth)->TickComponent(1.f, LEVELTICK_All, nullptr);
}