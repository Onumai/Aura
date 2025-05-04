// Copyright Patrick Haubner


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include <EnhancedInputComponent.h>

AAuraPlayerController::AAuraPlayerController()
{
	// Enables replication for this property, ensuring it stays synchronized across server and clients.
	bReplicates = true;
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	check(AuraContext);
	// Create the Enhanced Input Subsystem and add the mapping context.
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);
	Subsystem->AddMappingContext(AuraContext, 0);

	//CursorSettings
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	// Set the input mode to game and UI to allow for mouse interaction with UI elements.
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// Check if the input component is of type UEnhancedInputComponent
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent); // CastChecked ensures that the cast is valid and will assert if it fails.
	
	//Bind the MoveAction to the Move function using the Enhanced Input Component.
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();// Get the input axis vector from the action value.
	const FRotator Rotation = GetControlRotation();// Get the control rotation of the player controller.
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f); // Create a new rotation with only the yaw component.
	// Calculate the forward direction based on the yaw rotation.
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);//GetUnitAxis returns a unit vector in the specified axis direction.
	// Calculate the right direction based on the yaw rotation.
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); 

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		// Add movement input to the controlled pawn based on the input axis vector and the calculated directions.
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y); 
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}
