#include "ArenaSettings.h"
#include "ArenaActor.h"
#include "Arena.h"

void AArenaSettings::PostInitProperties()
{
	Super::PostInitProperties();
	RigWorld = Arena::MakeWorld();
}

void AArenaSettings::Serialize(FArchive& Ar)
{
	AInfo::Serialize(Ar);

	if (Ar.IsLoading())
	{ 
		UPackage* Pkg = Ar.GetLinker()->LinkerRoot;
		FString Name = FPaths::GetBaseFilename(Pkg->GetLoadedPath().GetPackageName());

		FString FilePath = FPaths::ProjectContentDir() + L"Maps/" + Name + L".json";
		TUniquePtr<FArchive> Reader(IFileManager::Get().CreateFileReader(*FilePath));
		if (!Reader) return;

		string Json(Reader->TotalSize(), 0);
		Reader->Serialize(Json.data(), Json.size());
		RigWorld->LoadFromJson(Json);
	}
	else if (Ar.IsSaving() && Ar.ContainsMap())
	{
		for (RigidObject* obj : RigWorld->objects | cptr)
		{
			AArenaActor* Actor = static_cast<AArenaActor*>(obj->actor);
			obj->name = StringCast<char>(*Actor->GetName()).Get();
		}

		string Json = RigWorld->SaveToJson();
		FString FilePath = FPaths::ProjectContentDir() + L"Maps/" + GetWorld()->GetMapName() + L".json";
		FFileHelper::SaveArrayToFile(MakeArrayView((uint8*)Json.data(), Json.size()), *FilePath);
	}
}

void AArenaSettings::PostLoad()
{
	Super::PostLoad();
}