// Copyright Patrick Haubner


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include <AbilitySystem/AuraAbilitySystemComponent.h>

#include "AuraGameplayTags.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetAuraAS()->GetHealth()); // Broadcast the initial health value
	OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth()); // Broadcast the initial max health value

	OnManaChanged.Broadcast(GetAuraAS()->GetMana()); // Broadcast the initial mana value
	OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana()); // Broadcast the initial max mana value
}

/**
 * Binds callbacks to attribute changes, player state changes, and ability system events.
 *
 * This function connects various delegate callbacks to track changes in player attributes,
 * experience points, and ability system effects. It ensures that UI elements are updated
 * dynamically in response to these events.
 *
 * Preconditions:
 * - PlayerState must be a valid instance of AAuraPlayerState.
 * - AttributeSet must be a valid instance of UAuraAttributeSet containing the required attribute data.
 * - AbilitySystemComponent must be a valid instance of UObject that supports GameplayAttributeValue change delegates.
 *
 * Behavior:
 * - Subscribes to the OnXPChangedDelegate of the AAuraPlayerState to track XP changes and calls the OnXPChanged function.
 * - Listens to health, max health, mana, and max mana attribute changes and broadcasts the new values.
 * - Initializes abilities immediately or subscribes to the AbilitiesGivenDelegate to initialize when abilities are granted.
 * - Processes gameplay effect tags and retrieves associated data table rows to broadcast relevant UI updates.
 * - Dynamically links messages associated with specific gameplay tags to the MessageWidgetRowDelegate.
 *
 * Broadcasts:
 * - OnHealthChanged: Signaling the current health value when it is updated.
 * - OnMaxHealthChanged: Signaling the updated maximum health value.
 * - OnManaChanged: Signaling the current mana value when it is updated.
 * - OnMaxManaChanged: Signaling the updated maximum mana value.
 * - MessageWidgetRowDelegate: Broadcasting data rows for gameplay tags with associated messages.
 *
 * Delegates and Methods:
 * - OnXPChanged: A method to handle changes in player XP.
 * - OnInitializeStartupAbilities: Invokes the initialization sequence for startup abilities when granted.
 *
 * Effect Tags:
 * - Processes gameplay effect tags, ensuring tags linked to "Message" initiate a UI update.
 */
void UOverlayWidgetController::BindCallbacksToDependencies()
{
	GetAuraPS()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	GetAuraPS()->OnLevelChangedDelegate.AddLambda(
	[this](int32 NewLevel, bool bLevelUp)
		{
		OnPlayerLevelChangedDelegate.Broadcast(NewLevel, bLevelUp);
		}
	);
	
	// Deleted the callback function and implemented the Broadcast into the AddLambda

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnManaChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetMaxManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxManaChanged.Broadcast(Data.NewValue);
			}
		);

	if (GetAuraASC())
	{
		GetAuraASC()->AbilityEquipped.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);
		if (GetAuraASC()->bStartupAbilitiesGiven)
		{
			BroadcastAbilityInfo();
		}
		else
		{
			GetAuraASC()->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
		}
		
		GetAuraASC()->EffectAssetTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
			{
				for (const FGameplayTag& Tag : AssetTags)
				{
					// For example, say that Tag = Message.HealthPotion
					// * "Message.HealthPotion".MatchesTag("Message") will return True, "Message".MatchesTag("Message.HealthPotion") will return False
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if (Tag.MatchesTag(MessageTag))
					{
						const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						MessageWidgetRowDelegate.Broadcast(*Row);
					}	
				}
			}
		);
	}
}


/**
 * Handles the event triggered when the player's XP changes.
 *
 * This function calculates the player's current level based on the given XP, the XP requirements
 * for the current and previous levels, and the remaining XP needed to level up. It then computes
 * and broadcasts the XP progress percentage for the current level to any listeners.
 *
 * Preconditions:
 * - The PlayerState must be valid and properly set.
 * - The AuraPlayerState must contain a valid reference to a ULevelUpInfo object to compute
 *   XP progression.
 * - LevelUpInfo in AuraPlayerState must have LevelUpInformation filled with valid level data.
 
 * Behavior:
 * - Determines the player's current level using the NewXP value.
 * - Validates the current level is within bounds of the level information.
 * - Computes the XP progress percentage for the current level based on the XP requirements.
 * - Broadcasts the computed XP progress percentage through the OnXPPercentChangedDelegate.
 
 * Broadcasts:
 * - OnXPPercentChangedDelegate: A delegate that signals the current XP progress percentage.
 *
 * @param NewXP The updated XP value for the player.
 */
void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	const ULevelUpInfo* LevelUpInfo = GetAuraPS()->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("Unable to find LevelUpInfo. Please fill out AuraPlayerState Blueprint!"));

	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();

	if (Level <= MaxLevel && Level > 0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level - 1].LevelUpRequirement;

		const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
		const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;

		const float XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelRequirement);

		OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
	}
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status,
	const FGameplayTag& Slot, const FGameplayTag& PreviousSlot) const
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PreviousSlot;
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;
	//Broadcast empty info if PreviousSlot is a valid slot. Only if equipping an already-equipped spell
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = Status;
	Info.InputTag = Slot;
	AbilityInfoDelegate.Broadcast(Info);
}


