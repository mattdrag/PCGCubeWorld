// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/GridPawnAIController.h"

#include "Core/ZXUtils.h"
#include "Components/StateTreeAIComponent.h"
#include "World/GridManagerComponent.h"


// Sets default values
AGridPawnAIController::AGridPawnAIController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StateTree = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAIComponent"));
}

void AGridPawnAIController::BeginPlay()
{
	Super::BeginPlay();

	StateTree->StartLogic();
}

void AGridPawnAIController::SetControlled(bool InControlled)
{
	bIsControlled = InControlled;
	
	if (!bIsControlled)
	{
		StateTree->ResumeLogic("No longer under control..");
	}
	else
	{
		StateTree->StopLogic("Under control..");
	}
}

void AGridPawnAIController::Command_MoveTo(const FVector& NewLocation)
{
	UGridManagerComponent* GridManager = UZXUtils::GetGridManager(this);
	if (!IsValid(GridManager))
	{
		return;
	}

	const FVector GridLoc = GridManager->CoordinatesToWorld(GridManager->WorldToCoordinates(NewLocation));
	MoveToLocation(GridLoc, 0.0001, false, false);
}
