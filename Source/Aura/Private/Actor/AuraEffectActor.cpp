// Copyright Patrick Haubner


#include "Actor/AuraEffectActor.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"


AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));

}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr) return;

	check(GameplayEffectClass);
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.f, EffectContextHandle);
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)  
{  
   if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap &&  
       InstantGameplayEffectClasses.Num() != 0)  
   {  
       for (TSubclassOf<UGameplayEffect>& GameplayEffectClass : InstantGameplayEffectClasses)  
       {  
           ApplyEffectToTarget(TargetActor, GameplayEffectClass);  
       }  
   }  

   if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap &&
	   DurationGameplayEffectClasses.Num() != 0)
   {
	   for (TSubclassOf<UGameplayEffect>& GameplayEffectClass : DurationGameplayEffectClasses)
	   {
		   ApplyEffectToTarget(TargetActor, GameplayEffectClass);
	   }
   }

   if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap &&
	   InfiniteGameplayEffectClasses.Num() != 0)
   {
	   for (TSubclassOf<UGameplayEffect>& GameplayEffectClass : InfiniteGameplayEffectClasses)
	   {
		   ApplyEffectToTarget(TargetActor, GameplayEffectClass);
	   }
   } 
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap &&
		InstantGameplayEffectClasses.Num() != 0)
	{
		for (TSubclassOf<UGameplayEffect>& GameplayEffectClass : InstantGameplayEffectClasses)
		{
			ApplyEffectToTarget(TargetActor, GameplayEffectClass);
		}
	}

	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap &&
		DurationGameplayEffectClasses.Num() != 0)
	{
		for (TSubclassOf<UGameplayEffect>& GameplayEffectClass : DurationGameplayEffectClasses)
		{
			ApplyEffectToTarget(TargetActor, GameplayEffectClass);
		}
	}

	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap &&
		InfiniteGameplayEffectClasses.Num() != 0)
	{
		for (TSubclassOf<UGameplayEffect>& GameplayEffectClass : InfiniteGameplayEffectClasses)
		{
			ApplyEffectToTarget(TargetActor, GameplayEffectClass);
		}
	}

	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) return;


		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		for (TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
		{
			if (TargetASC == HandlePair.Value)
			{
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
				HandlesToRemove.Add(HandlePair.Key);
			}
		}
		for(FActiveGameplayEffectHandle& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
}
