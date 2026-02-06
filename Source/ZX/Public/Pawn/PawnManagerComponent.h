// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core/ZX.h"
#include "Components/ActorComponent.h"
#include "PawnManagerComponent.generated.h"


class AGridPawn;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ZX_API UPawnManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPawnManagerComponent();

	// TODO: for now, just a debug spawn function..
	AGridPawn* GenerateGridPawn();

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AGridPawn> DefaultGridPawnClass;

	UPROPERTY(EditAnywhere)
	FVector GridPawnStagingLocation = FVector(0, 0, -10000);
};
