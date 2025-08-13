// Copyright Patrick Haubner


#include "AuraAssetManager.h"

#include "AbilitySystemGlobals.h"
#include "AuraGameplayTags.h"

UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);

	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);

	return *AuraAssetManager;
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	//FAuraGameplayTags::InitializeNativeGameplayTags();

	// This is required to use Target Data in AbilityTasks!
	UAbilitySystemGlobals::Get().InitGlobalData();

}
