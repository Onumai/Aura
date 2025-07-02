// Copyright Patrick Haubner


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Interaction/EnemyInterface.h"
#include "Input/AuraInputComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "AuraGameplayTags.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Aura/Aura.h"
#include "Gameframework/Character.h"
#include "GameFramework/HUD.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/Widget/DamageTextComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	// Enables replication for this property, ensuring it stays synchronized across server and clients.
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	
	CursorTrace();
	AutoRun();

}

void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageText  = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass); 
		DamageText->RegisterComponent(); // Register the component to make it active in the world.
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform); // Attach the damage text to the character's root component.
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform); // Detach it to allow it to move freely in the world.
		DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit); // Set the damage text to be displayed.
	}
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	
    APawn* ControlledPawn = GetPawn();
    if (ControlledPawn)
    {
       const FVector PawnLocation = ControlledPawn->GetActorLocation();
       const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(PawnLocation, ESplineCoordinateSpace::World);
       const FVector TargetSplinePointLocation = Spline->GetLocationAtSplinePoint(TargetSplinePointIdx, ESplineCoordinateSpace::World);
       FVector WorldDirection = TargetSplinePointLocation - PawnLocation;
       WorldDirection.Z = 0.0f;
       // NOTE: We get the normal after zeroing Z to get a constant movement speed along the XY plane.
       WorldDirection = WorldDirection.GetSafeNormal();
       ControlledPawn->AddMovementInput(WorldDirection);

       const float DistanceToTarget = (LocationOnSpline - TargetSplinePointLocation).Length();
       if (DistanceToTarget <= AutoRunAcceptanceRadius)
       {
          const bool bNextTargetPointExist = TargetSplinePointIdx < Spline->GetNumberOfSplinePoints() - 1;
          if (bNextTargetPointExist)
          {
             TargetSplinePointIdx++;
          }
          else
          {
             bAutoRunning = false;
          }
       }

       if (bDrawDebugEnabled)
       {
          for (int32 SplinePointIdx = 0; SplinePointIdx < Spline->GetNumberOfSplinePoints(); ++SplinePointIdx)
          {
             const FVector SplinePointLocation = Spline->GetLocationAtSplinePoint(SplinePointIdx, ESplineCoordinateSpace::World);
             if (SplinePointIdx > 0)
             {
                const FVector PreviousSplinePointLocation = Spline->GetLocationAtSplinePoint(SplinePointIdx - 1, ESplineCoordinateSpace::World);
                DrawDebugLine(GetWorld(), PreviousSplinePointLocation, SplinePointLocation, FColor::Red);
             }
             DrawDebugSphere(GetWorld(), SplinePointLocation, 10.0f, 12, FColor::Red);
          }
          DrawDebugSphere(GetWorld(), LocationOnSpline, 20.0f, 12, FColor::Cyan);

          const FVector LineStart = PawnLocation + WorldDirection.GetSafeNormal() * 50.0f;
          const FVector LineEnd = LineStart + WorldDirection * 100.0f;
          UKismetSystemLibrary::DrawDebugArrow(this, LineStart, LineEnd, 20.0f, FLinearColor::Yellow, 0.0f, 4);

          DrawDebugSphere(GetWorld(), TargetSplinePointLocation, 20.0f, 12, FColor::Yellow);

          UE_LOG(LogTemp,Warning, TEXT("TargetSplinePointIdx: %i, DistanceToTarget: %f"), TargetSplinePointIdx, DistanceToTarget);
       }
    }
}


bool AAuraPlayerController::GetCursorPlaneIntersection(const FVector& InPlaneOrigin, const FVector& InPlaneNormal,
	FVector& OutPlanePoint) const
{
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer && LocalPlayer->ViewportClient)
	{
		FVector2D MousePosition;
		const bool bMousePositionFound = LocalPlayer->ViewportClient->GetMousePosition(MousePosition);
		if (bMousePositionFound)
		{
			return GetScreenPositionPlaneIntersection(MousePosition, InPlaneOrigin, InPlaneNormal, OutPlanePoint);
		}
	}
	return false;
}

bool AAuraPlayerController::GetScreenPositionPlaneIntersection(const FVector2D& ScreenPosition,
	const FVector& InPlaneOrigin, const FVector& InPlaneNormal, FVector& OutPlanePoint) const
{
	AHUD* HUD = GetHUD();
	if (HUD && HUD->GetHitBoxAtCoordinates(ScreenPosition, true))
	{
		return false;
	}

	FVector WorldOrigin;
	FVector WorldDirection;
	const bool bScreenPositionDeprojected = UGameplayStatics::DeprojectScreenToWorld(this, ScreenPosition,
		WorldOrigin, WorldDirection);
	if (bScreenPositionDeprojected)
	{
		OutPlanePoint = FMath::LinePlaneIntersection(WorldOrigin, WorldOrigin + WorldDirection * HitResultTraceDistance,
			InPlaneOrigin, InPlaneNormal);
		return true;
	}
	return false;
}

