// Copyright Patrick Haubner


#include "AbilitySystem/Passive/PassiveSpellNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"

UPassiveSpellNiagaraComponent::UPassiveSpellNiagaraComponent()
{
	bAutoActivate = false;
}

void UPassiveSpellNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Look for ASC and listen for its Delegate Broadcast
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		AuraASC->ActivatePassiveEffect.AddUObject(this, &ThisClass::OnPassiveActivate);
		ActivateIfEquipped(AuraASC);
	}
	// If there is no ASC, get it from the CombatInterface and listen for its Delegate Broadcast
	else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		CombatInterface->GetOnASCRegisteredDelegate().AddLambda([this](UAbilitySystemComponent* ASC)
		{
			if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
			{
				ActivateIfEquipped(AuraASC);
			}
		});
	}
}

void UPassiveSpellNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
	if (AbilityTag.MatchesTagExact(PassiveSpellTag))
	{
		if (bActivate && !IsActive()) // IsActive() looks if the component is active
		{
			Activate();
		}
		else
		{
			Deactivate();
		}
	}
}

void UPassiveSpellNiagaraComponent::ActivateIfEquipped(UAuraAbilitySystemComponent* AuraASC)
{
	const bool bStartupAbilitiesGiven = AuraASC->bStartupAbilitiesGiven;
	if (bStartupAbilitiesGiven)
	{
		if (AuraASC->GetStatusFromAbilityTag(PassiveSpellTag) == FAuraGameplayTags::Get().Abilities_Status_Equipped)
		{
			Activate();
		}
	}
}
