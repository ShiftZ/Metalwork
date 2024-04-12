// Copyright Epic Games, Inc. All Rights Reserved.

#include "MetalworkArena.h"
#include "CoreInterface.h"
#include "MetasoundFrontendRegistries.h"

class FMetalworkArenaModule : public FDefaultGameModuleImpl
{
	void StartupModule() override
	{
		SetContentPath(*FPaths::ProjectContentDir(), WITH_EDITOR);

		FCoreUObjectDelegates::ReloadCompleteDelegate.AddLambda([](EReloadCompleteReason)
			{ SetContentPath(*FPaths::ProjectContentDir(), WITH_EDITOR); });
		FMetasoundFrontendRegistryContainer::Get()->RegisterPendingNodes();

		if (!UE_BUILD_SHIPPING)
		{
			add_logger([](string msg){ UE_LOG(LogTemp, Log, L"%s", StringCast<wchar_t>(msg.c_str()).Get()); });

			add_logger(DisplayLog(), [&](string msg, const DisplayLog& id)
				{ GEngine->AddOnScreenDebugMessage(id.key, id.time, FColor::Yellow, msg.c_str()); });

			flog_path("network", *(FPaths::ProjectLogDir() + "Network.log"), "---------------------------");

			add_logger(NetworkLog(), [&](string msg, const NetworkLog& id)
				{ flog("network", "P{}:{}", id.player, msg); });
		}
	}
};

void AssertionFailed(const char* Expr, const char* Loc, int Line, const FString& Message)
{
    const char* File = AlgoImpl::FindLastByPredicate(Loc, strlen(Loc), [](char Ch) { return Ch == '\\' || Ch == '/'; });

    FString Location = FString::Printf(L"%hs:%d", ++File, Line);
    FString LogMessage = FString::Printf(L"Assertion failed at %s (%hs)\n%s", *Location, Expr, *Message);

    UE_LOG(LogOutputDevice, Error, L"%s", *LogMessage);
    if (WITH_EDITOR) FMessageLog("PIE").Error(FText::AsCultureInvariant(LogMessage));

    FString DialogMsg = FString::Printf(L"%s:(%hs) evaluated to false.\n%sContinue session?", *Location, Expr, *Message);
    if (FMessageDialog::Open(EAppMsgType::YesNo, FText::AsCultureInvariant(DialogMsg), INVTEXT("Assertion failure")) == EAppReturnType::No)
    {
        if (WITH_EDITOR && GEngine->GameViewport)
            GEngine->GameViewport->CloseRequested(GEngine->GameViewport->Viewport);
        else
            FPlatformMisc::RequestExit(true);
    }
}

FString VARARGS PrintMessage(const TCHAR* Format, ...)
{
    constexpr int BufSize = 1024;
    TCHAR Buffer[BufSize];
    va_list ArgPtr;
    va_start(ArgPtr, Format);
    int Len = vswprintf(Buffer, BufSize - 1, Format, ArgPtr);
    va_end(ArgPtr);
    Buffer[Len] = '\n', Buffer[Len + 1] = '\0';
    return Buffer;
}

IMPLEMENT_PRIMARY_GAME_MODULE(FMetalworkArenaModule, MetalworkArena, "MetalworkArena");
