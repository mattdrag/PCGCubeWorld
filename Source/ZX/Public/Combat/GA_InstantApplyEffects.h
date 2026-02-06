// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core/ZX.h"
#include "ZXGameplayAbility.h"
#include "GA_InstantApplyEffects.generated.h"

class UZXGameplayEffect;
/**
 * 
 */
UCLASS()
class ZX_API UGA_InstantApplyEffects : public UZXGameplayAbility
{
	GENERATED_BODY()

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

public:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UZXGameplayEffect>> EffectsToApply;
};
