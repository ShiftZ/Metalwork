#include "ArenaSettings.h"
#include "ArenaActor.h"
#include "DebugDrawer.h"
#include "Arena.h"
#include "PropActor.h"

void AArenaSettings::PostInitProperties()
{
	Super::PostInitProperties();
	if (GetFlags() & RF_ClassDefaultObject) return;

	RigWorld.Reset(Arena::MakeWorld().release());
}

void AArenaSettings::PostLoad()
{
	Super::PostLoad();
	if (GetFlags() & RF_ClassDefaultObject) return;

#	if WITH_EDITOR
	{
		UChildActorComponent* ActorComponent = NewObject<UChildActorComponent>(this);
		ActorComponent->SetChildActorClass(AEditorTicker::StaticClass());
		ActorComponent->RegisterComponent();

		auto OnFileChanged = IDirectoryWatcher::FDirectoryChanged::CreateLambda([this](const TArray<FFileChangeData>& Files)
		{
			TSet<FName> Models;
			for (const FFileChangeData& Change : Files)
			{
				if (FPaths::GetExtension(Change.Filename) == L"json")
					Models.Add(*FPaths::GetBaseFilename(Change.Filename));
			}

			for (FName& ModelName : Models)
			for (TActorIterator<APropActor> It(GetWorld()); It; ++It)
			{
				if ((*It)->RigModel == ModelName)
					(*It)->SetModel(ModelName);
			}
		});

		IDirectoryWatcher* Watcher = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>(L"DirectoryWatcher").Get();
		Watcher->RegisterDirectoryChangedCallback_Handle(WatchDirectory, OnFileChanged, WatchHandle);
	}
#	endif
}

void AArenaSettings::EditorTick()
{
	FlushPersistentDebugLines(GetWorld());
	RigWorld->DebugDraw(DebugDrawer(GetWorld(), true));
}

AArenaSettings::~AArenaSettings()
{
#	if WITH_EDITOR
	{
		IDirectoryWatcher* Watcher = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>(L"DirectoryWatcher").Get();
		Watcher->UnregisterDirectoryChangedCallback_Handle(WatchDirectory, WatchHandle);
	}
#	endif
}