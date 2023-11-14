#include "MetalGameInstance.h"
#include "CoreInterface.h"

struct test_with_location
{
	test_with_location( std::format_string<int, int> str ) {}
};

void test( test_with_location fmt )
{
}

void UMetalGameInstance::Init()
{
	test("P{}:{}");

	Super::Init();
	
	if (!UE_BUILD_SHIPPING)
	{
		add_logger([](string msg){ UE_LOG(LogTemp, Log, L"%s", ANSI_TO_TCHAR(msg.c_str())); });
		add_logger(DisplayLog(), [&](string msg, const DisplayLog& id)
			{ GetEngine()->AddOnScreenDebugMessage(id.key, id.time, FColor::Yellow, msg.c_str()); });

		flog_path("network", *(FPaths::ProjectLogDir() + "Network.log"), "---------------------------");

		add_logger(NetworkLog(), [&](string msg, const NetworkLog& id)
			{ flog("network", "P{}:{}", id.player, msg); });
	}
}
