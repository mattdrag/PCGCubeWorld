// Fill out your copyright notice in the Description page of Project Settings.


#include "World/GridManagerComponent.h"

#include "Pawn/GridPawnAIController.h"
#include "World/ZXCube.h"
#include "Core/ZXUtils.h"
#include "Data/BiomeData.h"
#include "Data/FoliageData.h"
#include "World/FoliageSprite.h"

namespace GridManagerConsts
{
	const FName SpawnedCubeFolder = FName("GridCubes");
	const FName SpawnedFoliageFolder = FName("Foliage");
}

UGridManagerComponent::UGridManagerComponent()
{
}

void UGridManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SetSeed(GridSeed);
	
	// We are responsible for styling tiles. at this point, we should tell the asset management system to load our styles:
	LoadBiomes();
}

void UGridManagerComponent::InitData()
{
	GridTiles.Empty();
	bIsDataGenerated = false;

	SetSeed(GridSeed);
}


FGridTile* UGridManagerComponent::GetGridTile(const int32 InIndex)
{
	// Index into our grid tiles array, return a ptr:
	if (GridTiles.IsValidIndex(InIndex))
	{
		return &GridTiles[InIndex];
	}
	return nullptr;
}

FGridTile* UGridManagerComponent::GetGridTile(int32 X, int32 Y)
{
	// check grid bounds:
	if (X < 0 || X >= Columns || Y < 0 || Y >= Rows)
	{
		return nullptr;
	}
	
	// Convert coords to index, then call our other function:
	return GetGridTile(CoordinatesToIndex(X,Y));
}

FGridTile* UGridManagerComponent::GetGridTile(const FIntPoint& InCoordinates)
{
	// break apart FIntPoint:
	return GetGridTile(InCoordinates.X, InCoordinates.Y);
}

int32 UGridManagerComponent::CoordinatesToIndex(int32 X, int32 Y) const
{
	return X * Rows + Y;
}

int32 UGridManagerComponent::CoordinatesToIndex(const FIntPoint& InCoordinate) const
{
	// X * R + Y
	return CoordinatesToIndex(InCoordinate.X, InCoordinate.Y);
}

FIntPoint UGridManagerComponent::IndexToCoordinates(int32 InIndex) const
{
	if (Columns != 0)
	{
		// ( (Floor(i/C) , i % C )
		return FIntPoint(FMath::Floor(InIndex/Columns), InIndex % Columns);
	}
	
	return FIntPoint::ZeroValue;
}

FVector UGridManagerComponent::CoordinatesToWorld(const FIntPoint& InCoordinate) const
{
	// in order for 0,0 to be center of the world, we offset the x and y based on grid dimensions:
	const float OffsetX = Columns / 2;
	const float OffsetY = Rows / 2;
	return FVector(InCoordinate.X * CubeSize - OffsetX * CubeSize, InCoordinate.Y * CubeSize - OffsetY * CubeSize, GridHeight);
}

FVector UGridManagerComponent::IndexToWorld(int32 InIndex) const
{
	return CoordinatesToWorld(IndexToCoordinates(InIndex));
}

FIntPoint UGridManagerComponent::WorldToCoordinates(const FVector& InWorld) const
{
	// create coordinate from world
	const float OffsetX = Columns / 2;
	const float OffsetY = Rows / 2;
	const int CoordX = FMath::RoundToInt((InWorld.X + OffsetX * CubeSize)/CubeSize);
	const int CoordY = FMath::RoundToInt((InWorld.Y + OffsetY * CubeSize)/CubeSize);
	return FIntPoint(CoordX, CoordY);
}

int32 UGridManagerComponent::WorldToIndex(const FVector& InWorld) const
{
	return CoordinatesToIndex(WorldToCoordinates(InWorld));
}

FVector UGridManagerComponent::SnapToGrid(const FVector& InWorldLocation) const
{
	return IndexToWorld(WorldToIndex(InWorldLocation));
}

