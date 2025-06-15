// Copyright Patrick Haubner


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor); 

	AuraDamageStatics()
	{
		// Shorthand versions of the capture definitions, in MMC we do it manually in the constructor
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false); // Capture the Armor attribute from the target
	}
	
};

static const AuraDamageStatics& DamageStatics() // This function is used to ensure that the static struct is initialized only once
{
	static AuraDamageStatics DStatics; // This will ensure that the static struct is initialized only once, even if this function is called multiple times
	return DStatics;
}


UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceActor = SourceASC ? SourceASC->GetAvatarActor() : nullptr; 
	AActor* TargetActor = TargetASC ? TargetASC->GetAvatarActor() : nullptr; 

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParamters;
	EvaluationParamters.SourceTags = SourceTags;
	EvaluationParamters.TargetTags = TargetTags;

	float Armor = 0.f;

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParamters, Armor);
	Armor = FMath::Max<float>(0.f, Armor); // Ensure Armor is not negative
	++Armor;

	const FGameplayModifierEvaluatedData EvaluatedData(DamageStatics().ArmorProperty, EGameplayModOp::Additive, Armor);

	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
