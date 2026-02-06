// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/PawnManagerComponent.h"

#include "Pawn/GridPawn.h"

namespace GridPawnConsts
{
	const FName SpawnedPawnFolder = FName("GridPawns");
}

// Sets default values for this component's properties
UPawnManagerComponent::UPawnManagerComponent()
{
}

AGridPawn* UPawnManagerComponent::GenerateGridPawn()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return nullptr;
	}
	
	// TODO: some step that uses DataAsset

	// Spawn the guy:
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AGridPawn* GridPawn = World->SpawnActor<AGridPawn>(DefaultGridPawnClass, GridPawnStagingLocation, FRotator::ZeroRotator, SpawnParams);
	if (!IsValid(GridPawn))
	{
		UE_LOG(LogZX, Warning, TEXT("Failed to spawn pawn.."));
		return nullptr;
	}

	// Put it in a folder for easier viewing of world outliner:
	GridPawn->SetFolderPath(GridPawnConsts::SpawnedPawnFolder);

	return GridPawn;
}
