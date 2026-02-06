// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core/ZX.h"
#include "ZXPrimaryDataAsset.h"
#include "SkillSetData.generated.h"

class UZXGameplayAbility;
class UZXGameplayEffect;
/**
 * Abilities and effects associated with.. something.
 */
UCLASS()
class ZX_API USkillSetData : public UZXPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * Data
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	TArray<TSubclassOf<UZXGameplayAbility>> Abilities;

	/**
	 * Meta - Include with every new Data Asset
	 */
	USkillSetData() { AssetType = UZXAssetManager::SkillSetType; }
};
