#pragma once

#include "MetalGameInstance.generated.h"

UCLASS()
class UMetalGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	void Init() override;
};