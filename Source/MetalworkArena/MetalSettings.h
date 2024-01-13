#pragma once

#include "MetalSettings.generated.h"

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Metal Settings"))
class UMetalSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Custom")
    TSoftClassPtr<class AChainActor> ChainClass;
};