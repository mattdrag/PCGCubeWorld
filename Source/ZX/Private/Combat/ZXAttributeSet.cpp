// Fill out your copyright notice in the Description page of Project Settings.

#include "Combat/ZXAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Core/ZXUtils.h"

void UZXAttributeSet::AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && IsValid(AbilityComp))
	{
		// Change current value to maintain the current Val / Max percent
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		const float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UZXAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// This is called whenever attributes change, so for max health/mana we want to scale the current totals to match
	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
}

void UZXAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// NOTE: this applies to executions, not modifiers:

	// Context holds information about the effect:
	const FGameplayEffectContextHandle& Context = Data.EffectSpec.GetContext();
	
	// Source we get from Data's Context:
	AActor* SourceActor = Context.GetEffectCauser();
	
	// Store a local copy of the amount of damage done and clear the damage attribute
	float LocalDamageDone = FMath::Clamp(GetDamage(), 0.f, TNumericLimits<float>::Max());
	SetDamage(0.f);

	// Note: we can also do stuff with tags here. like different ability triggers, etc.

	// apply damage:
	const float NewHealth = FMath::Clamp(GetHealth() - LocalDamageDone, 0.0f, GetMaxHealth());
	SetHealth(NewHealth);
}

void UZXAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZXAttributeSet, Health, OldValue);
}

void UZXAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZXAttributeSet, MaxHealth, OldValue);
}

void UZXAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZXAttributeSet, AttackPower, OldValue);
}

void UZXAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZXAttributeSet, Armor, OldValue);
}

void UZXAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UZXAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UZXAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UZXAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UZXAttributeSet, Armor, COND_None, REPNOTIFY_Always);
}
