// Copyright Patrick Haubner

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouse.generated.h"

//Because CreateTargetDataUnderMouse is Async, we need to use a dynamic multicast delegate to allow binding in Blueprints as exceuction pins are not supported in Async tasks
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature, const FVector&, Data);

class AuraPlayerController;
/**
 * 
 */
UCLASS()
class AURA_API UTargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	// HidePin is used to avoid showing the OwningAbility pin in the Blueprint node and DefaultToSelf allows the node to automatically use the ability that owns it. "Self" in BP = "this" in C++
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "TargetDataUnderMouse", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UTargetDataUnderMouse* CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility);

	// BlueprintAssignable allows this delegate to be bound in Blueprints
	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData;

	//AuraPlayerController* PlayerController;

private:

	virtual void Activate() override;

};
