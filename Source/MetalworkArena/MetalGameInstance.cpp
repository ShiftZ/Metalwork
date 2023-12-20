#include "MetalGameInstance.h"
#include "CoreInterface.h"

void UMetalGameInstance::Init()
{
	Super::Init();

	SetContentPath(*FPaths::ProjectContentDir());
	
	if (!UE_BUILD_SHIPPING)
	{
		add_logger([](string msg){ UE_LOG(LogTemp, Log, L"%s", StringCast<wchar_t>(msg.c_str()).Get()); });
		add_logger(DisplayLog(), [&](string msg, const DisplayLog& id)
			{ GetEngine()->AddOnScreenDebugMessage(id.key, id.time, FColor::Yellow, msg.c_str()); });

		flog_path("network", *(FPaths::ProjectLogDir() + "Network.log"), "---------------------------");

		add_logger(NetworkLog(), [&](string msg, const NetworkLog& id)
			{ flog("network", "P{}:{}", id.player, msg); });
	}
}
