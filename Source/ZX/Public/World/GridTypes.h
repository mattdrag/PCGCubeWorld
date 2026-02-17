#pragma once

#include "CoreMinimal.h"
#include "GridTypes.generated.h"

class AFoliageSprite;
struct FFoliageInst;
class UFoliageData;
class IGridEntityInterface;
class AZXCube;

namespace GridTypesConsts
{
	const TArray<FIntPoint> NeighborArray =
	{
		FIntPoint(1,-1),
		FIntPoint(1,0),
		FIntPoint(1,1),
		FIntPoint(0,-1),
		FIntPoint(0,1),
		FIntPoint(-1,-1),
		FIntPoint(-1,0),
		FIntPoint(-1,1),
	};
	
	FORCEINLINE FIntPoint GridToMap(const FIntPoint& InGridCoords) { return FIntPoint(InGridCoords.Y, -InGridCoords.X); }
	FORCEINLINE FIntPoint MapToGrid(const FIntPoint& InMapCoords) { return FIntPoint(-InMapCoords.Y, InMapCoords.X); }
	
	FORCEINLINE FVector2D GridToMap(const FVector2D& InGridCoords) { return FVector2D(InGridCoords.Y, -InGridCoords.X); }
	FORCEINLINE FVector2D MapToGrid(const FVector2D& InMapCoords) { return FVector2D(-InMapCoords.Y, InMapCoords.X); }
}

UENUM()
enum class ENeighborDirection : uint8
{
	NorthWest	= 1 << 0, // 00000001
	North		= 1 << 1, // 00000010
	NorthEast	= 1 << 2, // 00000100
	West		= 1 << 3, // 00001000
	East		= 1 << 4, // 00010000
	SouthWest	= 1 << 5, // 00100000
	South		= 1 << 6, // 01000000
	SouthEast	= 1 << 7, // 10000000
};

UENUM(BlueprintType)
enum class ETileType : uint8
{
	Grass,
	Sand,
	Water,
	
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(ETileType, ETileType::Count);

USTRUCT(BlueprintType)
struct FColorRange
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere)
	FLinearColor Lightest;
	
	UPROPERTY(EditAnywhere)
	FLinearColor Base;
	
	UPROPERTY(EditAnywhere)
	FLinearColor Darkest;
};

USTRUCT(BlueprintType)
struct FBiomeTileTypeConfig
{
	GENERATED_USTRUCT_BODY()
	
	// TODO: refactor to work on 2 dims, moisture and altitude. will also determine mountains.
	// for now, very simple transition between -1 to 1, grass to sand to water.
	
	// -1 to sand_thresh
	UPROPERTY(EditAnywhere)
	ETileType BaseType = ETileType::Grass;
	
	// sand_thresh to water_thresh
	UPROPERTY(EditAnywhere)
	float SandThreshold = -0.1f;
	
	// water_thresh to 1
	UPROPERTY(EditAnywhere)
	float WaterThreshold = 0.f;
};

UENUM()
enum class EBiome : uint8
{
	TestGrasslands,
	
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EBiome, ETileType::Count);


// associates foliage data with a location offset relative to grid cell center.
USTRUCT(BlueprintType)
struct FFoliageInst
{
	GENERATED_USTRUCT_BODY()

	FFoliageInst() {}
	
	FFoliageInst(TObjectPtr<UFoliageData> InData, FVector2D Loc) 
		: Data(InData), LocationOffset(Loc)
	{}

	FVector2D LocationOffset;
	
	// TODO: consider soft object path
	UPROPERTY()
	TObjectPtr<UFoliageData> Data;
};

/*
**	Data for a grid tile
*/
USTRUCT(BlueprintType)
struct FGridTile
{
	GENERATED_USTRUCT_BODY()

	FGridTile() {}
	
	FGridTile(int32 InIdx)
		: MyIndex(InIdx)
	{}

	// TODO: occupancy.
	bool IsOccupied() { return false; }
	
	// Tile type:
	UPROPERTY(BlueprintReadOnly)
	ETileType Type = ETileType::Grass;
	
	// Altitude:
	UPROPERTY(BlueprintReadOnly)
	float Altitude = -1.f;
	
	// -1 means uninitialized:
	UPROPERTY(BlueprintReadOnly)
	int32 MyIndex = -1;
	
	UPROPERTY(BlueprintReadOnly)
	EBiome Biome = EBiome::TestGrasslands;
	
	// Foliage data:
	UPROPERTY(BlueprintReadOnly, Category="Foliage")
	TArray<FFoliageInst> FoliageInsts;
	// spawned Foliage:
	UPROPERTY(BlueprintReadOnly, Category="Foliage")
	TArray<AFoliageSprite*> SpawnedFoliage;
	
	// weak ref to my actor in world
	TWeakObjectPtr<AZXCube> MyCube;
};

USTRUCT(BlueprintType)
struct FCellularAutomataOptions
{
	GENERATED_USTRUCT_BODY()

	FCellularAutomataOptions() {}

	UPROPERTY(EditAnywhere)
	int32 NumIterations = 0;
};
