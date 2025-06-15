// Copyright Patrick Haubner


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor); 
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);

	AuraDamageStatics()
	{
		// Shorthand versions of the capture definitions, in MMC we do it manually in the constructor
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false); // Capture the Armor attribute from the target
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, true); // Capture the ArmorPenetration attribute from the source
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false); // Capture the BlockChance attribute from the target
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
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
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

	// Get Damage Set by Caller Magnitude
	float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);


	// Capture Block Chance on Target, and determine if there was a successful block

	float TargetBlockChance = 0.f;
	// Getting the BlockChance from the target's attribute set
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParamters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(0, TargetBlockChance); // Ensure BlockChance is not negative
	
	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance; // Determine if the damage is blocked based on the BlockChance
	
	// If Block, halve the damage
	Damage = bBlocked ? Damage / 2.f : Damage; // If blocked, halve the damage
	

	
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParamters, TargetArmor);
	TargetArmor = FMath::Max<float>(0, TargetArmor);

	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParamters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(0, SourceArmorPenetration);

	// ArmorPenetration ignores a percentage of the target's armor
	const float EffectiveArmor = TargetArmor *= ( 100 - SourceArmorPenetration * 0.25f ) / 100.f; // 4 points of ArmorPenetration reduces the target's armor by 1%
	// Armor ignores a percentage of incoming damage
	Damage *= ( 100 - EffectiveArmor * 0.333f) / 100.f;





	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);

	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
