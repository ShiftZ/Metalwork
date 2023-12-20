#include "ArenaSettings.h"
#include "ArenaActor.h"
#include "Arena.h"

void AArenaSettings::PostInitProperties()
{
	Super::PostInitProperties();
	if (GetFlags() & RF_ClassDefaultObject) return;

	RigWorld.Reset(Arena::MakeWorld().release());
}

void AArenaSettings::Serialize(FArchive& Ar)
{
	AInfo::Serialize(Ar);

	if (Ar.IsSaving())
	{
		for (RigidObject* Obj : RigWorld->objects | cptr)
		{
			AArenaActor* Actor = static_cast<AArenaActor*>(Obj->actor);
			Obj->name = StringCast<char>(*Actor->GetName()).Get();
		}

		if (Ar.GetLinker())
		{
			string JsonWorld = RigWorld->SaveToJson();
			FString FilePath = FPaths::ProjectContentDir() + L"Maps/" + GetWorld()->GetMapName() + L".json";
			FFileHelper::SaveArrayToFile(MakeArrayView((uint8*)JsonWorld.data(), JsonWorld.size()), *FilePath);
		}
		else
		{
			string JsonWorld = RigWorld->SaveToJson();
			int Size = JsonWorld.size();
			Ar << Size;
			Ar.Serialize(JsonWorld.data(), Size);
		}
	}
	else if (Ar.IsLoading())
	{
		if (Ar.GetLinker())
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
		else
		{
			int Size;
			Ar << Size;
			string Json(Size, 0);
			Ar.Serialize(Json.data(), Size);
			RigWorld->LoadFromJson(Json);
		}
	}
}

void AArenaSettings::PostLoad()
{
	Super::PostLoad();
	if (GetFlags() & RF_ClassDefaultObject) return;

	Drawer = MakeUnique<DebugDrawer>(GetWorld());
	RigWorld->SetDebugDrawer(Drawer.Get());
}