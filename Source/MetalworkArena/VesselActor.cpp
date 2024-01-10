#include "VesselActor.h"

#include "CoreInterface.h"
#include "Vessel.h"

AVesselActor::AVesselActor()
{
	//Audio = CreateDefaultSubobject<UAudioComponent>("Audio");
}

void AVesselActor::ArenaTick(float DeltaTime)
{
	Super::ArenaTick(DeltaTime);

	if (GetRig()->weapon)
	{
		//log(DisplayLog(1), "joint force = {}", GetRig()->head_joint->GetForce());
	}
}