void AAuraPlayerController::CursorTrace()
{
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit); // Perform a trace to get the hit result under the cursor.
	if (!CursorHit.bBlockingHit) return; // If there is no hit, return early.

	LastActor = ThisActor; // Store the last actor that was hit.
	ThisActor = CursorHit.GetActor(); // Store the current actor that was hit.
	//Cast not needed here, as ThisActor is already of type TScriptInterface<IEnemyInterface>.

	if (LastActor != ThisActor)
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (ThisActor) ThisActor->HighlightActor();
	}

	
	/*
	* Line trace form cursor. There are several scenarios:
	* A. LastActor is null && ThisActor is null: No actor that implemented that interface was hit.
	*	-> Do nothing.
	* B. LastActor is null && ThisActor is valid: A new actor that implemented that interface was hit.
	*	-> Call HighlightActor on ThisActor.
	* C. LastActor is valid && ThisActor is null: The last actor that implemented that interface was hit, but the cursor is now over empty space.
	*	->	Call UnHighlightActor on LastActor.
	* D. Both actors are valid, but LastActor != ThisActor: The cursor is now over a different actor that implemented that interface.
	*	->	Call UnHighlightActor on LastActor and HighlightActor on ThisActor.
	* E. Both actors are valid, and are the same actor: The cursor is still over the same actor that implemented that interface.
	*  ->	Do nothing.
	*/
	/*
	if (LastActor == nullptr)
	{
		if (ThisActor != nullptr)
		{
			//Case B
			ThisActor->HighlightActor();
		}
		else
		{
			//Case A - both are null, do nothing
		}
	}
	else // LastActor is valid
	{
		if (ThisActor == nullptr)
		{
			//Case C
			LastActor->UnHighlightActor();
		}
		else // Both actor are valid
		{
			if (LastActor != ThisActor)
			{
				//Case D
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
			else
			{
				//Case E - Do nothing
			}
		}
	}*/

}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = ThisActor ? true : false;
		bAutoRunning = false;
		ControlledPawnHalfHeight = Cast<ICombatInterface>(GetPawn())->GetHalfHeight();
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
		return;
	}

	if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);

	if (!bTargeting && !bShiftKeyDown)
	{
		if (FollowTime <= ShortPressThreshold)
		{
			const APawn* ControlledPawn = GetPawn();
			if (ControlledPawn)
			{
				FHitResult NavChannelCursorHitResult;
				GetHitResultUnderCursor(ECC_Navigation, false, NavChannelCursorHitResult);
				if (NavChannelCursorHitResult.bBlockingHit)
				{
					// Projecting a point from the cursor impact point to the NavMesh with a larger-than-default
					// Query Extent, so there are better chances to reach for the NavMesh and return a point,
					// then generating a path from the pawn location to this point (only if found).

					FNavLocation ImpactPointNavLocation;
					// NOTE: Default Query Extend = FVector(50.0f, 50.0f, 250.0f)
					const FVector QueryingExtent = FVector(400.0f, 400.0f, 250.0f);
					const FNavAgentProperties& NavAgentProps = GetNavAgentPropertiesRef();
					const bool bNavLocationFound = NavSystem->ProjectPointToNavigation(NavChannelCursorHitResult.ImpactPoint, ImpactPointNavLocation, QueryingExtent, &NavAgentProps);
					if (bNavLocationFound)
					{
						UNavigationPath* NavigationPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), ImpactPointNavLocation);
						if (NavigationPath && NavigationPath->PathPoints.Num() > 0)
						{
							Spline->ClearSplinePoints();
							for (const FVector& PathPoint : NavigationPath->PathPoints)
							{
								Spline->AddSplinePoint(PathPoint, ESplineCoordinateSpace::World);
							}
							CachedDestination = NavigationPath->PathPoints.Last();
							bAutoRunning = true;
						}
					}

					if (bDrawDebugEnabled)
					{
						DrawDebugBox(GetWorld(), NavChannelCursorHitResult.ImpactPoint, QueryingExtent, FColor::Silver, false, 3.0f);
					}
				}
			}
		}
		FollowTime = 0.0f;
		bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
		return;
	}

	if (bTargeting || bShiftKeyDown)
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
	}
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();

		APawn* ControlledPawn = GetPawn();
		if (ControlledPawn)
		{
			const FVector PawnLocation = ControlledPawn->GetActorLocation();
			FVector PawnBottomLocation = PawnLocation;
			PawnBottomLocation.Z -= ControlledPawnHalfHeight;
			FVector CursorHorizPlaneIntersection;
			const bool bIntersectionFound = GetCursorPlaneIntersection(PawnBottomLocation, FVector::UpVector, CursorHorizPlaneIntersection);
			if (bIntersectionFound)
			{
				FVector WorldDirection = (CursorHorizPlaneIntersection - PawnLocation).GetSafeNormal();
				WorldDirection.Z = 0.0f;
				ControlledPawn->AddMovementInput(WorldDirection);
				if (bDrawDebugEnabled)
				{
					DrawDebugSphere(GetWorld(), CursorHorizPlaneIntersection, 20.0f, 12, FColor::Green);

					const FVector LineStart = PawnLocation + WorldDirection.GetSafeNormal() * 50.0f;
					const FVector LineEnd = LineStart + WorldDirection * 100.0f;
					UKismetSystemLibrary::DrawDebugArrow(this, LineStart, LineEnd, 20.0f, FLinearColor::Green, 0.0f, 4);        }
			}
		}
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySytemComponent == nullptr)
	{
		AuraAbilitySytemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySytemComponent;
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	check(AuraContext);
	// Create the Enhanced Input Subsystem and add the mapping context.
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem) // if check for multiplayer support
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}
	//CursorSettings
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	// Set the input mode to game and UI to allow for mouse interaction with UI elements.
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);

	NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// Check if the input component is of type UEnhancedInputComponent
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent); // CastChecked ensures that the cast is valid and will assert if it fails.
	
	//Bind the MoveAction to the Move function using the Enhanced Input Component.
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
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
	bAutoRunning = false;
}


