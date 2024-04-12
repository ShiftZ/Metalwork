#include "ArenaEvents.h"

void UEffectEvent::Denied()
{
	if (Component.IsValid()) Component->DestroyComponent();
	StreamHandle->CancelHandle();
}

UEffectEvent* UEffectEvent::MakeSound(UWorld* World, TSoftObjectPtr<UMetaSoundSource> Sound,
												  FVector Location, TOptional<TPair<FName, float>> Params)
{
	UEffectEvent* Event = NewObject<UEffectEvent>();

	auto Play = FStreamableDelegate::CreateLambda([=, Event = TWeakObjectPtr<UEffectEvent>(Event)]
	{
		UAudioComponent* Component = UGameplayStatics::SpawnSoundAtLocation(World, Sound.Get(), Location);
		if (Params) Component->SetFloatParameter(Params->Key, Params->Value);
		Component->Play();

		if (Event.IsValid())
			Event->Component = Component;
	});

	Event->StreamHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(Sound.ToSoftObjectPath(), Play, 100);
	return Event;
}

UEffectEvent* UEffectEvent::MakeVFX(UWorld* World, TSoftObjectPtr<UNiagaraSystem> Template, FVector Location)
{
	UEffectEvent* Event = NewObject<UEffectEvent>();

	auto Play = FStreamableDelegate::CreateLambda([=, Event = TWeakObjectPtr<UEffectEvent>(Event)]
	{
		UNiagaraComponent* Component = UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, Template.Get(), Location);
		if (Event.IsValid()) 
			Event->Component = Component;
	});

	Event->StreamHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(Template.ToSoftObjectPath(), Play, 100);
	return Event;
}
