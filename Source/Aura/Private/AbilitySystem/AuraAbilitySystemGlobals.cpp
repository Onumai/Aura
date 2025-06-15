// Copyright Patrick Haubner


#include "AbilitySystem/AuraAbilitySystemGlobals.h"
#include "Aura/Public/AuraAbilityTypes.h"

FGameplayEffectContext* UAuraAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FAuraGameplayEffectContext();
}
