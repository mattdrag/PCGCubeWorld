// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "Core/ZX.h"
#include "GridTypes.h"
#include "Pawn/GridPawn.h"
#include "Pawn/ZXPawn.h"
#include "World/ZXCube.h"
#include "GridManagerComponent.generated.h"


class AZXSprite;
class UBiomeData;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ZX_API UGridManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// initialization:
	UGridManagerComponent();
	void InitData();
	void SetSeed(int32 InSeed);
	
	
	// Tile Getters:
	FGridTile* GetGridTile(const int32 InIndex);
	FGridTile* GetGridTile(int32 X, int32 Y);
	FGridTile* GetGridTile(const FIntPoint& InCoordinates);
	TArray<FGridTile*> GetGridTilesInRadius(int32 OriginPoint, int32 Radius = 1, bool bIncludeOrigin = false, bool bCheckIfOccupied = false);
	FGridTile* GetOpenGridTile(int32 OriginPoint, uint32 Radius = 1, bool bIncludeOrigin = false, bool bDeterministic = false);

	
	// Coordinate Transformations:
	int32 CoordinatesToIndex(int32 X, int32 Y) const;
	int32 CoordinatesToIndex(const FIntPoint& InCoordinate) const;
	FIntPoint IndexToCoordinates(int32 InIndex) const;
	FVector CoordinatesToWorld(const FIntPoint& InCoordinate) const;
	FVector IndexToWorld(int32 InIndex) const;
	FIntPoint WorldToCoordinates(const FVector& InWorld) const;
	int32 WorldToIndex(const FVector& InWorld) const;
	FVector SnapToGrid(const FVector& InWorldLocation) const;
	

	// Simple getters:
	FORCEINLINE int32 IsGridGenerated() const { return bIsDataGenerated; }
	FORCEINLINE int32 GetNumGridTiles() const { return GridTiles.Num(); }
	FORCEINLINE int32 GetNumRows() const { return Rows; }
	FORCEINLINE int32 GetNumColumns() const { return Columns; }
	FORCEINLINE FIntPoint GetGridMidpoint() const { return FIntPoint(Rows/2, Columns/2); }
	FORCEINLINE float GetGridHeight() const { return GridHeight; }
	
	
	// Grid Generation:
	bool SpawnEntireGrid(int32 InSeed = 0, const FCellularAutomataOptions& InCAOptions = FCellularAutomataOptions());
	bool GenerateGridData();
	bool DestroyGrid();
	bool SpawnCube(int32 InCubeIndex, int32 OptionalSwapIdx = -1);
	void FreeCube(int32 InCubeIndex);
	void FreeCube(const FGridTile& InTile);
	void MarkCube(int32 InCubeIndex);
	void CellularAutomataStep();
	
	
	// Places a guy on the grid:
	void PlacePawnOnGrid(AGridPawn* GridPawn, FGridTile* GridTile);
	void PlacePawnOnGrid(AGridPawn* GridPawn, int32 GridTile);
	
	
	// Map Data:
	FLinearColor GetColorForTile(const UBiomeData& InBiome, ETileType InTileType, float InAltitude, bool bAddBaseColor = false);
	FColor GetColorForMapTile(int32 InIdx);
	
	
	// Debug:
	void ToggleGridDebugText(uint8 Mode);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category="Generation")
	int32 Rows = 0;

	UPROPERTY(EditDefaultsOnly, Category="Generation")
	int32 Columns = 0;

	UPROPERTY(EditDefaultsOnly, Category="Generation")
	int32 CubeSize = 100;
	
	UPROPERTY(EditDefaultsOnly, Category="Generation")
	int32 GridSeed = 0;

	// for now load radius needs to be (Load_Radius*2 + 1) % 3 = 0
	UPROPERTY(EditDefaultsOnly, Category="Generation")
	int32 Load_Radius = 7;

	UPROPERTY(EditDefaultsOnly, Category="Generation", meta = (ClampMin=0,ClampMax=10))
	float PerlinScalar = 0.01;
	
	UPROPERTY(EditDefaultsOnly, Category="Cubes")
	TSubclassOf<AZXCube> CubeClass;
	
	// TODO: we may expand on this concept with multiple grids..
	float GridHeight = 0.f;
	
#pragma region Styling 
	void LoadBiomes();
	ETileType DetermineTileType(EBiome InBiome, float InAltitude);
	void StyleCube(AZXCube* InCube);
	uint8 Autotile(ETileType InType, const FIntPoint& InCoord);
	bool HasAnyNeighborsOfType(ETileType InType, const FIntPoint& InCoord);

	void SpawnFoliage(const UBiomeData& InBiome, const FVector& InWorldLoc);
	int32 GetJitteredGridForTile(FGridTile* InTile, TArray<FVector2D>& OutPoints, float FoliageLB, float FoliageUB);

	float PerlinNoiseZX(const FVector2D& Location);
	
	UPROPERTY(EditDefaultsOnly, Category="Styling")
	TArray<uint8> BitmaskToTileStyle;
	
	UPROPERTY(EditDefaultsOnly, Category="Styling")
	TSubclassOf<AZXSprite> FoliageClass;
#pragma endregion Styling
	
private:
	UPROPERTY()
	TArray<FGridTile> GridTiles;
	
	FRandomStream GridRandom;

	FCellularAutomataOptions CellularAutomataOptions;
	
	UPROPERTY()
	TMap<EBiome, TObjectPtr<UBiomeData>> BiomeData;
	
	TArray<int32> ShuffledPermutation;
	
	// TODO: change to an enum when grid generation is more complex
	bool bIsDataGenerated = false;
};
