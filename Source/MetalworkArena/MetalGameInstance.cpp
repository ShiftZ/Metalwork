#include "MetalGameInstance.h"

#include "logger.h"

using namespace std;

void UMetalGameInstance::Init()
{
	Super::Init();
	reg_logger([](string msg){ UE_LOG(LogTemp, Log, L"%s", ANSI_TO_TCHAR(msg.c_str())); });
}
