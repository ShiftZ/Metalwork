#include "ArenaSettings.h"
#include "ArenaActor.h"
#include "DebugDrawer.h"
#include "Arena.h"

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
			auto OnlyJson = [](const FFileChangeData& Change){ return FPaths::GetExtension(Change.Filename) == L"json"; };
			for (const FFileChangeData& Change : Files.FilterByPredicate(OnlyJson))
			{
				FName ModelName = *FPaths::GetBaseFilename(Change.Filename);
				for (TActorIterator<APropActor> It(GetWorld()); It; ++It)
				{
					if ((*It)->RigModel == ModelName)
					{
						(*It)->SetModel(ModelName);
					}
				}
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

void AArenaSettings::Destroyed()
{
	Super::Destroyed();

#	if WITH_EDITOR
	{
		IDirectoryWatcher* Watcher = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>(L"DirectoryWatcher").Get();
		Watcher->UnregisterDirectoryChangedCallback_Handle(WatchDirectory, WatchHandle);
	}
#	endif
}
