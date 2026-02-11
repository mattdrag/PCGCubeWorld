// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ZXCameraManager.h"

#include "Camera/CameraComponent.h"
#include "Pawn/ZXPawn.h"


float AZXCameraManager::Init(AZXPawn* InPawn, float InSpeed)
{
	InterpSpeed = InSpeed;
	
	if (!IsValid(InPawn) || !IsValid(InPawn->CameraComponent))
	{
		LOGZXEF("invalid pawn..");
		return -1.f;
	}

	TargetOrthoWidth = InPawn->CameraComponent->OrthoWidth;
	return TargetOrthoWidth;
}

void AZXCameraManager::UpdateCamera(float DeltaTime)
{
	Super::UpdateCamera(DeltaTime);

	// lerp ortho:
	AZXPawn* VTPawn = Cast<AZXPawn>(GetViewTargetPawn());
	if (!IsValid(VTPawn) || !IsValid(VTPawn->CameraComponent))
	{
		return;
	}

	// if current != target, interp:
	const float CurrentOrthoWidth = VTPawn->CameraComponent->OrthoWidth;
	if (!FMath::IsNearlyEqual(CurrentOrthoWidth, TargetOrthoWidth))
	{
		VTPawn->CameraComponent->SetOrthoWidth(FMath::FInterpTo(CurrentOrthoWidth, TargetOrthoWidth, DeltaTime, InterpSpeed));
	}
}