TArray<FGridTile*> UGridManagerComponent::GetGridTilesInRadius(int32 OriginPoint, int32 Radius, bool bIncludeOrigin, bool bCheckIfOccupied)
{
	TArray<FGridTile*> RetArr;

	// check for valid origin:
	FGridTile* Origin = GetGridTile(OriginPoint);
	if (Origin == nullptr)
	{
		return RetArr;
	}

	const FIntPoint OriginCoords = IndexToCoordinates(OriginPoint);

	// iterate in range:
	for (int32 i = -Radius; i < Radius; i++)
	{
		for (int32 j = -Radius; j < Radius; j++)
		{
			if (FGridTile* ThisGridTile = GetGridTile(OriginCoords.X + i, OriginCoords.Y + j))
			{
				if (bCheckIfOccupied && ThisGridTile->IsOccupied())
				{
					continue;
				}
				
				if (OriginPoint != ThisGridTile->MyIndex || bIncludeOrigin)
				{
					RetArr.Add(ThisGridTile);
				}
			}
		}
	}

	return RetArr;
}

FGridTile* UGridManagerComponent::GetOpenGridTile(int32 OriginPoint, uint32 Radius, bool bIncludeOrigin, bool bDeterministic)
{
	TArray<FGridTile*> GridTilesInRadius = GetGridTilesInRadius(OriginPoint, Radius, bIncludeOrigin, true);
	if (GridTilesInRadius.IsEmpty())
	{
		return nullptr;
	}

	// if deterministic, use our seed. else use randrange:
	const int32 ChoosenIdx = bDeterministic ? GridRandom.RandRange(0, GridTilesInRadius.Num() - 1) : FMath::RandRange(0, GridTilesInRadius.Num() - 1);
	return GridTilesInRadius[ChoosenIdx];
}

bool UGridManagerComponent::GenerateGridData()
{
	for (int32 i = 0; i < Rows; i++)
	{
		for (int32 j = 0; j < Columns; j++)
		{
			// Init with our current seed:
			FMath::RandInit(GridRandom.GetCurrentSeed());
			
			// todo: once we have more biomes, get actual biome
			const EBiome ChosenBiome = EBiome::TestGrasslands;
			
			// Make a tile:
			FGridTile NewTile(i * Columns + j);
			NewTile.Altitude = PerlinNoiseZX(FVector2D(i + 0.5f,j + 0.5f) * PerlinScalar);
			NewTile.Type = DetermineTileType(ChosenBiome, NewTile.Altitude); 
			NewTile.Biome = ChosenBiome;
			
			// this guy also gets some foliage potentially:
			GenerateFoliageForGridCell(ChosenBiome, NewTile);
			
			GridTiles.Add(NewTile);
		}
	}

	return true;
}

void UGridManagerComponent::GenerateFoliageForGridCell(EBiome InBiomeType, FGridTile& InTile)
{
	// get biome:
	TObjectPtr<UBiomeData>* InBiome = BiomeData.Find(InBiomeType);
	// NOTE: we technically dont need these checks, theyre done in validate right now..
	if (InBiome == nullptr || !IsValid(*InBiome))
	{
		return;
	}
	if ((*InBiome)->Foliage.IsEmpty())
	{
		return;
	}
	
	//  - NOTE: since we are styling on 1 dim, highes Altitude will have most foliage for now:
	const float FoliageLB = -1.f; // TODO: altitude curve
	const float FoliageUB = -0.3f;
	if (InTile.Altitude >= FoliageUB)
	{
		return;
	}
	
	// we can have between 0-8 foliage. split the range from LB -> UB into 8 discrete thresholds:
	const float FoliageNumThresh = 4;
	const float FoliageIncrementInterval = (FoliageLB - FoliageUB) / FoliageNumThresh;
	
	// now normalize this tiles altitude against it and LB:
	// TODO: for now, one foliage each..
	const int32 NumFoliage =  FMath::Clamp((InTile.Altitude - FoliageUB) / FoliageIncrementInterval, 0, 1);
	if (NumFoliage > 0)
	{
		TArray<FVector2D> FoliageLocs;
		GetJitteredGridForCell(NumFoliage, FoliageLocs);
		for (const FVector2D& FoliageLoc : FoliageLocs)
		{
			// Add inst:
			// pick a random foliage for now:
			// TODO: get random weighted index
			const int32 FoliageIdx = FoliageRandom.RandRange(0, (*InBiome)->Foliage.Num() - 1);
			UFoliageData* ChosenFoliage = (*InBiome)->Foliage[FoliageIdx];
			InTile.FoliageInsts.Add(FFoliageInst(ChosenFoliage, FoliageLoc));
		}
	}
}

