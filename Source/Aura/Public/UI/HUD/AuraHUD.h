// Copyright Patrick Haubner

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AuraHUD.generated.h"


class UAuraUserWidget;
class UOverlayWidgetController;
struct FWidgetControllerParams;
class UAttributeMenuWidgetController;
/**
 * 
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()

public:
	


	// Returns the OverlayWidgetController instance
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams); 

	// Initialize the overlay with the given parameters
	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS); 


	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams);


protected:
	

private:

	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass; // Widget class to be used for the overlay

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;


	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;
};
