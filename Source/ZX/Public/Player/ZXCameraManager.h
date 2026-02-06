// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core/ZX.h"
#include "Camera/PlayerCameraManager.h"
#include "Pawn/ZXPawn.h"
#include "ZXCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class ZX_API AZXCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	float TargetOrthoWidth = 0.f;

	void Init(AZXPawn* InPawn, float InSpeed);

protected:
	virtual void UpdateCamera(float DeltaTime) override;

	float InterpSpeed = 1.f;
};
