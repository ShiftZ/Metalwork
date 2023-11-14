#include "ArenaSettings.h"
#include "Arena.h"
#include "Misc/FileHelper.h"

void AArenaSettings::PostInitProperties()
{
	Super::PostInitProperties();
	Rig = Arena::MakeWorld();
}

void AArenaSettings::PostLoad()
{
	Super::PostLoad();
}

FCriticalSection MyCriticalSection;

void AArenaSettings::Serialize(FArchive& Ar)
{
	AInfo::Serialize(Ar);

	if (Ar.IsLoading())
	{
		FString FilePath = FPaths::ProjectContentDir() + L"Maps/" + GetWorld()->GetMapName() + L".json";
		TUniquePtr<FArchive> Reader(IFileManager::Get().CreateFileReader(*FilePath));
		if (Reader)
		{
			string Json(Reader->TotalSize(), 0);
			Reader->Serialize(Json.data(), Json.size());
			Rig->LoadFromJson(Json);
		}
	}
	else if (Ar.IsSaving() && Ar.ContainsMap())
	{
		string Json = Rig->SaveToJson();
		FString FilePath = FPaths::ProjectContentDir() + L"Maps/" + GetWorld()->GetMapName() + L".json";
		FFileHelper::SaveArrayToFile(MakeArrayView((uint8*)Json.data(), Json.size()), *FilePath);
	}

}