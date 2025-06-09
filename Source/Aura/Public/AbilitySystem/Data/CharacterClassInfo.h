// Copyright Patrick Haubner

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayEffect;


// This enum defines the different character classes available in the game.
UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	Elementalist UMETA(DisplayName = "Elementalist"),
	Warrior UMETA(DisplayName = "Warrior"),
	Ranger UMETA(DisplayName = "Ranger")

};

// This struct is used to define the default attributes for each character class.
USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributeEffect; // Effect that grants the primary attribute

};


/**
 * 
 */
 // This class holds the common class defaults for all character classes.
UCLASS()
class AURA_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// This function retrieves the default info for a given character class.
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation;

	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributeEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributeEffect;

	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass);
};
