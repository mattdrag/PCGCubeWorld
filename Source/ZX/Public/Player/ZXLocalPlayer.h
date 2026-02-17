// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "ZXLocalPlayer.generated.h"

/**
 * 
 */
UCLASS()
class ZX_API UZXLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()

	virtual bool GetProjectionData(FViewport* Viewport, FSceneViewProjectionData& ProjectionData, int32 StereoViewIndex = INDEX_NONE) const override;
	
	FMatrix CustomProjectionMatrixScalar = FMatrix(
	FPlane(0.70921985815f, 0.0f, 0.0f, 0.0f),
	FPlane(0.0f, 1.0f, 0.0f, 0.0f),
	FPlane(0.0f, 0.0f, 1.0f, 0.0f),
	FPlane(0.0f, 0.0f, 0.0f, 1.0f));
};
