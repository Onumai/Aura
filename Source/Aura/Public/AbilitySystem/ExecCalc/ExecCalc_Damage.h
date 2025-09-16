// Copyright Patrick Haubner

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayEffectTypes.h"
#include "ExecCalc_Damage.generated.h"

/**
 * 
 */

class UAbilitySystemComponent;
class UCharacterClassInfo;
UCLASS()
class AURA_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:

	UExecCalc_Damage();
	void DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FGameplayEffectSpec& Spec, FAggregatorEvaluateParameters EvaluationParameters) const;

	static float ApplyDamageReductionByHaloOfProtection(float Damage,
												 const UAbilitySystemComponent* TargetASC,
												 const UCharacterClassInfo* TargetCharacterClassInfo);
	
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
