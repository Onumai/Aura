// Copyright Patrick Haubner


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult, bool& bMouseDataWasStored)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
		bMouseDataWasStored = true;
	}
	else
	{
		//CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		bMouseDataWasStored = false;
	}
}

void UAuraBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

/**
 * Traces and identifies the first valid target impacted by the beam.
 *
 * This function performs a sphere trace starting from the weapon's tip socket to the provided target location.
 * It checks for a blocking hit and stores information about the hit location and the impacted actor.
 * If the hit actor implements the combat interface, it binds a delegate to handle the actor's death.
 *
 * @param BeamTargetLocation The target location where the beam trace will be directed.
 */
void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	//check (OwnerCharacter);

	if (!OwnerCharacter)
	{
		StoreOwnerVariables();
	}

	if (!OwnerCharacter)
	{
		// Defensive: Ability abbrechen statt harter check() Assertion
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	
	if (OwnerCharacter->Implements<UCombatInterface>())
	{
		if (USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter))
		{
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(OwnerCharacter); // Ignores the Actor that cast the spell 
			FHitResult HitResult;
			const FVector SocketLocation = Weapon->GetSocketLocation(FName("TipSocket")); // Traces from the tip socket
			UKismetSystemLibrary::SphereTraceSingle(
				OwnerCharacter,
				SocketLocation,
				BeamTargetLocation,
				10.f,
				TraceTypeQuery1,
				false,
				ActorsToIgnore,
				EDrawDebugTrace::None,
				HitResult,
				true);

			if (HitResult.bBlockingHit) // If bBlockingHit is true, we get the ImpactPoint and the actor that blocked the trace
			{
				MouseHitLocation = HitResult.ImpactPoint;
				MouseHitActor = HitResult.GetActor();
			}
		}
	}
	if (!IsValid(MouseHitActor)) return;
	
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MouseHitActor))
	{
		if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::PrimaryTargetDied))
		{
			CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::PrimaryTargetDied);
		}
	}
}

void UAuraBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets)
{
	if (!IsValid(MouseHitActor))
	{
		OutAdditionalTargets.Reset();
		return;
	}
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo()); //Executing Actor	
	ActorsToIgnore.Add(MouseHitActor); //Actor that already got hit
	
	TArray<AActor*> OverlappingActors;
	float Radius = 850.f;
	UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
		GetAvatarActorFromActorInfo(),
		OverlappingActors,
		ActorsToIgnore,
		Radius,
		MouseHitActor->GetActorLocation());
	
	//int32 NumAdditionalTargets = FMath::Min(GetAbilityLevel() - 1, MaxNumShockTargets);
	int32 NumAdditionalTargets = 5;

	UAuraAbilitySystemLibrary::GetClosestTargets(
		NumAdditionalTargets,
		OverlappingActors,
		OutAdditionalTargets,
		MouseHitActor->GetActorLocation());
	
	for (AActor* Target : OutAdditionalTargets)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Target))
		{
			if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::AdditionalTargetDied))
			{
				CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::AdditionalTargetDied);
			}
		}
	}
}

void UAuraBeamSpell::RemoveOnDeathBindingFromPrimaryTarget(AActor* Target)
{
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(MouseHitActor);
	if (CombatInterface)
	{
		CombatInterface->GetOnDeathDelegate().RemoveDynamic(this, &UAuraBeamSpell::PrimaryTargetDied);
	}
}

void UAuraBeamSpell::RemoveOnDeathBindingFromAdditionalTargets(AActor* AdditionalTarget)
{
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(AdditionalTarget);
	if (CombatInterface)
	{
		CombatInterface->GetOnDeathDelegate().RemoveDynamic(this, &UAuraBeamSpell::AdditionalTargetDied);
	}
}
