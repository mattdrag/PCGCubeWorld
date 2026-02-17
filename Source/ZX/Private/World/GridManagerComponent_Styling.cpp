#include "Core/ZXUtils.h"
#include "Data/BiomeData.h"
#include "Data/ZXAssetManager.h"
#include "World/FoliageSprite.h"
#include "World/GridTypes.h"
#include "World/GridManagerComponent.h"
#include "World/ZXCube.h"
#include "World/ZXSprite.h"

namespace GridManagerStylingConsts
 {
 	constexpr int32 NumFoliageSlots = 8;
	
	const FName SpawnedFoliageFolder = FName("Foliage");
 }

void UGridManagerComponent::LoadBiomes()
{
	UZXAssetManager* AssetManager = UZXUtils::GetZXAssetManager();
	if (AssetManager == nullptr)
	{
		return;
	}
	
	TArray<FPrimaryAssetId> TileStyleIdList;
	AssetManager->GetPrimaryAssetIdList(UZXAssetManager::BiomeDataType, TileStyleIdList);
	for (const FPrimaryAssetId& Id : TileStyleIdList)
	{
		// Get the uobject:
		FAssetData AssetDataToParse;
		AssetManager->GetPrimaryAssetData(Id, AssetDataToParse);

		// Add to map:
		UBiomeData* LocalBiomeData = Cast<UBiomeData>(AssetDataToParse.GetAsset());
		if (LocalBiomeData != nullptr)
		{
			BiomeData.Add(LocalBiomeData->BiomeId, LocalBiomeData);
		}
	}
}

ETileType UGridManagerComponent::DetermineTileType(EBiome InBiome, float InAltitude)
{
	// TODO: right now we have 1 biome, eventually we want to pass it in as a parameter or something..
	TObjectPtr<UBiomeData>* CurrentBiome = BiomeData.Find(InBiome);
	if (CurrentBiome == nullptr || !IsValid(*CurrentBiome))
	{
		LOGZXEF("CurrentBiome configured incorrectly");
		return ETileType::Grass;
	}
	
	// TODO: biome tile type configs are simple at the moment:
	const float SandThresh = (*CurrentBiome)->TileTypeConfig.SandThreshold;
	const float WaterThresh = (*CurrentBiome)->TileTypeConfig.WaterThreshold;
	if (InAltitude > WaterThresh)
	{
		return ETileType::Water;
	}
	if (InAltitude > SandThresh)
	{
		return ETileType::Sand;
	}
	return (*CurrentBiome)->TileTypeConfig.BaseType;
}

void UGridManagerComponent::StyleCube(AZXCube* InCube)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UGridManagerComponent_StyleCube);
	
	if (!IsValid(InCube))
	{
		LOGZXEF("null cube..");
		return;
	}
	// get tile:
	FGridTile* InTile = GetGridTile(InCube->MyGridTileIdx);
	if (InTile == nullptr)
	{
		LOGZXEF("null tile..");
		return;
	}

	const FIntPoint InCoordinate = IndexToCoordinates(InTile->MyIndex);
	
	// Get biome from tile:
	TObjectPtr<UBiomeData>* CurrentBiome = BiomeData.Find(InTile->Biome);
	if (CurrentBiome == nullptr || !IsValid(*CurrentBiome))
	{
		LOGZXEF("CurrentBiome configured incorrectly");
		return;
	}
	
	// make sure it has a material we can style:
	UMaterialInstanceDynamic* DynCubeMat = InCube->GetDynamicMaterial(InTile->Type);
	if (!IsValid(DynCubeMat))
	{
		LOGZXEF("null cube material..");
		return;
	}
	
	// Base is either dirt or sand. if there are any adjacent sand tiles, it becomes sand.
	const bool bIsSand = HasAnyNeighborsOfType(ETileType::Sand, InCoordinate);
	DynCubeMat->SetTextureParameterValue("TopTexture", bIsSand ? (*CurrentBiome)->Sand : (*CurrentBiome)->Dirt);
	DynCubeMat->SetTextureParameterValue("SideTexture", bIsSand ? (*CurrentBiome)->Sand : (*CurrentBiome)->Dirt);
	DynCubeMat->SetVectorParameterValue("BaseShade", GetColorForTile(**CurrentBiome, ETileType::Sand, InTile->Altitude));

	// Grass:
	if (InTile->Type == ETileType::Grass)
	{
		const uint8 TileSheetIndex = Autotile(ETileType::Grass, InCoordinate);
	
		// Top:
		if ((*CurrentBiome)->Grass.IsValidIndex(TileSheetIndex))
		{
			DynCubeMat->SetTextureParameterValue("GrassTexture", (*CurrentBiome)->Grass[TileSheetIndex]);
		}
		
		// Grass shading:
		DynCubeMat->SetVectorParameterValue("GrassShade", GetColorForTile(**CurrentBiome, ETileType::Grass, InTile->Altitude));
	}
	
	// Water:
	if (InTile->Type == ETileType::Water)
	{
		const uint8 TileSheetIndex = Autotile(ETileType::Water, InCoordinate);
		
		if ((*CurrentBiome)->Water.IsValidIndex(TileSheetIndex))
		{
			DynCubeMat->SetTextureParameterValue("WaterTexture", (*CurrentBiome)->Water[TileSheetIndex]);
		}
		
		// Water gets shaded:
		DynCubeMat->SetVectorParameterValue("WaterShade", GetColorForTile(**CurrentBiome, ETileType::Water, InTile->Altitude));
	}
}

