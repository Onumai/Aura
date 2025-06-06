// Copyright Patrick Haubner


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"
#include "AbilitySystemComponent.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		SendMouseCursorData();
	}
	else
	{
		//TODO: We are on the server, so listen for target data.
	}
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
	// ScopedPredictionWindow is used to ensure that the target data is sent with the correct prediction key
	// Prediction keys are used to ensure that the server and client are in sync
	FScopedPredictionWindow ScopedPridiction(AbilitySystemComponent.Get()); 

	// AbilityTask has a reference to the Ability, which has a reference to the PlayerController
	// So no header file is needed here
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();

	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	FVector ImpactPoint = CursorHit.ImpactPoint;
	
	FGameplayAbilityTargetDataHandle DataHandle;
	// Create a new target data object; new is necessary to allocate memory for it
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	DataHandle.Add(Data);

	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
