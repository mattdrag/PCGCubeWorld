// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/DebugDamageExecution.h"

#include "Combat/ZXAttributeSet.h"

struct DebugDamageStatics
{
	// Source
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);

	// locals:
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);

	// Target
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);

	DebugDamageStatics()
	{
		/////////// Source ///////////
		// Capture the Source's AttackPower. We do want to snapshot this at the moment we create the GameplayEffectSpec that will execute the damage.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UZXAttributeSet, AttackPower, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UZXAttributeSet, Damage, Source, true);

		//////////// Target ////////////
		// Capture the Target's Armor attribute. Do not snapshot it, because we want to use the health value at the moment we apply the execution.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UZXAttributeSet, Armor, Target, false);
	}
};

static const DebugDamageStatics& DbgDamageStatics()
{
	static DebugDamageStatics DmgStatics;
	return DmgStatics;
}

UDebugDamageExecution::UDebugDamageExecution()
{
	RelevantAttributesToCapture.Add(DbgDamageStatics().AttackPowerDef);
	RelevantAttributesToCapture.Add(DbgDamageStatics().DamageDef);
	RelevantAttributesToCapture.Add(DbgDamageStatics().ArmorDef);
}

void UDebugDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	const UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();
	
	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor() : nullptr;

	// TODO: if you ever need to use these, you should first check nullptr
	
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags(); 
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// Capture Attributes:
	float AttackPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DbgDamageStatics().AttackPowerDef, EvaluationParameters, AttackPower);

	float Damage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DbgDamageStatics().DamageDef, EvaluationParameters, Damage);
	
	float Armor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DbgDamageStatics().ArmorDef, EvaluationParameters, Armor);

	// --------------------------------------------
	//	Damage Done = Damage + AttackPower - Armor
	// --------------------------------------------
	
	const float DamageCalc = Damage + AttackPower - Armor;
	
	// This is what actually applies the damage:
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DbgDamageStatics().DamageProperty, EGameplayModOp::Additive, DamageCalc));

	// Combat Log:
	UE_LOG(LogZX, Log, TEXT("%s did %f damage to %s.."), IsValid(SourceActor) ? *SourceActor->GetName() : *FString("someone"), DamageCalc, IsValid(TargetActor) ? *TargetActor->GetName() : *FString("someone"));
}
