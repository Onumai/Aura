// Copyright Patrick Haubner

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()
public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override; // Belongs to Character class in Engine 
	virtual void OnRep_PlayerState() override; 

	/* Combat Interface*/

	virtual int32 GetPlayerLevel() override;

	/* End Combat Interface*/
private:
	virtual void InitAbilityActorInfo() override; // Initialize the ability actor info for the character
};
