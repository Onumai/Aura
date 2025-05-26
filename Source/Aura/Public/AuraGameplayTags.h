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

	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;

	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegenaration;
	FGameplayTag Attributes_Secondary_ManaRegenaration;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;
protected:

private:
	static FAuraGameplayTags GameplayTags;
};
