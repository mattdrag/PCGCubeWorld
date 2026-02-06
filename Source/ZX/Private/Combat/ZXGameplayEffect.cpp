// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/ZXGameplayEffect.h"

#include "UI/ZXGameplayEffectData.h"


void UZXGameplayEffect::PostCDOCompiled(const FPostCDOCompiledContext& Context)
{
	Super::PostCDOCompiled(Context);

	FindOrAddComponent<UZXGameplayEffectData>();
}
