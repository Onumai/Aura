// Copyright Patrick Haubner

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AttributeInfo.generated.h"


USTRUCT(BlueprintType)
struct FAuraAttributeInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AttributeTag = FGameplayTag(); // This is the tag that identifies the attribute, e.g. "Attribute.Strength"

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeName = FText(); // This is the name of the attribute, e.g. "Strength"

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeDescription = FText(); // This is the description of the attribute, e.g. "Strength increases your physical damage output."

	UPROPERTY(BlueprintReadOnly) // Not exposed to the editor, don't want to allow editing of this value
	float AttributeValue = 0.f; // This is the current value of the attribute, e.g. 10 for Strength
};

/**
 * 
 */
UCLASS()
class AURA_API UAttributeInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	FAuraAttributeInfo FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound = false) const; // This function finds the attribute info for a given tag. If bLogNotFound is true, it will log a warning if the tag is not found.
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAuraAttributeInfo> AttributeInformation; // This is the array of attributes, e.g. Strength, Intelligence, etc.>

};
