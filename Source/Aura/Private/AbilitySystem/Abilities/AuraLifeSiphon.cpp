// Copyright Patrick Haubner


#include "AbilitySystem/Abilities/AuraLifeSiphon.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"


FString UAuraLifeSiphon::GetDescription(int32 Level)
{
	const int32 DamageReduction = PassiveAbilityCurve.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	
	return FString::Printf(TEXT(
	// Title
	"<Title>Mana Siphon</>\n\n"
			
	// Level
	"<Small>Skill Level: </><Level>%d</>\n"
	// ManaCost
	"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
	// Cooldown
	"<Small>Cooldown: </><Cooldown>%.1f s</>\n\n"
	// Description 
		"<Default>Generates a protective halo around the character, "
		"reducing all types of incoming damage by</><ManaCost> %d%% </>\n\n"),

		// Values
		Level,
		ManaCost,
		Cooldown,
		DamageReduction);
}

FString UAuraLifeSiphon::GetNextLevelDescription(int32 Level)
{
	const int32 DamageReduction = PassiveAbilityCurve.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
	// Title
	"<Title>Next Level</>\n\n"
			
	// Level
	"<Small>Skill Level: </><Level>%d</>\n"
	// ManaCost
	"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
	// Cooldown
	"<Small>Cooldown: </><Cooldown>%.1f s</>\n\n"
	// Description 
		"<Default>Generates a protective halo around the character, "
		"reducing all types of incoming damage by</><ManaCost> %d%% </>\n\n"),

		// Values
		Level,
		ManaCost,
		Cooldown,
		DamageReduction);
}

void UAuraLifeSiphon::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo())))
	{
		AuraASC->AddLooseGameplayTag(FAuraGameplayTags::Get().Abilities_Passive_LifeSiphon);
	}
}

void UAuraLifeSiphon::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo())))
	{
		AuraASC->RemoveLooseGameplayTag(FAuraGameplayTags::Get().Abilities_Passive_LifeSiphon);
	}
}
