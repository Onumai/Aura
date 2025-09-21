// Copyright Patrick Haubner

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LoadScreenHUD.generated.h"

class UMVVM_LoadScreen;
class ULoadScreenWidget;
/**
 * 
 */
UCLASS()
class AURA_API ALoadScreenHUD : public AHUD
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, Category = "LoadScreen")
	TSubclassOf<UUserWidget> LoadScreenWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "LoadScreen")
	TObjectPtr<ULoadScreenWidget> LoadScreenWidget;

	UPROPERTY(EditDefaultsOnly, Category = "LoadScreen")
	TSubclassOf<UMVVM_LoadScreen> LoadScreenViewModelClass;

	UPROPERTY(BlueprintReadOnly, Category = "LoadScreen")
	TObjectPtr<UMVVM_LoadScreen> LoadScreenViewModel;
	
protected:
	virtual void BeginPlay() override;
};
