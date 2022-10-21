#include "MetalGameInstance.h"

#include "logger.h"
#include "CoreDefinitions.h"

using namespace std;

void UMetalGameInstance::Init()
{
	Super::Init();
	add_logger([](string msg){ UE_LOG(LogTemp, Log, L"%s", ANSI_TO_TCHAR(msg.c_str())); });

	add_logger(DisplayLog(), [&](string msg, const DisplayLog& id)
	{
		GetEngine()->AddOnScreenDebugMessage(id.key, id.time, FColor::Yellow, ANSI_TO_TCHAR(msg.c_str()));
	});
}
