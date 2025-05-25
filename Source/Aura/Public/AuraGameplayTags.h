// Copyright Patrick Haubner

#pragma once

#include "GameplayTagContainer.h"
#include "CoreMinimal.h"

/**
 * AuraGameplayTags
 * 
 * Singelton containing native Gameplay Tags
 */

struct FAuraGameplayTags
{
	
public:
	static const FAuraGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();

	FGameplayTag Attributes_Secondary_Armor;

protected:

private:
	static FAuraGameplayTags GameplayTags;
};