uint8 UGridManagerComponent::Autotile(ETileType InType, const FIntPoint& InCoord)
{
	// Get bitmask:
	uint8 NeighborBitmask = 0;
	uint8 i = 0;
	for (const FIntPoint& Neighbor : GridTypesConsts::NeighborArray)
	{
		const FIntPoint NeighborCoordinate = InCoord + Neighbor;
		const FGridTile* NeighborTile = GetGridTile(NeighborCoordinate);
		if (NeighborTile != nullptr && NeighborTile->Type == InType)
		{
			NeighborBitmask |= (1 << (7 - i));
		}
		i++;
	}
	// Convert to tile sheet:
	if (!BitmaskToTileStyle.IsValidIndex(NeighborBitmask))
	{
		UE_LOG(LogZX, Error, TEXT("%s: BitmaskToTileStyle incomplete.."), *FString(__FUNCTION__));
		return 0;
	}
	return BitmaskToTileStyle[NeighborBitmask];
}

bool UGridManagerComponent::HasAnyNeighborsOfType(ETileType InType, const FIntPoint& InCoord)
{
	for (const FIntPoint& Neighbor : GridTypesConsts::NeighborArray)
	{
		const FIntPoint NeighborCoordinate = InCoord + Neighbor;
		const FGridTile* NeighborTile = GetGridTile(NeighborCoordinate);
		if (NeighborTile != nullptr && NeighborTile->Type == InType)
		{
			return true;
		}
	}
	return false;
}

FColor UGridManagerComponent::GetColorForMapTile(int32 InIdx)
{
	// from the tile, we need base color
	FGridTile* InTile = GetGridTile(InIdx);
	if (InTile == nullptr)
	{
		return FColor::Black;
	}
	// Get biome from tile:
	TObjectPtr<UBiomeData>* CurrentBiome = BiomeData.Find(InTile->Biome);
	if (CurrentBiome == nullptr || !IsValid(*CurrentBiome))
	{
		LOGZXEF("CurrentBiome configured incorrectly");
		return FColor::Black;
	}
	
	// for map, we want to take the base color and add the shading:
	return GetColorForTile(**CurrentBiome, InTile->Type, InTile->Altitude, true).ToFColor(false);
}

FLinearColor UGridManagerComponent::GetColorForTile(const UBiomeData& InBiome, ETileType InTileType, float InAltitude, bool bAddBaseColor)
{
	FLinearColor RetColor = FColor::Black;
	
	// Get color range for this tile type:
	const FColorRange* ColorRange = InBiome.TileColorRanges.Find(InTileType);
	if (ColorRange == nullptr)
	{
		LOGZXEF("ColorRange does not contain %d", InTileType);
		return RetColor;
	}
	
	auto CalcShading = [](const FColorRange& ColorRange, float DarkestVal, float LightestVal, float Altitude)
	{
		// grass will go from -1 to sand thresh:
		const float MidPoint = (DarkestVal + LightestVal) / 2;
			
		// Transform to linear colors in order to take diff:
		const bool bDarken = Altitude < MidPoint;
		const FLinearColor TargetColor = bDarken ? ColorRange.Darkest : ColorRange.Lightest;
		const FLinearColor BaseColor = ColorRange.Base;
		const float Alpha = (Altitude - MidPoint) / ((bDarken ? DarkestVal : LightestVal) - MidPoint) ;
		const FLinearColor LerpedColor = FMath::Lerp(BaseColor, TargetColor, Alpha);

		// shading is additive, so we actually want the diff:
		return LerpedColor - BaseColor;
	};
	
	switch (InTileType) 
	{
		case ETileType::Grass:		RetColor = CalcShading(*ColorRange, -1, InBiome.TileTypeConfig.SandThreshold, InAltitude); break;
		case ETileType::Sand:		RetColor = CalcShading(*ColorRange, InBiome.TileTypeConfig.WaterThreshold*2, InBiome.TileTypeConfig.SandThreshold*2, InAltitude); break;
		case ETileType::Water:		RetColor = CalcShading(*ColorRange, 1, InBiome.TileTypeConfig.WaterThreshold, InAltitude); break;
		default:					RetColor = FColor::Black; break;
	}
	
	return bAddBaseColor ? ColorRange->Base + RetColor : RetColor;
}
