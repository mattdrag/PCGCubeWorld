// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/GridPawn.h"

#include "Pawn/GridPawnMovementComponent.h"


// Sets default values
AGridPawn::AGridPawn()
{
	// Components:
	MovementComponent = CreateDefaultSubobject<UGridPawnMovementComponent>(TEXT("MovementComponent"));
}

// Called when the game starts or when spawned
void AGridPawn::BeginPlay()
{
	Super::BeginPlay();
}