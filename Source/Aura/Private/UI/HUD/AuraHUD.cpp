// Copyright Patrick Haubner


#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "GameFramework/PlayerState.h"  
#include "AbilitySystemComponent.h"  
#include "AttributeSet.h" 


UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass); 
		OverlayWidgetController->SetWidgetControllerParams(WCParams);

		return OverlayWidgetController;
	}


	return OverlayWidgetController;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{	
	// checkf is a macro that checks if the condition is true, and if not, it will log an error message and crash the game.
	checkf(OverlayWidgetClass, TEXT("OverlayWidgetClass uninitialized, please fill out BP_AuraHUD")); 
	checkf(OverlayWidgetControllerClass, TEXT("OverlayWidgetControllerClass uninitialized, please fill out BP_AuraHUD"));

	// Constructed OverlayWidget
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass); // Create the widget instance
	OverlayWidget = Cast<UAuraUserWidget>(Widget); // Cast the widget to UAuraUserWidget 
	
	// Constructed OverlayWidgetController
	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS); // Create the widget controller parameters
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams); // Get the overlay widget controller

	OverlayWidget->SetWidgetController(WidgetController); // Set the widget controller in the overlay widget

	Widget->AddToViewport();
}


