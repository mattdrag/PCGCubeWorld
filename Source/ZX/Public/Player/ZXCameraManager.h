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
	void Init(AZXPawn* InPawn, float InSpeed);
	
	FORCEINLINE float GetTargetOrthoWidth() const { return TargetOrthoWidth; }
	FORCEINLINE void SetTargetOrthoWidth(float InOrthoWidth) { TargetOrthoWidth = InOrthoWidth; }

protected:
	virtual void UpdateCamera(float DeltaTime) override;

	float TargetOrthoWidth = 0.f;
	
	float InterpSpeed = 1.f;
};