void UGridManagerComponent::CellularAutomataStep()
{
	// Next gen will be applied at the end of the step:
	TArray<ETileType> NextGen;
	NextGen.Reserve(GridTiles.Num());
	
	// iterate all grid tiles:
	for (FGridTile& GridTile : GridTiles)
	{
		// Count the number of grass vs dirt:
		int32 NumGrass = 0;
		int32 NumDirt = 0;
		for (const FIntPoint& Neighbor : GridTypesConsts::NeighborArray)
		{
			const FIntPoint NeighborCoordinate = IndexToCoordinates(GridTile.MyIndex) + Neighbor;
			const FGridTile* NeighborTile = GetGridTile(NeighborCoordinate);
			if (NeighborTile != nullptr && NeighborTile->Type == ETileType::Grass)
			{
				NumGrass++;
			}
			else
			{
				NumDirt++;
			}
		}

		// transform this tile based on grass to dirt ratio:
		if (GridTile.Type == ETileType::Grass && NumDirt > 4)
		{
			// grass dies:
			NextGen.Add(ETileType::Sand);
		}
		else if (GridTile.Type == ETileType::Sand && NumGrass > 4)
		{
			// grass grows:
			NextGen.Add(ETileType::Grass);
		}
		else
		{
			// no change:
			NextGen.Add(GridTile.Type);
		}
	}

	// Apply NextGen tile types:
	for (int32 i = 0; i < GridTiles.Num(); i++)
	{
		if (!NextGen.IsValidIndex(i))
		{
			LOGZXEF("Next generation has invalid index..");
			return;
		}

		GridTiles[i].Type = NextGen[i];
	}
}

bool UGridManagerComponent::DestroyGrid()
{
	// destroy cubes:
	for (int32 i = 0; i < GridTiles.Num(); i++)
	{
		FreeCube(GridTiles[i]);
	}

	// clear the data:
	InitData();
	
	return true;
}

bool UGridManagerComponent::SpawnEntireGrid(int32 InSeed, const FCellularAutomataOptions& InCAOptions)
{
	// 0. Destroy everything:
	if (!DestroyGrid())
	{
		return false;
	}

	// 0.5. Config:
	SetSeed(InSeed);
	CellularAutomataOptions = InCAOptions;
	
	// 0.6 Validate data:
	if (!ValidateGridData())
	{
		return false;
	}
	
	// 1. Make data
	if (!GenerateGridData())
	{
		return false;
	}

	// 2. Run CA:
	for (int32 i = 0; i < CellularAutomataOptions.NumIterations; i++)
	{
		CellularAutomataStep();
	}
	

	// Completed:
	bIsDataGenerated = true;
	if (auto UIDelegates = UZXUtils::GetUIDelegates(this))
	{
		UIDelegates->OnMapGenerationComplete.Broadcast();
	}

	return true;
}

bool UGridManagerComponent::ValidateGridData()
{
	// todo: theres only one biome right now..
	const EBiome ChosenBiome = EBiome::TestGrasslands;
	
	TObjectPtr<UBiomeData>* InBiome = BiomeData.Find(ChosenBiome);
	if (InBiome == nullptr || !IsValid(*InBiome))
	{
		LOGZXEF("CurrentBiome configured incorrectly");
		return false;
	}
	if ((*InBiome)->Foliage.IsEmpty())
	{
		LOGZXEF("Biome %s has no foliage..", *(*InBiome)->DisplayName.ToString());
		return false;
	}
	
	return true;
}

