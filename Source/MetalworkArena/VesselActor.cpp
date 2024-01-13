#include "VesselActor.h"

#include "CoreInterface.h"
#include "Weapon.h"

void AVesselActor::ArenaTick(float DeltaTime)
{
	Super::ArenaTick(DeltaTime);
}

AChainActor::AChainActor()
{
	Audio = CreateDefaultSubobject<UAudioComponent>("Audio");
	Audio->SetupAttachment(RootComponent);
}

void AChainActor::BeginPlay()
{
	Super::BeginPlay();
	Audio->SetSound(ChainSound);
}

void AChainActor::ArenaTick(float DeltaTime)
{
	Super::ArenaTick(DeltaTime);

	Float NewRopeForce = GetRig()->rope_joint->GetForce();
	if (!Audio->IsPlaying() && LastRopeForce == 0 && NewRopeForce > 0) Audio->Play();
	LastRopeForce = NewRopeForce;
}
