// Copyright Patrick Haubner

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()
public:

	UPROPERTY()
	FString SlotName = FString(); // Necessary value, also important to be able to save slots with the same player name

	UPROPERTY()
	int32 SlotIndex = 0; // Necessary value

	UPROPERTY()
	FString PlayerName = FString("Default Name");
};