void UGridManagerComponent::FreeCube(int32 InCubeIndex)
{
	if (!GridTiles.IsValidIndex(InCubeIndex))
	{
		LOGZXEF("invalid idx %d..", InCubeIndex);
		return;
	}
	
	FreeCube(GridTiles[InCubeIndex]);
}

void UGridManagerComponent::FreeCube(FGridTile& InTile)
{
	// md todo: pool
	if (AZXCube* ValidCube = InTile.MyCube.Get())
	{
		ValidCube->Destroy();
	}
	
	// remove foliage:
	FreeFoliage(InTile);
}

void UGridManagerComponent::SpawnFoliage(FGridTile& InTile)
{
	// md todo: grab from pool
	
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}
	
	// destroy any foliage that is already there..
	if (!InTile.SpawnedFoliage.IsEmpty())
	{
		LOGZXWF("spawning cube %d already has foliage.. destroying it first..", InTile.MyIndex);
		FreeFoliage(InTile);
	}
	
	// for each inst (loc + data), spawn a foliage:
	for (const FFoliageInst& FoliageInst : InTile.FoliageInsts)
	{
		// we need valid data to spawn it:
		if (!IsValid(FoliageInst.Data))
		{
			LOGZXWF("Failed to spawn foliage for tile (%d) - data invalid.", InTile.MyIndex);
			continue;
		}
		
		AFoliageSprite* FoliageSprite = World->SpawnActor<AFoliageSprite>(FoliageClass, IndexToWorld(InTile.MyIndex) + FVector(FoliageInst.LocationOffset.X, FoliageInst.LocationOffset.Y, 0.f), FRotator::ZeroRotator);
		if (!IsValid(FoliageSprite))
		{
			LOGZXWF("Failed to spawn foliage (%d)", InTile.MyIndex);
			return;
		}
		
		// set up data:
		FoliageSprite->SetFolderPath(GridManagerConsts::SpawnedFoliageFolder);
		FoliageSprite->SetData(*FoliageInst.Data, InTile.MyIndex);
		
		// add ref:
		InTile.SpawnedFoliage.Add(FoliageSprite);
	}
}

void UGridManagerComponent::FreeFoliage(FGridTile& InTile)
{
	// md todo: pool
	
	// destroy all spawned foliage:
	for (AFoliageSprite* SpawnedFoliage : InTile.SpawnedFoliage)
	{
		SpawnedFoliage->Destroy();
	}
	
	// remove all refs:
	InTile.SpawnedFoliage.Empty();
}

bool UGridManagerComponent::SpawnCube(int32 InCubeIndex, int32 OptionalSwapIdx)
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return false;
	}
	
	// get grid tile:
	FGridTile* GridTile = GetGridTile(InCubeIndex);
	if (GridTile == nullptr)
	{
		LOGZXEF("Grid tile is null..");
		return false;
	}
	
	// if theres already a cube, free it first:
	if (GridTile->MyCube != nullptr)
	{
		LOGZXWF("spawning cube %d but it already exists.. freeing first..", InCubeIndex);
		FreeCube(*GridTile);
	}
	
	AZXCube* SpawnedCube;
	// Swap:
	if (OptionalSwapIdx != -1 && GridTiles.IsValidIndex(OptionalSwapIdx) && GridTiles[OptionalSwapIdx].MyCube.IsValid())
	{
		// we need to remove foliage and remake afterwards:
		FreeFoliage(*GridTile);
		
		// perform the swap:
		SpawnedCube = GridTiles[OptionalSwapIdx].MyCube.Get();
		GridTiles[OptionalSwapIdx].MyCube = nullptr;
		SpawnedCube->SetActorLocation(IndexToWorld(InCubeIndex));
	}
	// Spawn:
	else
	{
		SpawnedCube = World->SpawnActor<AZXCube>(CubeClass,  IndexToWorld(InCubeIndex), FRotator::ZeroRotator);
	}
	if (!IsValid(SpawnedCube))
	{
		LOGZXWF("Failed to spawn cube (%d)", InCubeIndex);
		return false;
	}

	// Put it in a folder for easier viewing of world outliner:
	SpawnedCube->SetFolderPath(GridManagerConsts::SpawnedCubeFolder);

	// Set-up data for our cube:
	SpawnedCube->SetData(*GridTile);

	// style it:
	StyleCube(SpawnedCube);
	
	// set up a weak reference for our data to ref our in world actor:
	GridTile->MyCube = SpawnedCube;
	
	SpawnFoliage(*GridTile);
	
	return true;
}

