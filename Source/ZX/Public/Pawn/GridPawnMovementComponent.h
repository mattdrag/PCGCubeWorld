// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GridPawnMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ZX_API UGridPawnMovementComponent : public UFloatingPawnMovement
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGridPawnMovementComponent();
};
