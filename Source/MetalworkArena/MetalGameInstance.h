#pragma once

#include "MetalGameInstance.generated.h"

UCLASS()
class UMetalGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<UObject*> PreloadedAssets;

private:
	void Init() override;
};