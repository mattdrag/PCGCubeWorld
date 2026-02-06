// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "ZXGameplayEffect.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ZX_API UZXGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	virtual void PostCDOCompiled(const FPostCDOCompiledContext& Context) override;
};
