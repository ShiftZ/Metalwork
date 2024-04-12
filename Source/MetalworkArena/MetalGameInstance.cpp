#include "MetalGameInstance.h"

void UMetalGameInstance::Init()
{
	Super::Init();

	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

	// Niagara subsystem components
	FARFilter NiagaraFilter;
	NiagaraFilter.ClassPaths.Add(UNiagaraSystem::StaticClass()->GetClassPathName());

	TArray<FAssetData> NiagaraAssets;
	AssetRegistry.GetAssets(NiagaraFilter, NiagaraAssets);

	TSet<FSoftObjectPath> NiagaraComponents;

	for (const FAssetData& Asset : NiagaraAssets)
	{
		FString PackageName = Asset.PackageName.ToString();
		if (PackageName.StartsWith(L"/Game/"))
		{
			TArray<FAssetIdentifier> Dependencies;
			AssetRegistry.GetDependencies(Asset.PackageName, Dependencies);
			for (FAssetIdentifier& Dependency : Dependencies)
			{
				if (Dependency.ToString().StartsWith(L"/Niagara"))
					NiagaraComponents.Add({Dependency.PackageName, Dependency.ObjectName, {}});
			}
		}
	}

	TArray<FSoftObjectPath> ComponentsArray = NiagaraComponents.Array();

	auto Handler = FStreamableDelegate::CreateLambda([this, Components = MoveTemp(NiagaraComponents)]
	{
		for (const FSoftObjectPath& ComponentPath : Components)
			PreloadedAssets.Add(ComponentPath.TryLoad());
	});

	UAssetManager::GetStreamableManager().RequestAsyncLoad(MoveTemp(ComponentsArray), Handler, -100);
}
