// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ZXLocalPlayer.h"

#include "Core/ZX.h"

bool UZXLocalPlayer::GetProjectionData(FViewport* Viewport, FSceneViewProjectionData& ProjectionData,
                                       int32 StereoViewIndex) const
{
	bool Res = Super::GetProjectionData(Viewport, ProjectionData, StereoViewIndex);
	ProjectionData.ProjectionMatrix = ProjectionData.ProjectionMatrix * CustomProjectionMatrixScalar;
	return Res;
}
