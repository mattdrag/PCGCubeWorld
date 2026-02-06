#include "Framework/ZXGameState.h"

#include "Core/ZXUtils.h"

#include "Pawn/GridPawn.h"
#include "Pawn/PawnManagerComponent.h"
#include "World/GridManagerComponent.h"

AZXGameState::AZXGameState()
{
	GridManager = CreateDefaultSubobject<UGridManagerComponent>(TEXT("GridManager"));
	PawnManager = CreateDefaultSubobject<UPawnManagerComponent>(TEXT("PawnManager"));
}

void AZXGameState::BeginPlay()
{
	Super::BeginPlay();
	
	// todo: remove this later
	if (IsValid(GridManager))
	{
		GridManager->InitData();
		if (bSpawnGridOnStart)
		{
			const bool Result = GridManager->SpawnEntireGrid();
		}
	}
}

void AZXGameState::SpawnGridPawn()
{
	if (IsValid(PawnManager) && IsValid(GridManager))
	{
		UE_LOG(LogZX, Warning, TEXT("Spawning random grid pawn.."));
		
		// Make a GridPawn:
		AGridPawn* SpawnedPawn = PawnManager->GenerateGridPawn();
		if (IsValid(SpawnedPawn))
		{
			// Place him somewhere near the middle of the grid..
			const FIntPoint CenterGrid = FIntPoint(GridManager->GetNumRows() / 2, GridManager->GetNumColumns() / 2);
			// Find an open tile:
			if (FGridTile* OpenGridTile = GridManager->GetOpenGridTile(GridManager->CoordinatesToIndex(CenterGrid), DebugSpawnRadius, true))
			{
				// Tell Grid manager to put him there.
				GridManager->PlacePawnOnGrid(SpawnedPawn, OpenGridTile);
			}
		}
	}
}

void AZXGameState::DestroyGrid()
{
	if (IsValid(GridManager))
	{
		UE_LOG(LogZX, Warning, TEXT("Destroying entire grid.."));
		const bool Result = GridManager->DestroyGrid();
	}
}

void AZXGameState::ZXDisplayGridDebugText(uint8 Mode)
{
	if (IsValid(GridManager))
	{
		GridManager->ToggleGridDebugText(Mode);
	}
}