void UGridManagerComponent::PlacePawnOnGrid(AGridPawn* GridPawn, FGridTile* GridTile)
{
	if (!IsValid(GridPawn))
	{
		UE_LOG(LogZX, Error, TEXT("Tried placing invalid grid pawn.."));
		return;
	}
	if (GridTile == nullptr)
	{
		UE_LOG(LogZX, Error, TEXT("Tried placing grid pawn at invalid tile.."));
		return;
	}
	// TODO: it should probably have a valid AIC by now?
	AGridPawnAIController* GridPawnAIC = Cast<AGridPawnAIController>(GridPawn->GetController());
	if (!IsValid(GridPawnAIC))
	{
		UE_LOG(LogZX, Error, TEXT("Invalid Grid Pawn AIC.."));
		return;
	}

	const FIntPoint CubeLoc = IndexToCoordinates(GridTile->MyIndex);
	const FVector CubeLocWorld = CoordinatesToWorld(CubeLoc);
	// const FVector BoostedLoc = CubeLocWorld + FVector(0, 0, CubeSize);
	const FVector BoostedLoc = CubeLocWorld + FVector(0, 0, GridHeight);
	GridPawn->SetActorLocation(BoostedLoc);
	GridPawnAIC->InitialGridTile = GridTile->MyIndex;

	// TODO: tile is now occupied.
}

void UGridManagerComponent::PlacePawnOnGrid(AGridPawn* GridPawn, int32 TileIndex)
{
	PlacePawnOnGrid(GridPawn, GetGridTile(TileIndex));
}

void UGridManagerComponent::ToggleGridDebugText(uint8 Mode)
{
	for (const FGridTile& GridTile : GridTiles)
	{
		if (AZXCube* MyCube = GridTile.MyCube.Get())
		{
			MyCube->ToggleGridDebugText(Mode);
		}
	}
}



/*
 * Seeded Perlin noise:
 *  - FMath doesnt have seeded random perlin noise generator.
 *  -> copy 99% of their code and add in seeding somewhere
 */

