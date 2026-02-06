// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatPawn.h"
#include "GridPawn.generated.h"

struct FGridTile;
class UGridPawnMovementComponent;


/*
 * A combat pawn that moves around on the grid.
 */
UCLASS()
class ZX_API AGridPawn : public ACombatPawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGridPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UGridPawnMovementComponent* MovementComponent;
};
