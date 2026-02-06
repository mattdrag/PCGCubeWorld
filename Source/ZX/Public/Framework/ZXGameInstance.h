// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ZXGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ZX_API UZXGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
};
