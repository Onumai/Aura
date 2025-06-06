// Copyright Patrick Haubner


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
    // AbilityTask has a reference to the Ability, which has a reference to the PlayerController
	// So no header file is needed here
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get(); 

    FHitResult CursorHit;
    PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
    FVector ImpactPoint = CursorHit.ImpactPoint;
    ValidData.Broadcast(ImpactPoint);
}
