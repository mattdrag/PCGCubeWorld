// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/GA_InstantApplyEffects.h"

#include "Combat/ZXGameplayEffect.h"
#include "Pawn/CombatPawn.h"

void UGA_InstantApplyEffects::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (TriggerEventData == nullptr || !IsValid(TriggerEventData->Target))
	{
		LOGZXEF("Invalid event data..");
		return;
	}
	
	// Get src and tar
	const ACombatPawn* Owner = Cast<ACombatPawn>(GetOwningActorFromActorInfo());
	const ACombatPawn* Target = Cast<ACombatPawn>(TriggerEventData->Target);

	// TODO: if you use Owner/Target, null check
	
	for (const TSubclassOf<UGameplayEffect> EffectClass : EffectsToApply)
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(EffectClass);

		// md-note: here we will set setbycaller magnitudes, which should come in through event data or something. dw rn.
		/*
		if (EffectSpecHandle.IsValid())
		{
			EffectSpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Effect.Damage")), 50.0f);
		}
		*/

		// FGameplayAbilityTargetDataHandle
		ApplyGameplayEffectSpecToTarget(Handle, ActorInfo, ActivationInfo, EffectSpecHandle, TriggerEventData->TargetData);
	}
}
