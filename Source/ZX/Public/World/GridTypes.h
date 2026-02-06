#pragma once

#include "CoreMinimal.h"
#include "GridTypes.generated.h"

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
	Dirt,
	Sand,
	Water,
	
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(ETileType, ETileType::Count);

/*
**	Data for a grid tile
*/
USTRUCT(BlueprintType)
struct FGridTile
{
	GENERATED_USTRUCT_BODY()

	FGridTile() {}
	
	FGridTile(ETileType InTileType, float InMoisture, int32 InIdx)
		: Type(InTileType), Moisture(InMoisture), MyIndex(InIdx)
	{}

	// TODO: occupancy.
	bool IsOccupied() { return false; }
	
	// Tile type:
	UPROPERTY(BlueprintReadOnly)
	ETileType Type = ETileType::Grass;
	
	// Moisture:
	UPROPERTY(BlueprintReadOnly)
	float Moisture = -1.f;
	
	// -1 means uninitialized:
	UPROPERTY(BlueprintReadOnly)
	int32 MyIndex = -1;
	
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