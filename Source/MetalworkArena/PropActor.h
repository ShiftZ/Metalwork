#pragma once

#include "ArenaActor.h"
#include "PropActor.generated.h"

UCLASS()
class METALWORKARENA_API APropActor : public AArenaActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta=(GetOptions="GetPropModels"))
	FName RigModel;

	UFUNCTION()
	TArray<FName> GetPropModels();

	void PostEditChangeProperty(FPropertyChangedEvent& Event) override;
	void PostRegisterAllComponents() override;
	void SetModel(FName ModelName);

	class RigidProp* GetRig() { return (RigidProp*)Rig; }
};