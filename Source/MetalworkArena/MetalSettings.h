#pragma once

#include "Body.h"
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

	UPROPERTY(config, EditAnywhere)
	TSoftClassPtr<AActor> FlagClass;

	UPROPERTY(config, EditAnywhere)
	TSoftObjectPtr<class USoundData> Sound;

	UPROPERTY(config, EditAnywhere)
	TSoftObjectPtr<class UEffectsData> Effects;
};

UENUM(Meta=(Bitflags))
enum class ERigMaterial
{
	Void = Body::Void,
	Wood = Body::Wood,
	Stone = Body::Stone,
	Metal = Body::Metal
};
ENUM_CLASS_FLAGS(ERigMaterial)

USTRUCT()
struct FImpactSound
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Meta = (Bitmask, BitmaskEnum="ERigMaterial"))
	int32 Materials = 0;

	UPROPERTY(EditAnywhere, Meta=(AllowedClasses = "UMetaSoundSource"))
	TSoftObjectPtr<UMetaSoundSource> Sound;
};

UCLASS()
class USoundData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FImpactSound> Impacts;

	UMetaSoundSource* GetImpactSound(Body::Material MatA, Body::Material MatB)
	{
		int32 Materials = (1 << MatA) | (1 << MatB) & ~1;
		FImpactSound* Impact = Impacts.FindByPredicate([&](FImpactSound& Impact){ return Impact.Materials == Materials; });
		return Impact ? Impact->Sound.LoadSynchronous() : nullptr;
	}
};

USTRUCT()
struct FImpactEffect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Meta = (Bitmask, BitmaskEnum="ERigMaterial"))
	int32 Materials = 0;

	UPROPERTY(EditAnywhere, Meta=(AllowedClasses = "UMetaSoundSource"))
	TSoftObjectPtr<UNiagaraSystem> Effect;
};

UCLASS()
class UEffectsData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FImpactEffect> Impacts;

	UNiagaraSystem* GetImpactEffect(Body::Material MatA, Body::Material MatB)
	{
		int32 Materials = (1 << MatA) | (1 << MatB) & ~1;
		FImpactEffect* Impact = Impacts.FindByPredicate([&](FImpactEffect& Impact){ return Impact.Materials == Materials; });
		return Impact ? Impact->Effect.LoadSynchronous() : nullptr;
	}
};