// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectComponent.h"
#include "GameplayTagContainer.h"
#include "ZXGameplayEffectData.generated.h"

/**
 * This will have a combination of UI data, and other stuff that will be important for auto registering our effects and
 *  associating them with tags. Our GE's add this automatically, MUST HAVE TO REGISTER
 */
UCLASS(Blueprintable, EditInlineNew)
class ZX_API UZXGameplayEffectData : public UGameplayEffectComponent
{
	GENERATED_BODY()

	// MD-TODO: more UI Data..
	UPROPERTY(EditAnywhere)
	FText DisplayName;

	UPROPERTY(EditAnywhere)
	FGameplayTag EffectTag;

	// MD-NOTE: this would be a good place to put requirements i.e. level req, tech req, etc.
};
