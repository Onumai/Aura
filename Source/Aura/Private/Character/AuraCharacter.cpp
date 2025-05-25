// Copyright Patrick Haubner


#include "Character/AuraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Player/AuraPlayerController.h"
#include <UI/HUD/AuraHUD.h>

AAuraCharacter::AAuraCharacter()
{
	// Set the default values for the character's movement component
	GetCharacterMovement()->bOrientRotationToMovement = true; // Allow the character to rotate towards the direction of movement
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f); // Set the rotation rate for the character
	GetCharacterMovement()->bConstrainToPlane = true; // Constrain the character to a plane
	GetCharacterMovement()->bSnapToPlaneAtStart = true; // Snap the character to the plane at the start

	// Set the character's rotation settings
	bUseControllerRotationPitch = false; // Disable controller rotation for pitch
	bUseControllerRotationYaw = false; // Disable controller rotation for yaw	
	bUseControllerRotationRoll = false; // Disable controller rotation for roll

}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController); 
	
	//Init ability actor info for the Server
	InitAbilityActorInfo();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//Init ability actor info for the Client
	InitAbilityActorInfo();
	
}

int32 AAuraCharacter::GetPlayerLevel()
{
	const AAuraPlayerState * AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState); // Ensure that the player state is valid
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent(); // Get the ability system component from the player state
	AttributeSet = AuraPlayerState->GetAttributeSet(); // Get the attribute set from the player state

	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
	InitializeDefaultAttributes();
}
