// Copyright Patrick Haubner


#include "Character/AuraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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
