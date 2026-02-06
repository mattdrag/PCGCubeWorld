// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core/ZX.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ZXAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class UZXAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	//
	// Attributes Declarations:
	//

	/** Current Health */
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing=OnRep_Health)
	FGameplayAttributeData Health = 10.f;
	ATTRIBUTE_ACCESSORS(UZXAttributeSet, Health)

	/** Max Health  */
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing=OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth = 10.f;
	ATTRIBUTE_ACCESSORS(UZXAttributeSet, MaxHealth)

	/** AttackPower of the attacker is multiplied by the base Damage to reduce health, so 1.0 means no bonus */
	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing=OnRep_AttackPower)
	FGameplayAttributeData AttackPower = 1.f;
	ATTRIBUTE_ACCESSORS(UZXAttributeSet, AttackPower)

	/** Armor (percentage based phys defense) */
	UPROPERTY(BlueprintReadOnly, Category = "Defense", ReplicatedUsing=OnRep_Armor)
	FGameplayAttributeData Armor = 0.f;
	ATTRIBUTE_ACCESSORS(UZXAttributeSet, Armor)
	
	/** Damage is a 'temporary' attribute used by the DamageExecution to calculate final damage, which then turns into -Health */
	UPROPERTY(BlueprintReadOnly, Category = "Temp")
	FGameplayAttributeData Damage = 0.f;
	ATTRIBUTE_ACCESSORS(UZXAttributeSet, Damage)

protected:
	// Attributes:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	// Executions:
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	/** Helper function to proportionally adjust the value of an attribute when it's associated max attribute changes. (i.e. When MaxHealth increases, Health increases by an amount that maintains the same percentage as before) */
	void AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);

	//
	// Replication stuff:
	//
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_AttackPower(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Armor(const FGameplayAttributeData& OldValue);
};