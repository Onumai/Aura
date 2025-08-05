// Copyright Patrick Haubner


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();

	SpellPointsChangedDelegate.Broadcast(GetAuraPS()->GetSpellPoints());
	
}

/**
 * Binds callback functions to the necessary dependencies to handle ability status changes.
 *
 * This method connects a lambda function to the `AbilityStatusChanged` delegate in the
 * `UAuraAbilitySystemComponent`. When an ability's status changes, the lambda function retrieves
 * the updated ability information based on the provided `AbilityTag` and `StatusTag`, updates the status
 * tag, and broadcasts the updated ability information through the `AbilityInfoDelegate`.
 *
 * The function ensures that the ability information is dynamically updated and keeps the widget
 * synchronized with the current state of abilities.
 *
 * Preconditions:
 * - The `AbilityInfo` member must be initialized and contain valid ability information.
 * - The `GetAuraASC` method must return a valid instance of `UAuraAbilitySystemComponent`.
 *
 * Postconditions:
 * - Subscribers to `AbilityInfoDelegate` will receive updates whenever an ability’s status changes.
 */
void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraASC()->AbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
	{
		if (AbilityInfo)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			Info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	});
	
	GetAuraPS()->OnAttributePointsChangedDelegate.AddLambda(
		[this](int32 SpellPoints)
		{
			SpellPointsChangedDelegate.Broadcast(SpellPoints);
		}
	);
}
