#include "VesselActor.h"

#include "CoreDefinitions.h"
#include "CoreInterface.h"
#include "Weapon.h"

void AChainActor::BeginPlay()
{
	Super::BeginPlay();
	Audio = NewObject<UAudioComponent>(this);
	Audio->SetupAttachment(RootComponent);
	Audio->bAllowSpatialization = false;
	Audio->bAutoActivate = false;
	Audio->SetSound(ChainSound);
	Audio->SetFloatParameter("Stretch", 1);
	Audio->RegisterComponent();
}

void AChainActor::ArenaTick(float DeltaTime)
{
	Super::ArenaTick(DeltaTime);

	Float NewRopeForce = GetRig()->rope_joint->GetForce();

	/*Float AccAngle = ranges::fold_left(GetRig()->joints | cptr, 0., [&](Float Acc, Joint* J) {
		return Acc + abs(Unwind(J->GetBodyA()->GetAngle() - J->GetBodyB()->GetAngle()));
	});*/

	Float AccImpulse = frame_time * ranges::fold_left(GetRig()->joints | cptr,
	                                                  GetRig()->rope_joint->GetForce(),
	                                                  [&](Float Acc, Joint* J) { return Acc + J->GetForce(); });

	Loose |= AccImpulse < 0.1;

	if (Audio->IsPlaying())
	{
		if (AccImpulse > Audio->GetInstanceParameters().Last().FloatParam)
			Audio->SetFloatParameter(StretchName, AccImpulse);
	}
	else if (Loose && AccImpulse > 0.33)
	{
		Audio->SetFloatParameter(StretchName, AccImpulse - 0.33);
		Audio->Play();
		Loose = false;
	}
}