namespace ZXNoiseHelpers
{
	// random permutation of 256 numbers, repeated 2x
	static constexpr int32 NumPermutations = 512;
	static const int32 Permutation[NumPermutations] = {
		63, 9, 212, 205, 31, 128, 72, 59, 137, 203, 195, 170, 181, 115, 165, 40, 116, 139, 175, 225, 132, 99, 222, 2, 41, 15, 197, 93, 169, 90, 228, 43, 221, 38, 206, 204, 73, 17, 97, 10, 96, 47, 32, 138, 136, 30, 219,
		78, 224, 13, 193, 88, 134, 211, 7, 112, 176, 19, 106, 83, 75, 217, 85, 0, 98, 140, 229, 80, 118, 151, 117, 251, 103, 242, 81, 238, 172, 82, 110, 4, 227, 77, 243, 46, 12, 189, 34, 188, 200, 161, 68, 76, 171, 194,
		57, 48, 247, 233, 51, 105, 5, 23, 42, 50, 216, 45, 239, 148, 249, 84, 70, 125, 108, 241, 62, 66, 64, 240, 173, 185, 250, 49, 6, 37, 26, 21, 244, 60, 223, 255, 16, 145, 27, 109, 58, 102, 142, 253, 120, 149, 160,
		124, 156, 79, 186, 135, 127, 14, 121, 22, 65, 54, 153, 91, 213, 174, 24, 252, 131, 192, 190, 202, 208, 35, 94, 231, 56, 95, 183, 163, 111, 147, 25, 67, 36, 92, 236, 71, 166, 1, 187, 100, 130, 143, 237, 178, 158,
		104, 184, 159, 177, 52, 214, 230, 119, 87, 114, 201, 179, 198, 3, 248, 182, 39, 11, 152, 196, 113, 20, 232, 69, 141, 207, 234, 53, 86, 180, 226, 74, 150, 218, 29, 133, 8, 44, 123, 28, 146, 89, 101, 154, 220, 126,
		155, 122, 210, 168, 254, 162, 129, 33, 18, 209, 61, 191, 199, 157, 245, 55, 164, 167, 215, 246, 144, 107, 235, 

		63, 9, 212, 205, 31, 128, 72, 59, 137, 203, 195, 170, 181, 115, 165, 40, 116, 139, 175, 225, 132, 99, 222, 2, 41, 15, 197, 93, 169, 90, 228, 43, 221, 38, 206, 204, 73, 17, 97, 10, 96, 47, 32, 138, 136, 30, 219,
		78, 224, 13, 193, 88, 134, 211, 7, 112, 176, 19, 106, 83, 75, 217, 85, 0, 98, 140, 229, 80, 118, 151, 117, 251, 103, 242, 81, 238, 172, 82, 110, 4, 227, 77, 243, 46, 12, 189, 34, 188, 200, 161, 68, 76, 171, 194,
		57, 48, 247, 233, 51, 105, 5, 23, 42, 50, 216, 45, 239, 148, 249, 84, 70, 125, 108, 241, 62, 66, 64, 240, 173, 185, 250, 49, 6, 37, 26, 21, 244, 60, 223, 255, 16, 145, 27, 109, 58, 102, 142, 253, 120, 149, 160,
		124, 156, 79, 186, 135, 127, 14, 121, 22, 65, 54, 153, 91, 213, 174, 24, 252, 131, 192, 190, 202, 208, 35, 94, 231, 56, 95, 183, 163, 111, 147, 25, 67, 36, 92, 236, 71, 166, 1, 187, 100, 130, 143, 237, 178, 158,
		104, 184, 159, 177, 52, 214, 230, 119, 87, 114, 201, 179, 198, 3, 248, 182, 39, 11, 152, 196, 113, 20, 232, 69, 141, 207, 234, 53, 86, 180, 226, 74, 150, 218, 29, 133, 8, 44, 123, 28, 146, 89, 101, 154, 220, 126,
		155, 122, 210, 168, 254, 162, 129, 33, 18, 209, 61, 191, 199, 157, 245, 55, 164, 167, 215, 246, 144, 107, 235
	};
	
	// Note: If you change the Grad2 or Grad3 functions, check that you don't change the range of the resulting noise as well; it should be (within floating point error) in the range of (-1, 1)
	FORCEINLINE float Grad2(int32 Hash, float X, float Y)
	{
		// corners and major axes (similar to the z=0 projection of the cube-edge-midpoint sampling from improved Perlin noise)
		switch (Hash & 7)
		{
		case 0: return X;
		case 1: return X + Y;
		case 2: return Y;
		case 3: return -X + Y;
		case 4: return -X;
		case 5: return -X - Y;
		case 6: return -Y;
		case 7: return X - Y;
		// can't happen
		default: return 0;
		}
	}

	// Curve w/ second derivative vanishing at 0 and 1, from Perlin's improved noise paper
	FORCEINLINE float SmoothCurve(float X)
	{
		return X * X * X * (X * (X * 6.0f - 15.0f) + 10.0f);
	}
	
	static constexpr int32 NumFoliageSlots = 9;
	static constexpr float FoliageCellDims = 3.f;
	static const FIntPoint FoliageSlots[NumFoliageSlots] = 
	{
		FIntPoint(0,0), FIntPoint(0,1), FIntPoint(0,2),
		FIntPoint(1,0), FIntPoint(1,1), FIntPoint(1,2),
		FIntPoint(2,0), FIntPoint(2,1), FIntPoint(2,2)
	};
}

