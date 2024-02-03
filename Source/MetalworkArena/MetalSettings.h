#pragma once

#include "MetalSettings.generated.h"

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Metal Settings"))
class UMetalSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditAnywhere)
	TSoftClassPtr<class AVesselActor> VesselClass;

	UPROPERTY(config, EditAnywhere)
	TSoftClassPtr<class AChainActor> ChainClass;

	UPROPERTY(config, EditAnywhere)
	TSoftClassPtr<class AAnchorActor> AnchorClass;
};