void UGridManagerComponent::SetSeed(int32 InSeed)
{
	GridRandom.Initialize(InSeed);
	FoliageRandom.Initialize(InSeed);
	
	// seed our perlin noise too:
	ShuffledPermutation.Reset(ZXNoiseHelpers::NumPermutations);
	ShuffledPermutation.Append(ZXNoiseHelpers::Permutation);
	
	const int32 LastIndex = ZXNoiseHelpers::NumPermutations - 1;	
	for (int32 i = 0; i < LastIndex; ++i)
	{
		int32 Index = GridRandom.RandRange(0, LastIndex);
		if (i != Index)
		{
			ShuffledPermutation.Swap(i, Index);
		}
	}
	
	// setup our foliage random:
	ShuffledFoliageSlots.Reset();
	ShuffledFoliageSlots.Append(ZXNoiseHelpers::FoliageSlots);
}

float UGridManagerComponent::PerlinNoiseZX(const FVector2D& Location)
{
	using namespace ZXNoiseHelpers;

	float Xfl = FMath::FloorToFloat((float)Location.X);		// LWC_TODO: Precision loss
	float Yfl = FMath::FloorToFloat((float)Location.Y);
	int32 Xi = (int32)(Xfl) & 255;
	int32 Yi = (int32)(Yfl) & 255;
	float X = (float)Location.X - Xfl;
	float Y = (float)Location.Y - Yfl;
	float Xm1 = X - 1.0f;
	float Ym1 = Y - 1.0f;
	
	int32 AA = ShuffledPermutation[Xi] + Yi;
	int32 AB = AA + 1;
	int32 BA = ShuffledPermutation[Xi + 1] + Yi;
	int32 BB = BA + 1;

	float U = SmoothCurve(X);
	float V = SmoothCurve(Y);

	// Note: Due to the choice of Grad2, this will be in the (-1,1) range with no additional scaling
	return FMath::Lerp(
			FMath::Lerp(Grad2(ShuffledPermutation[AA], X, Y), Grad2(ShuffledPermutation[BA], Xm1, Y), U),
			FMath::Lerp(Grad2(ShuffledPermutation[AB], X, Ym1), Grad2(ShuffledPermutation[BB], Xm1, Ym1), U),
			V);
}

void UGridManagerComponent::GetJitteredGridForCell(int32 NumPoints, TArray<FVector2D>& OutPoints)
{
	// randomly pick foliage slots:
	//  [][][]
	//  [][][]
	//  [][][]
	
	if (ShuffledFoliageSlots.Num() < NumPoints)
	{
		LOGZXEF("trying to access more foliage num than slots..");
		return;
	}
	
	const float FullCell = CubeSize / ZXNoiseHelpers::FoliageCellDims;
	const float HalfCell = FullCell / 2.f;
	
	// shuffle the foliage slots:
	for (int32 i = 0; i < ZXNoiseHelpers::NumFoliageSlots - 1; ++i)
	{
		int32 Index = FoliageRandom.RandRange(0, ZXNoiseHelpers::NumFoliageSlots - 1);
		if (i != Index)
		{
			ShuffledFoliageSlots.Swap(i, Index);
		}
	}
	
	// place first N foliage, where N is numpoints off the front of the shuffle slots arr:
	for (int32 i = 0; i < NumPoints; ++i)
	{
		// cell center:
		const float CellCenterX = (ShuffledFoliageSlots[i].X * FullCell) + HalfCell - CubeSize / 2.f;
		const float CellCenterY = (ShuffledFoliageSlots[i].Y * FullCell) + HalfCell - CubeSize / 2.f;

		// jitter it:
		const float MaxOffset = HalfCell * FoliageJitter;
		const float OffsetX = FoliageRandom.FRandRange(-MaxOffset, MaxOffset);
		const float OffsetY = FoliageRandom.FRandRange(-MaxOffset, MaxOffset);

		OutPoints.Add(FVector2D(CellCenterX + OffsetX, CellCenterY + OffsetY));
	}
}
